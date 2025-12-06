#include "Grid.h"
#include "GameOfLife.h"
#include "ConwayRule.h"
#include "AliveState.h"
#include "DeadState.h"
#include <cassert>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

// Minimal assertion helper for tests
static void expect(bool condition, const std::string& message) {
    if (!condition) {
        std::cerr << "Test failed: " << message << "\n";
        std::exit(1);
    }
}

static void logCase(const std::string& label) {
    std::cout << "[CASE] " << label << "... ";
}

static void logOk() {
    std::cout << "OK\n";
}

// Build a grid from a vector of strings composed of '1' (alive) and '0' (dead)
static Grid makeGrid(const std::vector<std::string>& rows) {
    int r = static_cast<int>(rows.size());
    int c = static_cast<int>(rows.empty() ? 0 : rows.front().size());
    Grid g(r, c, false);
    for (int y = 0; y < r; ++y) {
        for (int x = 0; x < c; ++x) {
            if (rows[y][x] == '1')
                g.at(y, x).setState(new AliveState());
            else
                g.at(y, x).setState(new DeadState());
        }
    }
    return g;
}

// Compare grid to expected pattern
static void expectGrid(const Grid& g, const std::vector<std::string>& rows, const std::string& msg) {
    expect(static_cast<int>(rows.size()) == g.rows(), msg + " (row count)");
    expect(static_cast<int>(rows.front().size()) == g.cols(), msg + " (col count)");
    for (int y = 0; y < g.rows(); ++y) {
        for (int x = 0; x < g.cols(); ++x) {
            bool alive = rows[y][x] == '1';
            if (g.at(y, x).isAlive() != alive) {
                std::cerr << "Mismatch at (" << y << "," << x << ") in " << msg << "\n";
                std::exit(1);
            }
        }
    }
}

static void test_block_is_stable() {
    logCase("Still life (block) remains stable");
    // Still life block should remain unchanged and report stability
    Grid g = makeGrid({
        "0000",
        "0110",
        "0110",
        "0000"
    });
    GameOfLife life(g, std::make_unique<ConwayRule>(), 10);
    life.step();
    expectGrid(life.currentGrid(), {
        "0000",
        "0110",
        "0110",
        "0000"
    }, "block still life");
    expect(life.isStable(), "block should be stable after one step");
    expect(life.hasFinished(), "block should finish on stability");
    logOk();
}

static void test_blinker_oscillates() {
    logCase("Blinker oscillates with period 2");
    // Blinker period-2 oscillator
    Grid g = makeGrid({
        "00000",
        "00000",
        "01110",
        "00000",
        "00000"
    });
    GameOfLife life(g, std::make_unique<ConwayRule>(), 10);
    life.step();
    expectGrid(life.currentGrid(), {
        "00000",
        "00100",
        "00100",
        "00100",
        "00000"
    }, "blinker phase 2");
    life.step();
    expectGrid(life.currentGrid(), {
        "00000",
        "00000",
        "01110",
        "00000",
        "00000"
    }, "blinker phase 1 return");
    logOk();
}

static void test_lonely_cell_dies() {
    logCase("Isolated live cell dies in one step");
    Grid g = makeGrid({
        "000",
        "010",
        "000"
    });
    GameOfLife life(g, std::make_unique<ConwayRule>(), 5);
    life.step();
    expectGrid(life.currentGrid(), {
        "000",
        "000",
        "000"
    }, "lonely cell should die");
    logOk();
}

int main() {
    std::cout << "Running Game of Life unit tests...\n";
    test_block_is_stable();
    test_blinker_oscillates();
    test_lonely_cell_dies();
    std::cout << "All tests passed.\n";
    return 0;
}
