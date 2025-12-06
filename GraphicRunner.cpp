#include "GraphicRunner.h"
#include "InitialStateLoader.h"
#include "GameOfLife.h"
#include "ConwayRule.h"
#include <SFML/Graphics.hpp>
#include <optional>
#include <iostream>

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
        Grid grid = InitialStateLoader::loadFromFile(config.inputFile,
                                                     config.toroidal);
        GameOfLife game(grid, std::make_unique<ConwayRule>(), config.maxIterations);

        const int cellSize = 20;
        sf::RenderWindow window(
            sf::VideoMode(sf::Vector2u(
                static_cast<unsigned int>(grid.cols() * cellSize),
                static_cast<unsigned int>(grid.rows() * cellSize))),
            "Game of Life");

        window.setFramerateLimit(30);
        sf::Clock clock;
        float stepTime = 0.2f;
        float accumulator = 0.f;
        bool paused = false;

        while (window.isOpen()) {
            while (const std::optional<sf::Event> event = window.pollEvent()) {
                if (event->is<sf::Event::Closed>())
                    window.close();
                if (const auto* keyPress = event->getIf<sf::Event::KeyPressed>()) {
                    if (keyPress->code == sf::Keyboard::Key::Space)
                        paused = !paused;
                    if (keyPress->code == sf::Keyboard::Key::N && paused)
                        game.step();
                }
            }

            float dt = clock.restart().asSeconds();
            accumulator += dt;

            if (!paused && accumulator >= stepTime && !game.hasFinished()) {
                accumulator = 0.f;
                game.step();
            }

            window.clear(sf::Color::Black);
            drawGrid(window, game.currentGrid(), cellSize);
            window.display();
        }
    }
    catch (const std::exception& ex) {
        std::cerr << "Erreur: " << ex.what() << "\n";
    }
}
