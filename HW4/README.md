# HW4: Standard-Cell Placement Legalization

## Overview
This project implements an Abacus-based legalization algorithm for standard-cell placement.

Given an initial placement and placement constraints, the goal is to assign cells to legal rows and subrows, remove overlaps, and minimize displacement.

## Build and Execution

Compile the program from the `src/` directory:

```bash
cd src
make
```

This generates the executable in the output directory defined by the Makefile.

An example execution command is:

```bash
./hw4 ../testcases/ibm01.aux ../output/ibm01.pl
```

## Input Files

The implementation reads benchmark inputs in the following formats:

- `.aux` — benchmark configuration
- `.nodes` — cell and terminal definitions
- `.pl` — initial placement
- `.scl` — row and site information

## Implementation Details

The implementation consists of three main stages:

1. Parse the benchmark files and construct cells, terminals, rows, and subrows.
2. Remove blocked regions from rows based on terminal locations.
3. Legalize standard cells using an Abacus-style clustering approach.

For each cell, the algorithm:
- searches for candidate rows,
- selects a feasible subrow,
- evaluates displacement cost,
- places the cell into a cluster,
- merges overlapping clusters when necessary.

After legalization, the final cell positions are snapped to row locations and written to the output placement file.

## Optimization Strategies

Two heuristic strategies were introduced to improve the efficiency of row selection and legalization.

### 1. Bidirectional row search
In the row-search stage, the implementation does not only scan candidate rows in a single direction.  
Instead, it compares rows from both the front and the back of the row list to increase the chance of identifying a better placement row earlier.  
This modification was intended to improve both row selection quality and overall legalization efficiency.

### 2. Cost-guided early stopping
During legalization, I observed that once a cell is assigned to a promising row and subrow, the placement cost often decreases progressively.  
Based on this observation, I used the previous placement cost as a heuristic reference when searching for better row candidates.  
If the current best cost becomes sufficiently smaller than the previous one, the search can stop early instead of continuing to evaluate more rows.

### Trade-off
In practice, these heuristics improve runtime, but they also introduce a trade-off between runtime and placement quality, especially in terms of maximum displacement.  
The bidirectional row-search strategy tends to help both speed and max-displacement quality, while the cost-guided early stopping strategy mainly improves speed but may worsen the maximum-displacement result in some cases.

## Output

The program outputs a legalized placement file containing:
- legalized cell coordinates
- fixed terminal coordinates

## Experimental Results

The report includes displacement and runtime results for the following benchmarks:

- `adaptec1`
- `adaptec3`
- `ibm01`
- `ibm07`
- `ibm09`

## Notes

- The main strength of my implementation is runtime optimization. Compared with the fastest previous top-5 result, my implementation is faster by 60% on ibm01, 41.2% on ibm07, and 40% on ibm09. 
- The trade-off is that the total displacement is not as competitive as the best previous solutions.
