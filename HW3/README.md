# HW3: Fixed-outline Floorplan Design

## Overview
This project implements a simulated annealing (SA)-based algorithm for fixed-outline floorplan design using hard blocks.

The objective is to optimize floorplan quality by minimizing wirelength while satisfying fixed-outline and dead-space constraints.

## Build and Execution

Compile the program from the `src/` directory:

```bash
cd src
make
```

This generates the executable `hw3` in the `bin/` directory.

An example execution command from the `bin/` directory is:

```bash
./hw3 ../testcases/n100.hardblocks ../testcases/n100.nets ../testcases/n100.pl ../output/n100_0.1.floorplan 0.1
```

The executable can be used to run different benchmark cases such as `n100`, `n200`, and `n300`.

## Implementation Details

The implementation uses a postfix representation of slicing floorplans together with simulated annealing.

The main flow is as follows:

1. Parse the hard block, pin, and netlist files and construct the input data structure.
2. Generate an initial postfix representation and estimate the outline from the total block area and dead-space ratio.
3. Apply simulated annealing with perturbation operations to search for better floorplans.
4. Rebuild the slicing tree from the postfix expression, evaluate feasible width-height combinations, and assign block positions.
5. Compute the final wirelength using HPWL (Half-Perimeter Wirelength).

The optimization is performed in two stages. The first stage focuses on finding a feasible solution under the outline constraint, and the second stage refines the feasible solution to further reduce wirelength.

## Perturbation Operations

The implementation uses three perturbation operations:

- **M1**: swap two block operands in the postfix representation
- **M2**: change the cut types in a consecutive operator segment
- **M3**: swap an adjacent operand and operator when the postfix expression remains valid

To improve exploration of the solution space, the M1 operation is not restricted to adjacent operands.

## Optimization Strategies

I also tested different random seeds and compiler optimization settings. In practice, changing the random seed did not produce a large difference in the final results, while compiler optimization provided some runtime improvement.

## Output

The program generates a floorplan output file containing the final wirelength and block placements.

Example output format:

```text
Wirelength 75563
Blocks
sb0 152 284 1
sb1 126 179 0
...
```

## Running with Script

A helper script is provided in the `src/` directory:

```bash
cd src
bash run.sh <testcase_number>
```

Available options:
- `1` — run `n100`
- `2` — run `n200`
- `3` — run `n300`

The script compiles the program, runs the selected testcase, and then invokes the verifier.

## Notes

- The smallest dead-space ratio for which this implementation can produce a legal result within the required runtime is `0.1`.
- Example floorplan screenshots are provided in the `Floorplan print` folder.
