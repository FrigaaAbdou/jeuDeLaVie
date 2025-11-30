#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>
#include <filesystem>
#include <optional>
#include <memory>
#include <system_error>
#include <ctime>

using namespace std;

// ------------------------------------------------------------
// Core types: CellState hierarchy and Cell
// ------------------------------------------------------------

class CellState {
    public:

        virtual ~CellState() {}
        virtual bool isAlive() const = 0;
        // Deprecated in favor of Rule strategy; kept for compatibility
        virtual CellState* nextState(int /*neighbors*/) const { return nullptr; }
};

class DeadState:public CellState {
    public:
        bool isAlive() const override {
            return false;
        }
};

class AliveState:public CellState {
    public:
        bool isAlive() const override {
            return true;
        }
};



class Cell {
private:
    CellState* state;

public:
    // Constructor
    Cell(CellState* initialState) : state(initialState) {}

    // Destructor
    ~Cell() {
        delete state;
    }

    // Copy constructor (deep copy)
    Cell(const Cell& other) {
        if (other.state == nullptr) {
            state = nullptr;
        } else if (other.state->isAlive()) {
            state = new AliveState();
        } else {
            state = new DeadState();
        }
    }

    // Copy assignment (deep copy)
    Cell& operator=(const Cell& other) {
        if (this != &other) {
            delete state;
            if (other.state == nullptr) {
                state = nullptr;
            } else if (other.state->isAlive()) {
                state = new AliveState();
            } else {
                state = new DeadState();
            }
        }
        return *this;
    }

    // Move constructor
    Cell(Cell&& other) noexcept : state(other.state) {
        other.state = nullptr;
    }

    // Move assignment
    Cell& operator=(Cell&& other) noexcept {
        if (this != &other) {
            delete state;
            state = other.state;
            other.state = nullptr;
        }
        return *this;
    }

    bool isAlive() const {
        return state != nullptr && state->isAlive();
    }

    void setState(CellState* newState) {
        delete state;
        state = newState;
    }

    CellState* getState() const {
        return state;
    }
};


// ------------------------------------------------------------
// Rule hierarchy (independent from states)
// ------------------------------------------------------------
class Rule {
public:
    virtual ~Rule() {}
    // Returns whether the cell will be alive next step
    virtual bool next(bool currentAlive, int aliveNeighbors) const = 0;
};

class ConwayRule : public Rule {
public:
    bool next(bool currentAlive, int aliveNeighbors) const override {
        if (currentAlive) {
            return aliveNeighbors == 2 || aliveNeighbors == 3; // survival
        } else {
            return aliveNeighbors == 3; // birth
        }
    }
};

// ------------------------------------------------------------
// Grid: encapsulates the cell matrix and evolution using Rule
// ------------------------------------------------------------
class Grid {
private:
    int rows;
    int cols;
    vector<vector<Cell>> grid;

public:
    Grid(int r, int c) : rows(r), cols(c) {
        grid.reserve(rows);
        for (int i = 0; i < rows; ++i) {
            vector<Cell> row;
            row.reserve(cols);
            for (int j = 0; j < cols; ++j) {
                row.emplace_back(new DeadState());
            }
            grid.push_back(std::move(row));
        }
    }

    int numRows() const { return rows; }
    int numCols() const { return cols; }

    bool isAlive(int i, int j) const {
        return grid[i][j].isAlive();
    }

    void setAlive(int i, int j, bool alive) {
        if (alive) grid[i][j].setState(new AliveState());
        else grid[i][j].setState(new DeadState());
    }

    int countAliveNeighbors(int i, int j) const {
        int count = 0;
        for (int di = -1; di <= 1; ++di) {
            for (int dj = -1; dj <= 1; ++dj) {
                if (di == 0 && dj == 0) continue;
                int ni = i + di;
                int nj = j + dj;
                if (ni >= 0 && ni < rows && nj >= 0 && nj < cols) {
                    if (grid[ni][nj].isAlive()) count++;
                }
            }
        }
        return count;
    }

