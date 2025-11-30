# UML Class Diagram Explanation — Game of Life (Basic OOP Architecture)

This document explains the UML diagram representing the first set of classes implemented for the Game of Life project.
The UML corresponds to the following classes:

* `CellState` (abstract)
* `AliveState`
* `DeadState`
* `Cell`

---

# 1. Overview

The goal of this UML model is to represent a **polymorphic state system**, where each `Cell` does not store a boolean (alive/dead), but instead stores an **object representing its state**.

This design follows good Object-Oriented Programming principles, especially:

* **Encapsulation**
* **Inheritance**
* **Polymorphism**
* **Single Responsibility Principle (SRP)**

---

# 2. Class Descriptions

### 2.1. `CellState` (Abstract Class)

This abstract class represents the *general concept of a cell state*.
It defines two pure virtual functions:

| Method                                      | Description                                                     |
| ------------------------------------------- | --------------------------------------------------------------- |
| `bool isAlive() const`                      | Must return whether the state represents a living cell.         |
| `CellState* nextState(int neighbors) const` | Computes the next state based on the number of alive neighbors. |

➡️ Because it is abstract, you **cannot create** a `CellState` object directly.
Instead, you must use derived classes (`AliveState` or `DeadState`).

---

### 2.2. `AliveState` (Concrete Class)

Represents a cell **currently alive**.

Implements:

* `isAlive()` → returns `true`
* `nextState(neighbors)` → implements Conway’s survival/death rules

Rules applied:

* If neighbors == 2 or 3 → remains alive
* Else → becomes dead

➡️ This class models the behavior of a living cell.

---

### 2.3. `DeadState` (Concrete Class)

Represents a cell **currently dead**.

Implements:

* `isAlive()` → returns `false`
* `nextState(neighbors)` → implements Conway’s birth rule

Rules applied:

* If neighbors == 3 → becomes alive
* Else → stays dead

➡️ This class models the behavior of a dead/inactive cell.

---

### 2.4. `Cell` (Concrete Class)

Represents **one cell** in the grid.

It contains:

* `CellState* state;` → pointer to current state

Methods:

* `isAlive()` → delegates to state
* `setState(newState)` → replaces its state
* `getState()` → returns the pointer
* destructor frees memory

➡️ The cell does **not** contain logic itself.
It *delegates all behavior* to its state object (polymorphism).

This allows:

* changing rules without modifying `Cell`
* adding new states (e.g., **ObstacleState**) easily
* more flexible and clean architecture

---

# 3. Relationships Between Classes

### Inheritance

```
CellState <|-- AliveState
CellState <|-- DeadState
```

This means both concrete classes **derive from** the abstract base class.

### Composition / Aggregation

```
Cell --> CellState
```

A `Cell` **has a** `CellState`.
This is a composition-like relationship: the `Cell` manages the lifetime of its state object.

---

# 4. Why This Architecture?

Using polymorphism offers several advantages:

### ✔ Cleaner code

No `if (alive) { ... } else { ... }`

### ✔ Extendable

Add an `ObstacleState` or future rules without touching existing classes.

### ✔ SOLID compliant

* **Single Responsibility**
  Each state knows its own rules.
* **Open/Closed Principle**
  You can add new states without modifying existing code.

### ✔ Perfect for your project requirements

The instructions require using:

* inheritance
* polymorphism
* clean object design

This architecture fully satisfies those constraints.


---

# Conclusion

This UML establishes the **core backbone** of the OOP Game of Life implementation.
From here, we can safely extend the design with:

* `Grid`
* `Game`
* `Rule` hierarchy
* SFML renderer
* unit tests
* toroidal grid
* obstacles
