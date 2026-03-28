#!/bin/bash

# Run from the src/ directory.
set -e

make
cd ../bin

if [ "$1" = "1" ]; then
    ./hw3 ../testcases/n100.hardblocks ../testcases/n100.nets ../testcases/n100.pl ../output/n100_0.15.floorplan 0.15
    cd ../verifier
    ./verifier ../testcases/n100.hardblocks ../testcases/n100.nets ../testcases/n100.pl ../output/n100_0.15.floorplan 0.15
elif [ "$1" = "2" ]; then
    ./hw3 ../testcases/n200.hardblocks ../testcases/n200.nets ../testcases/n200.pl ../output/n200_0.15.floorplan 0.15
    cd ../verifier
    ./verifier ../testcases/n200.hardblocks ../testcases/n200.nets ../testcases/n200.pl ../output/n200_0.15.floorplan 0.15
elif [ "$1" = "3" ]; then
    ./hw3 ../testcases/n300.hardblocks ../testcases/n300.nets ../testcases/n300.pl ../output/n300_0.15.floorplan 0.15
    cd ../verifier
    ./verifier ../testcases/n300.hardblocks ../testcases/n300.nets ../testcases/n300.pl ../output/n300_0.15.floorplan 0.15
else
    echo "Usage: bash run.sh <testcase_number>"
    echo "  1 -> n100"
    echo "  2 -> n200"
    echo "  3 -> n300"
    exit 1
fi
