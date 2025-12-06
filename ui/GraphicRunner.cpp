#include "GraphicRunner.h"
#include "FileGridLoader.h"
#include "SimulationService.h"
#include "ConwayRule.h"
#include <SFML/Graphics.hpp>
#include <cctype>
#include <filesystem>
#include <functional>
#include <optional>
#include <random>
#include <vector>
#include <iostream>

static bool loadFont(sf::Font& font) {
    // Try bundled pixel font first, then common system fonts; fall back to title-only if none found
    const std::vector<std::filesystem::path> candidates = {
        "assets/fonts/pixel.ttf",
#if defined(__APPLE__)
        "/System/Library/Fonts/Supplemental/Arial.ttf",
        "/System/Library/Fonts/Supplemental/Helvetica.ttc",
        "/Library/Fonts/Arial.ttf",
#endif
        "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
        "/usr/share/fonts/dejavu/DejaVuSans.ttf",
        "/usr/share/fonts/TTF/DejaVuSans.ttf"
    };

    for (const auto& path : candidates) {
        if (std::filesystem::exists(path)) {
            if (font.openFromFile(path)) return true;
        }
    }
    return false;
}

static bool pointInRect(const sf::Vector2f& p, const sf::FloatRect& rect) {
    return rect.contains(p);
}

static void drawGrid(sf::RenderWindow& window,
                     const Grid& grid,
                     int cellSize)
{
    sf::RectangleShape rect(sf::Vector2f(cellSize - 1, cellSize - 1));
    for (int r = 0; r < grid.rows(); ++r) {
        for (int c = 0; c < grid.cols(); ++c) {
            rect.setPosition(sf::Vector2f(static_cast<float>(c * cellSize),
                                          static_cast<float>(r * cellSize)));
            rect.setFillColor(grid.at(r,c).isAlive()
                              ? sf::Color::White
                              : sf::Color(30,30,30));
            window.draw(rect);
        }
    }
}

