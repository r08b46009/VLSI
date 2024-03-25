#! /bin/bash

# # start from ./src
make
cd ..
cd bin
if [ "$1" = "1" ]; then
    ./hw3 ../testcases/n100.hardblocks ../testcases/n100.nets ../testcases/n100.pl ../output/n100_0.15.floorplan 0.15
    cd ..
    cd verifier
    ./verifier ../testcases/n100.hardblocks ../testcases/n100.nets ../testcases/n100.pl ../output/n100_0.15.floorplan 0.15
elif [ "$1" = "2" ]; then
    ./hw3 ../testcases/n200.hardblocks ../testcases/n200.nets ../testcases/n200.pl ../output/n200_0.15.floorplan 0.15
    cd ..
    cd verifier
    ./verifier ../testcases/n200.hardblocks ../testcases/n200.nets ../testcases/n200.pl ../output/n200_0.15.floorplan 0.15
elif [ "$1" = "3" ]; then
    ./hw3 ../testcases/n300.hardblocks ../testcases/n300.nets ../testcases/n300.pl ../output/n300_0.15.floorplan 0.15
    cd ..
    cd verifier
    ./verifier ../testcases/n300.hardblocks ../testcases/n300.nets ../testcases/n300.pl ../output/n300_0.15.floorplan 0.15
else
    echo "Error"
fi
# make
# cd ..
# cd bin
# if [ "$1" = "1" ]; then
#     ./hw3 ../testcases/n100.hardblocks ../testcases/n100.nets ../testcases/n100.pl ../output/n100_0.15.floorplan 0.15
#     cd ..
#     cd verifier
#     ./verifier ../testcases/n100.hardblocks ../testcases/n100.nets ../testcases/n100.pl ../output/n100_0.15.floorplan 0.15
# elif [ "$1" = "2" ]; then
#     ./hw3 ../testcases/n200.hardblocks ../testcases/n200.nets ../testcases/n200.pl ../output/n200_0.15.floorplan 0.15
#     cd ..
#     cd verifier
#     ./verifier ../testcases/n200.hardblocks ../testcases/n200.nets ../testcases/n200.pl ../output/n200_0.15.floorplan 0.15
# elif [ "$1" = "3" ]; then
#     ./hw3 ../testcases/n300.hardblocks ../testcases/n300.nets ../testcases/n300.pl ../output/n300_0.15.floorplan 0.15
#     cd ..
#     cd verifier
#     ./verifier ../testcases/n300.hardblocks ../testcases/n300.nets ../testcases/n300.pl ../output/n300_0.15.floorplan 0.15
# else
#     echo "Error"
# fi

# start from ./src
# make
# cd ..
# cd bin
# if [ "$1" = "1" ]; then
#     ./hw2 ../testcases/p2-1.cells ../testcases/p2-1.nets ../output/p2-1.out
# elif [ "$1" = "2" ]; then
#     ./hw2 ../testcases/p2-2.cells ../testcases/p2-2.nets ../output/p2-2.out
# elif [ "$1" = "3" ]; then
#     ./hw2 ../testcases/p2-3.cells ../testcases/p2-3.nets ../output/p2-3.out
# elif [ "$1" = "4" ]; then
#     ./hw2 ../testcases/p2-4.cells ../testcases/p2-4.nets ../output/p2-4.out
# elif [ "$1" = "5" ]; then
#     ./hw2 ../testcases/p2-5.cells ../testcases/p2-5.nets ../output/p2-5.out
# else
#     echo "Error"
# fi
