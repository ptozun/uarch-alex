# Micro-architectural Analysis of ALEX, ART, and Btree

## How to run the benchmark driver
Download the project:

    git clone https://github.com/ptozun/uarch-alex.git
    cd uarch-alex/

For keeping this repository brief, we exclude the source-codes for the three indexes. You can download the index codebases using the links mentioned in the paper; Micro-architectural Analysis of a Learned Index, aiDM 2022. The codebases should be placed under the corresponding subfolder for each index.\
There is a benchmark driver located in each folder. \
To compile the benchmark driver there is a build.sh in the folder. \
It will create a folder called 'build' which contains an executable file. \
For ALEX that file is named: runALEX \
For ART that file is named: runART \
For B+Tree that file is named: runbt \
Below is an example of how to run the benchmark driver using ALEX as an example:

    ./build.sh
    ./build/runALEX init_keys number_of_request reads updates inserts deletions
  
The argument *init_key* is the amount of initial keys. \
The argument *number_of_request* is the amount of requests. \
The arguments *reads*, *updates*, *inserts* and *deletions* are the distribution of requests between read, update, insert and deletion. They should be given as a number between 0 and 100 indicating percentage of requests.\
Example:

    ./build/runALEX 2000 100 25 25 40 10
    
The above will create the initial index with 2000 keys.\
Perform a total of 100 requests, where ~25% are reads,\
~25% are updates, ~40% are inserts and ~10% deletions. 
### Important: Remove build folder after each run
After each run it is recommended to remove the build directory. It garantees that the benchmark driver is compiled from scratch. \
This is especially important when changing the options for the benchmark driver. Failing to do so, will make the benchmark driver run with the old options and not the new. \
Re-compiling instead of removing does NOT work. The build folder has to be deleted for the options to run as intended.
### Options for the benchmark driver
The benchmark driver is located in the folder of the code base. \
The name of the ALEX benchmark driver is: Alex.cpp \
The name of the B+Tree benchmark driver is: btree.cpp \
The name of the ART benchmark driver is: art.cpp \

Inside the benchmark driver there are 6 changeable values. These can be used to adapt the benchmark driver.

| Values             | Description                                                 |
|--------------------|-------------------------------------------------------------|
| KEY_TYPE           | Macro which is used determine the data type of the keys     |
| PAYLOAD_TYPE       | Macro which is used determine the data type of the payloads |
| key_lowerBound     | Used to set the lower bound for random key generation       |
| key_upperBound     | Used to set the upper bound for random key generation       |
| payload_lowerBound | Used to set the lower bound for random payload generation   |
| payload_upperBound | Used to set the upper bound for random payload generation   |

### Compile options
The benchmark driver comes with 3 compile options:

| Options     | Description                                                        |
|-------------|--------------------------------------------------------------------|
| vtune       | Used to enable code that interacts with VTune                      |
| time        | Enables stopwatches to time individual request                     |
| consecutive | This will trigger the index to be build with consecutive integers. |

They can be combined, but it is important to keep the correct order if multiple options are used. \
The order is that 'vtune' always goes before 'time' and 'time' always goes before 'consecutive'. \
Below is some examples: 

    ./build.sh
    ./build.sh vtune
    ./build.sh consecutive
    ./build.sh time consecutive
    ./build.sh vtune consecutive
    ./build.sh vtune time consecutive

If the benchmark driver is compiled with ’time’ as an argument, the benchmark driver will time the individual requests and output the total time spent on read, update, write  and deletions respectively. \
If one wishes to run the experiment where the index will be filled with consecutive values instead, one has to compile the benchmark driver with the argument ’consecutive’. With this option key_lowerBound and key_upperBound will still dictate in what range the random key for requests will be generated. However the key used for an insert request will not be the randomly generated key, but instead the maximum key value plus 1 in order to continue having an index with consecutive values. All random generation is performed with a uniform distribution