    // Computes one evolution step according to rule; returns true if any cell changed
    bool step(const Rule &rule) {
        vector<vector<bool>> nextAlive(rows, vector<bool>(cols, false));
        bool changed = false;
        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j) {
                bool cur = grid[i][j].isAlive();
                int n = countAliveNeighbors(i, j);
                bool nxt = rule.next(cur, n);
                nextAlive[i][j] = nxt;
                if (nxt != cur) changed = true;
            }
        }
        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j) {
                setAlive(i, j, nextAlive[i][j]);
            }
        }
        return changed;
    }

    bool equals(const Grid &other) const {
        if (rows != other.rows || cols != other.cols) return false;
        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j) {
                if (isAlive(i, j) != other.isAlive(i, j)) return false;
            }
        }
        return true;
    }

    void randomize(int aliveEvery = 4) {
        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j) {
                bool alive = (rand() % aliveEvery) == 0;
                setAlive(i, j, alive);
            }
        }
    }

    static optional<Grid> fromFile(const string &path) {
        ifstream in(path);
        if (!in) return nullopt;
        int r = 0, c = 0;
        if (!(in >> r >> c)) return nullopt;
        Grid g(r, c);
        for (int i = 0; i < r; ++i) {
            for (int j = 0; j < c; ++j) {
                int v = 0;
                if (!(in >> v)) return nullopt;
                g.setAlive(i, j, v != 0);
            }
        }
        return g;
    }

    bool writeToFile(const string &path) const {
        ofstream out(path);
        if (!out) return false;
        out << rows << " " << cols << "\n";
        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j) {
                out << (isAlive(i, j) ? 1 : 0);
                if (j + 1 < cols) out << ' ';
            }
            out << "\n";
        }
        return true;
    }
};

// ------------------------------------------------------------
// Game: orchestrates a Grid with a Rule
// ------------------------------------------------------------
class Game {
private:
    Grid grid;
    unique_ptr<Rule> rule;
public:
    Game(Grid g, unique_ptr<Rule> r)
        : grid(std::move(g)), rule(std::move(r)) {}

    Grid& getGrid() { return grid; }
    const Grid& getGrid() const { return grid; }

    bool step() {
        return grid.step(*rule);
    }
};

// ------------------------------------------------------------
// Test utilities (simple unit tests)
// ------------------------------------------------------------
static bool assertEvolution(Grid start, const Grid &expected, int iterations, const Rule &rule) {
    for (int i = 0; i < iterations; ++i) {
        start.step(rule);
    }
    return start.equals(expected);
}

static bool runUnitTests() {
    ConwayRule rule;

    // Test 1: Block (still life) remains unchanged after 3 steps
    {
        Grid start(4, 4);
        start.setAlive(1, 1, true);
        start.setAlive(1, 2, true);
        start.setAlive(2, 1, true);
        start.setAlive(2, 2, true);
        Grid expected = start; // copy
        if (!assertEvolution(start, expected, 3, rule)) {
            cerr << "Test failed: Block still life" << endl;
            return false;
        }
    }

    // Test 2: Blinker oscillates with period 2
    {
        Grid start(5, 5);
        start.setAlive(2, 1, true);
        start.setAlive(2, 2, true);
        start.setAlive(2, 3, true);

        Grid expected(5, 5);
        expected.setAlive(1, 2, true);
        expected.setAlive(2, 2, true);
        expected.setAlive(3, 2, true);

        if (!assertEvolution(start, expected, 1, rule)) {
            cerr << "Test failed: Blinker step 1" << endl;
            return false;
        }
        // After 2 steps, return to original
        if (!assertEvolution(start, start, 2, rule)) {
            cerr << "Test failed: Blinker period 2" << endl;
            return false;
        }
    }

    cout << "All tests passed." << endl;
    return true;
}

