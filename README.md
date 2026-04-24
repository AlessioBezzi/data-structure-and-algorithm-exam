# Data Structure and Algorithm Project

## Overview

This project was developed as part of the final exam for the "Algoritmi e Principi dell'Informatica (API)" course at Politecnico di Milano (2023/2024).

## Motivation

The goal was not only correctness, but efficiency under constraints:

-   Minimize memory usage (no explicit graph representation)
-   Reduce runtime overhead
-   Exploit the ordered nature of station identifiers

## Architecture

Stations are stored in a binary search tree (BST).

## Algorithm Design

Two algorithms: - carsPath() (forward) - carsPathReverse() (backward)

Key properties:
-   Path built incrementally during traversal
-   Early termination when destination is reached

## Complexity Analysis

Let n = number of stations, k = cars per station.

### Time Complexity

-   BST operations:
    -   Average: O(log n)\
    -   Worst: O(n)
-   Pathfinding:
    -   Forward traversal: O(n) average, O(n²) worst\
    -   Backward traversal: O(n) average, O(n²) worst

Worst-case quadratic behavior occurs only in degenerate layouts
requiring repeated scans over partial paths.

### Space Complexity

-   BST storage: O(n)
-   Cars storage: O(n)
-   Path structure: O(n)

Total space complexity: O(n)


## Input Format

aggiungi-stazione `<id>`{=html} `<num_auto>`{=html} \<autonomie...\>\
demolisci-stazione `<id>`{=html}\
aggiungi-auto `<stazione>`{=html} `<autonomia>`{=html}\
rottama-auto `<stazione>`{=html} `<autonomia>`{=html}\
`<start>`{=html} `<end>`{=html}

## Output

Path: 10 15 20 30

No path: nessun percorso

## Compilation

gcc -O2 -o main main.c

## Result

Final grade: 30/30L
