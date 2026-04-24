# Data Structure and Algorithm Project

## Overview

This project was developed as part of the final exam for the "Algoritmi e Principi dell'Informatica (API)" course at Politecnico di Milano (2023/2024).

## Motivation

The goal was not only correctness, but efficiency under constraints:

-   Minimize memory usage (no explicit graph representation)
-   Reduce runtime overhead
-   Exploit the ordered nature of station identifiers

## Architecture

Stations are stored in a Binary Search Tree (BST), where each node represents a station identified by its position.

The BST structure is used to:
- Maintain stations in sorted order
- Enable efficient search, insertion, and deletion operations
- Support ordered traversals (in-order and reverse in-order), which are directly exploited by the pathfinding algorithm

Each node also contains a local structure representing the vehicles available at that station, allowing constant-time access to the maximum reachable distance.
This design avoids the need for an explicit graph representation and reduces memory overhead.

## Algorithm Design

The system implements two symmetric pathfinding procedures:

- `carsPath()` for forward traversal (increasing station order)
- `carsPathReverse()` for backward traversal (decreasing station order)

### Core Mechanics

- The tree is traversed in-order (forward) or reverse in-order (backward)
- The path is built incrementally during traversal using a linear structure that stores:
  - current station
  - predecessor
  - maximum reachable distance (car autonomy)
- At each step, the algorithm determines whether the next station is directly reachable or requires extending the path through previously visited nodes

## Result

Final grade: 30/30L