// ------------------------------------------------------------
// Console mode
// ------------------------------------------------------------
static int runConsoleMode(const string &inputPath, int maxIterations) {
    auto og = Grid::fromFile(inputPath);
    if (!og) {
        cerr << "Failed to read input file: " << inputPath << endl;
        return 1;
    }
    Game game(*og, make_unique<ConwayRule>());

    namespace fs = std::filesystem;
    fs::path inPath(inputPath);
    string base = inPath.stem().string();
    fs::path outDir = inPath.parent_path() / (base + string("_out"));
    error_code ec;
    fs::create_directories(outDir, ec);
    if (ec) {
        cerr << "Failed to create output directory: " << outDir << endl;
        return 1;
    }

    bool changed = true;
    for (int iter = 1; iter <= maxIterations && changed; ++iter) {
        changed = game.step();
        fs::path outFile = outDir / (to_string(iter) + ".txt");
        if (!game.getGrid().writeToFile(outFile.string())) {
            cerr << "Failed to write iteration file: " << outFile << endl;
            return 1;
        }
    }
    return 0;
}

// ------------------------------------------------------------
// GUI rendering helpers
// ------------------------------------------------------------
static void drawGrid(sf::RenderWindow &window, const Grid &grid, int cellSize, int yOffset) {
    sf::RectangleShape cellShape(sf::Vector2f(cellSize - 1, cellSize - 1));
    for (int i = 0; i < grid.numRows(); ++i) {
        for (int j = 0; j < grid.numCols(); ++j) {
            cellShape.setPosition(sf::Vector2f(j * cellSize, yOffset + i * cellSize));
            if (grid.isAlive(i, j)) {
                cellShape.setFillColor(sf::Color::White);
            } else {
                cellShape.setFillColor(sf::Color(30, 30, 30));
            }
            window.draw(cellShape);
        }
    }
}

// Reload grid from file and resize window to fit
static bool reloadFromFileAndResize(const string &path,
                                    Game &game,
                                    sf::RenderWindow &window,
                                    int cellSize,
                                    int uiBar,
                                    int &rows,
                                    int &cols) {
    auto g = Grid::fromFile(path);
    if (!g) {
        cerr << "Failed to reload input file: " << path << endl;
        return false;
    }
    // Replace game's grid and rule
    game = Game(std::move(*g), make_unique<ConwayRule>());
    rows = game.getGrid().numRows();
    cols = game.getGrid().numCols();
    // Resize window to fit new grid (plus UI bar)
    window.setSize(sf::Vector2u(cols * cellSize, rows * cellSize + uiBar));
    return true;
}

static void printUsage(const char* argv0) {
    cerr << "Usage:\n"
         << "  " << argv0 << " --console <iterations> <input_file>\n"
         << "  " << argv0 << " [--step <seconds>] [<input_file>]\n"
         << "  " << argv0 << " --test\n";
}