void GraphicRunner::run(const SimulationConfig& config) {
    try {
        // --- MENU STATE ---
        sf::Font font;
        const bool haveFont = loadFont(font);

        std::string inputPath = config.inputFile.empty() ? "state.txt" : config.inputFile;
        std::string maxIterStr = std::to_string(config.maxIterations);
        std::string errorMsg;
        enum class Field { File, Iter };
        Field activeField = Field::File;

        sf::RenderWindow window(sf::VideoMode(sf::Vector2u(900, 600)),
                                "Jeu de la vie - Menu");
        window.setFramerateLimit(60);

        const sf::Vector2u menuSize = window.getSize();
        const int bgCellSize = 16;
        const int bgCols = static_cast<int>(menuSize.x) / bgCellSize + 1;
        const int bgRows = static_cast<int>(menuSize.y) / bgCellSize + 1;
        std::vector<uint8_t> bgCells(bgCols * bgRows, 0);
        std::mt19937 rng(static_cast<unsigned>(std::random_device{}()));
        std::uniform_real_distribution<float> flicker(0.f, 1.f);
        sf::Clock bgClock;

        auto drawText = [&](const std::string& s, float x, float y, unsigned size,
                            sf::Color color, float letterSpacing = 1.f,
                            sf::Color outline = sf::Color::Transparent,
                            float outlineThickness = 0.f,
                            bool center = false) {
            if (!haveFont) return;
            sf::Text t(font, s, size);
            t.setFillColor(color);
            t.setLetterSpacing(letterSpacing);
            t.setOutlineColor(outline);
            t.setOutlineThickness(outlineThickness);
            if (center) {
                const sf::FloatRect b = t.getLocalBounds();
                t.setOrigin(sf::Vector2f(b.position.x + b.size.x * 0.5f,
                                         b.position.y + b.size.y * 0.5f)); // centrer
            }
            t.setPosition(sf::Vector2f(x, y));
            window.draw(t);
        };

        auto renderMenu = [&]() {
            // Background flicker grid for "game" feel
            if (bgClock.getElapsedTime().asSeconds() > 0.12f) {
                bgClock.restart();
                for (int r = 0; r < bgRows; ++r) {
                    for (int c = 0; c < bgCols; ++c) {
                        float p = flicker(rng);
                        if (p < 0.02f) bgCells[r * bgCols + c] = 1;
                        else if (p < 0.04f) bgCells[r * bgCols + c] = 0;
                    }
                }
            }

            window.clear(sf::Color(12, 14, 22));

            sf::RectangleShape cell(sf::Vector2f(static_cast<float>(bgCellSize - 1),
                                                 static_cast<float>(bgCellSize - 1)));
            for (int r = 0; r < bgRows; ++r) {
                for (int c = 0; c < bgCols; ++c) {
                    cell.setPosition(sf::Vector2f(static_cast<float>(c * bgCellSize),
                                                  static_cast<float>(r * bgCellSize)));
                    cell.setFillColor(bgCells[r * bgCols + c]
                                      ? sf::Color(60, 140, 190, 200)
                                      : sf::Color(30, 34, 46, 160));
                    window.draw(cell);
                }
            }

            // Overlay to soften background
            sf::RectangleShape overlay(sf::Vector2f(static_cast<float>(menuSize.x),
                                                    static_cast<float>(menuSize.y)));
            overlay.setFillColor(sf::Color(0, 0, 0, 90));
            window.draw(overlay);

            // Title & subtitle (centered)
            drawText("JEU DE LA VIE", menuSize.x * 0.5f, 90.f, 64,
                     sf::Color::White, 1.4f, sf::Color(0, 0, 0, 150), 3.f, true);
            drawText("Configurer et lancer la simulation", menuSize.x * 0.5f, 150.f,
                     22, sf::Color(200, 200, 210), 1.0f, sf::Color::Transparent, 0.f, true);

            // Panel (menu card)
            const float panelW = 820.f;
            const float panelH = 260.f;
            sf::RectangleShape panel(sf::Vector2f(panelW, panelH));
            panel.setOrigin(panel.getSize() * 0.5f);
            panel.setPosition(sf::Vector2f(menuSize.x * 0.5f, menuSize.y * 0.48f));
            panel.setFillColor(sf::Color(30, 32, 44, 220));
            panel.setOutlineThickness(2.f);
            panel.setOutlineColor(sf::Color(80, 120, 180, 180));
            window.draw(panel);

            float leftX = panel.getPosition().x - panel.getSize().x / 2.f + 40.f;
            float row1Y = panel.getPosition().y - 60.f;
            float row2Y = panel.getPosition().y + 30.f;

            drawText("Fichier d'entree :", leftX + 130.f, row1Y - 18.f, 20,
                     sf::Color(180, 190, 210), 1.0f, sf::Color::Transparent, 0.f, true);
            sf::RectangleShape fileBox(sf::Vector2f(500.f, 46.f));
            fileBox.setPosition(sf::Vector2f(leftX + 130.f, row1Y));
            fileBox.setFillColor(sf::Color(50, 55, 70, 230));
            fileBox.setOutlineThickness(1.5f);
            fileBox.setOutlineColor(sf::Color(90, 110, 150));
            if (activeField == Field::File)
                fileBox.setOutlineColor(sf::Color(120, 150, 200));
            window.draw(fileBox);
            drawText(inputPath + (activeField == Field::File ? "|" : ""),
                     fileBox.getPosition().x + fileBox.getSize().x / 2.f,
                     fileBox.getPosition().y + fileBox.getSize().y / 2.f + 2.f,
                     18, sf::Color::White, 1.0f, sf::Color::Transparent, 0.f, true);

            drawText("Iterations max :", leftX + 130.f, row2Y - 18.f, 20,
                     sf::Color(180, 190, 210), 1.0f, sf::Color::Transparent, 0.f, true);
            sf::RectangleShape iterBox(sf::Vector2f(180.f, 46.f));
            iterBox.setPosition(sf::Vector2f(leftX + 130.f, row2Y));
            iterBox.setFillColor(sf::Color(50, 55, 70, 230));
            iterBox.setOutlineThickness(1.5f);
            iterBox.setOutlineColor(sf::Color(90, 110, 150));
            if (activeField == Field::Iter)
                iterBox.setOutlineColor(sf::Color(120, 150, 200));
            window.draw(iterBox);
            drawText(maxIterStr + (activeField == Field::Iter ? "|" : ""),
                     iterBox.getPosition().x + iterBox.getSize().x / 2.f,
                     iterBox.getPosition().y + iterBox.getSize().y / 2.f + 2.f,
                     18, sf::Color::White, 1.0f, sf::Color::Transparent, 0.f, true);

            // Buttons
            sf::Vector2f btnSize(240.f, 64.f);
            float btnY = menuSize.y * 0.78f;
            sf::Vector2f startPos(menuSize.x * 0.5f - btnSize.x - 20.f, btnY);
            sf::Vector2f quitPos(menuSize.x * 0.5f + 20.f, btnY);

            sf::Vector2i mp = sf::Mouse::getPosition(window);
            sf::Vector2f mpos(static_cast<float>(mp.x), static_cast<float>(mp.y));
            sf::FloatRect startRect(startPos, btnSize);
            sf::FloatRect quitRect(quitPos, btnSize);

            sf::Color startBase(70, 170, 110);
            sf::Color startHover(90, 200, 140);
            sf::Color quitBase(190, 80, 80);
            sf::Color quitHover(220, 110, 110);

            sf::RectangleShape startBtn(btnSize);
            startBtn.setPosition(startPos);
            startBtn.setFillColor(startRect.contains(mpos) ? startHover : startBase);
            startBtn.setOutlineThickness(2.f);
            startBtn.setOutlineColor(sf::Color(0, 0, 0, 120));
            window.draw(startBtn);
            drawText("Demarrer", startPos.x + btnSize.x / 2.f,
                     startPos.y + btnSize.y / 2.f, 24, sf::Color::White, 1.1f,
                     sf::Color::Transparent, 0.f, true);

            sf::RectangleShape quitBtn(btnSize);
            quitBtn.setPosition(quitPos);
            quitBtn.setFillColor(quitRect.contains(mpos) ? quitHover : quitBase);
            quitBtn.setOutlineThickness(2.f);
            quitBtn.setOutlineColor(sf::Color(0, 0, 0, 120));
            window.draw(quitBtn);
            drawText("Quitter", quitPos.x + btnSize.x / 2.f,
                     quitPos.y + btnSize.y / 2.f, 24, sf::Color::White, 1.1f,
                     sf::Color::Transparent, 0.f, true);

            // Footer / shortcuts
            drawText("Raccourcis : Espace (pause), N (pas a pas)  |  v1.0",
                     20.f, static_cast<float>(menuSize.y) - 24.f, 14,
                     sf::Color(170, 170, 180));

            if (!errorMsg.empty()) {
                drawText(errorMsg, menuSize.x * 0.5f, menuSize.y * 0.9f, 16,
                         sf::Color(220, 80, 80), 1.0f, sf::Color::Black, 1.f, true);
            }

            window.display();
        };

        // Menu loop
        bool inMenu = true;
        std::function<void()> startSimulation; // forward declaration

        startSimulation = [&]() {
            errorMsg.clear();
            int parsedIter = config.maxIterations;
            try {
                parsedIter = std::stoi(maxIterStr);
                if (parsedIter <= 0) throw std::invalid_argument("non positive");
            } catch (...) {
                errorMsg = "Iterations max invalides (entier > 0 attendu)";
                return;
            }

            try {
                SimulationConfig cfg = config;
                cfg.inputFile = inputPath;
                cfg.maxIterations = parsedIter;
                FileGridLoader loader;
                SimulationService service(cfg, loader, nullptr, std::make_unique<ConwayRule>());

                const int cellSize = 20;
                window.create(sf::VideoMode(sf::Vector2u(
                    static_cast<unsigned int>(service.currentGrid().cols() * cellSize),
                    static_cast<unsigned int>(service.currentGrid().rows() * cellSize))),
                    "Game of Life - Iteration 0");

                auto updateTitle = [&window, &service]() {
                    window.setTitle("Game of Life - Iteration " +
                                    std::to_string(service.currentIteration()));
                };
                updateTitle();

                std::optional<sf::Text> iterationText;
                if (haveFont) {
                    iterationText.emplace(font, "", 18);
                    iterationText->setFillColor(sf::Color::Green);
                    iterationText->setPosition(sf::Vector2f(5.f, 5.f));
                }

                window.setFramerateLimit(30);
                sf::Clock clock;
                float stepTime = 0.2f;
                float accumulator = 0.f;
                bool paused = false;
                inMenu = false;

                while (window.isOpen()) {
                    while (const std::optional<sf::Event> event = window.pollEvent()) {
                        if (event->is<sf::Event::Closed>()) {
                            window.close();
                        }
                        if (const auto* keyPress = event->getIf<sf::Event::KeyPressed>()) {
                            if (keyPress->code == sf::Keyboard::Key::Space)
                                paused = !paused;
                            if (keyPress->code == sf::Keyboard::Key::N && paused) {
                                service.step();
                                updateTitle();
                            }
                        }
                    }

                    float dt = clock.restart().asSeconds();
                    accumulator += dt;

                    if (!paused && accumulator >= stepTime && !service.hasFinished()) {
                        accumulator = 0.f;
                        service.step();
                        updateTitle();
                    }

                    window.clear(sf::Color::Black);
                    drawGrid(window, service.currentGrid(), cellSize);
                    if (iterationText.has_value()) {
                        iterationText->setString("Iteration: " +
                                                 std::to_string(service.currentIteration()));
                        window.draw(*iterationText);
                    }
                    window.display();
                }
            }
            catch (const std::exception& ex) {
                errorMsg = ex.what();
            }
        };

        while (window.isOpen() && inMenu) {
            while (const std::optional<sf::Event> event = window.pollEvent()) {
                if (event->is<sf::Event::Closed>()) {
                    window.close();
                }
                if (const auto* keyPress = event->getIf<sf::Event::KeyPressed>()) {
                    if (keyPress->code == sf::Keyboard::Key::Enter) {
                        startSimulation();
                    }
                    if (keyPress->code == sf::Keyboard::Key::Tab) {
                        activeField = (activeField == Field::File) ? Field::Iter : Field::File;
                    }
                    if (keyPress->code == sf::Keyboard::Key::Escape) {
                        window.close();
                    }
                    if (keyPress->code == sf::Keyboard::Key::Backspace) {
                        if (activeField == Field::File) {
                            if (!inputPath.empty()) inputPath.pop_back();
                        } else {
                            if (!maxIterStr.empty()) maxIterStr.pop_back();
                        }
                    }
                }
                if (const auto* textEvt = event->getIf<sf::Event::TextEntered>()) {
                    char32_t u = textEvt->unicode;
                    if (u >= 32 && u < 127) {
                        char c = static_cast<char>(u);
                        if (activeField == Field::File) {
                            inputPath.push_back(c);
                        } else if (std::isdigit(static_cast<unsigned char>(c))) {
                            maxIterStr.push_back(c);
                        }
                    }
                }
                if (const auto* mouseEvt = event->getIf<sf::Event::MouseButtonPressed>()) {
                    sf::Vector2f mpos(static_cast<float>(mouseEvt->position.x),
                                      static_cast<float>(mouseEvt->position.y));
                    sf::Vector2f btnSize(240.f, 64.f);
                    float btnY = window.getSize().y * 0.78f;
                    sf::FloatRect startRect(sf::Vector2f(window.getSize().x * 0.5f - btnSize.x - 20.f, btnY),
                                            btnSize);
                    sf::FloatRect quitRect(sf::Vector2f(window.getSize().x * 0.5f + 20.f, btnY),
                                           btnSize);

                    const float panelW = 820.f;
                    sf::Vector2f panelCenter(window.getSize().x * 0.5f, window.getSize().y * 0.48f);
                    float leftX = panelCenter.x - panelW / 2.f + 40.f + 130.f;
                    float row1Y = panelCenter.y - 60.f;
                    float row2Y = panelCenter.y + 30.f;
                    sf::FloatRect fileRect(sf::Vector2f(leftX, row1Y),
                                           sf::Vector2f(500.f, 46.f));
                    sf::FloatRect iterRect(sf::Vector2f(leftX, row2Y),
                                           sf::Vector2f(180.f, 46.f));

                    if (pointInRect(mpos, startRect)) startSimulation();
                    if (pointInRect(mpos, quitRect)) window.close();
                    if (pointInRect(mpos, fileRect)) activeField = Field::File;
                    if (pointInRect(mpos, iterRect)) activeField = Field::Iter;
                }
            }
            if (inMenu) renderMenu();
        }
    }
    catch (const std::exception& ex) {
        std::cerr << "Erreur: " << ex.what() << "\n";
    }
}
