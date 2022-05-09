#!/usr/bin/env bash
if [[ "$#" -ne 0 && $1 == "vtune" ]]
then
    if [[ "$#" -eq 1 && $1 == "vtune" ]]
    then
        mkdir -p build
        cd build;
        cmake -DVTUNE=1 -DCMAKE_BUILD_TYPE=Release ..;
    elif [[ "$#" -eq 2 && $2 == "time" ]]
    then
        mkdir -p build
        cd build;
        cmake -DVTUNE=1 -DTIME=1 -DCMAKE_BUILD_TYPE=Release ..;
    elif [[ "$#" -eq 2 && $2 == "consecutive" ]]
    then
        mkdir -p build
        cd build;
        cmake -DVTUNE=1 -DCONSECUTIVE=1 -DCMAKE_BUILD_TYPE=Release ..; 
    elif [ "$#" -eq 3 ] && [ $2 == "time" ] && [ $3 == "consecutive" ]
    then
        mkdir -p build
        cd build;
        cmake -DVTUNE=1 -DTIME=1 -DCONSECUTIVE=1 -DCMAKE_BUILD_TYPE=Release ..;
    else
        echo "Wrong arguments or the order of arguments are wrong"
    fi
else
    if [[ "$#" -eq 0 ]]
    then
        mkdir -p build
        cd build;
        cmake -DCMAKE_BUILD_TYPE=Release ..;
    elif [[ "$#" -eq 1 && $1 == "time" ]]
    then
        mkdir -p build
        cd build;
        cmake -DTIME=1 -DCMAKE_BUILD_TYPE=Release ..;
    elif [[ "$#" -eq 1 && $1 == "consecutive" ]]
    then
        mkdir -p build;
        cd build;
        cmake -DCONSECUTIVE=1 -DCMAKE_BUILD_TYPE=Release ..;
    elif [ "$#" -eq 2 ] && [ $1 == "time" ] && [ $2 == "consecutive" ]
    then
        mkdir -p build;
        cd build;
        cmake -DTIME=1 -DCONSECUTIVE=1 -DCMAKE_BUILD_TYPE=Release ..;
    else
        echo "Wrong arguments or the order of arguments are wrong"
    fi
fi
make;
cd ..;