int main(int argc, char** argv) {
    srand(static_cast<unsigned>(time(nullptr)));

    // Arg parsing
    if (argc > 1) {
        string mode = argv[1];
        if (mode == string("--test")) {
            return runUnitTests() ? 0 : 1;
        }
        if (mode == string("--console")) {
            if (argc < 4) {
                printUsage(argv[0]);
                return 1;
            }
            int iterations = stoi(argv[2]);
            string inputPath = argv[3];
            return runConsoleMode(inputPath, iterations);
        }
    }

    // GUI mode
    // Defaults
    int rows = 30;
    int cols = 40;
    int CELL_SIZE = 20;
    float stepTime = 0.2f; // seconds between generations
    const int UI_BAR = 28; // pixels reserved for a top UI bar

    // Optional args: [--step <seconds>] [<input_file>]
    string inputPath;
    for (int i = 1; i < argc; ++i) {
        string a = argv[i];
        if (a == string("--step") && i + 1 < argc) {
            stepTime = max(0.01f, static_cast<float>(atof(argv[++i])));
        } else {
            inputPath = a;
        }
    }

    optional<Grid> og;
    if (!inputPath.empty()) {
        og = Grid::fromFile(inputPath);
        if (!og) {
            cerr << "Failed to open input file: " << inputPath << ". Falling back to random grid." << endl;
        }
    }

    Grid initGrid = og ? *og : Grid(rows, cols);
    if (!og) {
        initGrid.randomize(4); // ~25% alive
    } else {
        rows = initGrid.numRows();
        cols = initGrid.numCols();
    }

    Game game(std::move(initGrid), make_unique<ConwayRule>());

    sf::RenderWindow window(
        sf::VideoMode(sf::Vector2u(cols * CELL_SIZE, rows * CELL_SIZE + UI_BAR)),
        "Game of Life"
    );
    window.setFramerateLimit(60);

    bool paused = false;
    sf::Clock clock;
    float accumulator = 0.f;
    // iteration counter not displayed; removed to avoid unused warnings

    // Simple on-screen UI bar + Refresh button (top). Click to reload from file.
    sf::RectangleShape topBar;
    topBar.setSize(sf::Vector2f(static_cast<float>(cols * CELL_SIZE), static_cast<float>(UI_BAR)));
    topBar.setPosition(sf::Vector2f(0.f, 0.f));
    topBar.setFillColor(sf::Color(40, 40, 40));
    topBar.setOutlineThickness(0.f);

    sf::RectangleShape refreshButton;
    refreshButton.setSize(sf::Vector2f(90.f, UI_BAR - 8.f));
    refreshButton.setPosition(sf::Vector2f(8.f, 4.f));
    refreshButton.setFillColor(sf::Color(70, 140, 200, 220));
    refreshButton.setOutlineThickness(1.f);
    refreshButton.setOutlineColor(sf::Color(20, 20, 20));

    while (window.isOpen()) {
        // --- Events ---
        while (auto event = window.pollEvent()) {
            // Window close
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }

            // Key pressed
            if (auto keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                if (keyPressed->code == sf::Keyboard::Key::Space) {
                    paused = !paused; // toggle pause
                }
                if (keyPressed->code == sf::Keyboard::Key::R) {
                    // Reset random pattern (keeps same dimensions)
                    game.getGrid().randomize(4);
                }
                if (keyPressed->code == sf::Keyboard::Key::F) {
                    // Refresh from input file if provided
                    if (!inputPath.empty()) {
                        if (reloadFromFileAndResize(inputPath, game, window, CELL_SIZE, UI_BAR, rows, cols)) {
                            paused = false; // immediately relaunch from initial state
                            accumulator = 0.f;
                        }
                    }
                }
                if (keyPressed->code == sf::Keyboard::Key::N) {
                    // Step once when paused
                    if (paused) { game.step(); }
                }
                if (keyPressed->code == sf::Keyboard::Key::Up) {
                    stepTime = max(0.01f, stepTime - 0.02f);
                }
                if (keyPressed->code == sf::Keyboard::Key::Down) {
                    stepTime = min(2.0f, stepTime + 0.02f);
                }
            }

            // Mouse click: check refresh button
            if (auto mousePressed = event->getIf<sf::Event::MouseButtonPressed>()) {
                if (mousePressed->button == sf::Mouse::Button::Left) {
                    sf::Vector2i m = sf::Mouse::getPosition(window);
                    sf::Vector2f mf(static_cast<float>(m.x), static_cast<float>(m.y));
                    if (refreshButton.getGlobalBounds().contains(mf)) {
                        if (!inputPath.empty()) {
                            if (reloadFromFileAndResize(inputPath, game, window, CELL_SIZE, UI_BAR, rows, cols)) {
                                paused = false;
                                accumulator = 0.f;
                            }
                        }
                    }
                }
            }
        }

        // --- Update simulation ---
        float dt = clock.restart().asSeconds();
        accumulator += dt;

        if (!paused && accumulator >= stepTime) {
            accumulator = 0.f;
            bool changed = game.step();
            // If stable, pause (meets termination on stability requirement)
            if (!changed) paused = true;
        }

        // --- Draw ---
        window.clear(sf::Color::Black);
        // top bar
        window.draw(topBar);
        // grid below the bar
        drawGrid(window, game.getGrid(), CELL_SIZE, UI_BAR);
        // Draw the button on the bar (only if a file provided)
        if (!inputPath.empty()) {
            window.draw(refreshButton);
        }
        window.display();
    }

    return 0;
}
