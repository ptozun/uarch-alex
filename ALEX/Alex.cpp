#include "./src/core/alex.h"
#include <stdio.h>
#include <stdlib.h> 
#include <locale.h>
#include <thread>
#include <list>
#ifdef VTUNE
    #include <ittnotify.h>
#endif
#define KEY_TYPE unsigned long long
#define PAYLOAD_TYPE unsigned long long

int main (int argc, char* argv[]){
    if (argc != 7) {
        printf("Please ensure you put in the arguments correctly:\n");
        printf("See README for instructions");

        return 0;
    }
    //Counters
    int faultyLookup = 0;
    int faultyUpdate = 0;
    int faultyDelete = 0;

    int numberOfReads = 0;
    int numberOfUpdates = 0;
    int numberOfInserts = 0;
    int numberOfDeletions = 0;

    //Timers
    #ifdef TIME
        double read_time = 0.0;
       	double update_time = 0.0;
        double insert_time = 0.0;
        double delete_time = 0.0;
    #endif

    //Total number of keys
    const int num_keys = atoi(argv[1]);

    // The number of times that each action shall be executed.
    // Calculated as a percentage of total keys.
    const int num_requests = atoi(argv[2]);
    const int percent_read = atoi(argv[3]);
    const int percent_update = atoi(argv[4]);
    const int percent_insert = atoi(argv[5]);
    const int percent_delete = atoi(argv[6]);

    // CHANGEABLE values.
    // Determines the ranges of the random generation
    KEY_TYPE key_lowerBound = 0;
    KEY_TYPE key_upperBound = (static_cast<KEY_TYPE>(2)) * (static_cast<KEY_TYPE>(num_keys));

    PAYLOAD_TYPE payload_lowerBound = 9223372036854775808u;
    PAYLOAD_TYPE payload_upperBound = 18446744073709551615u;

    // Creating an array of pairs.
    auto values = new std::pair<KEY_TYPE, PAYLOAD_TYPE>[num_keys];

    // Random 64 bit integer generator
    std::mt19937_64 gen(std::random_device{}());
    
    // Generates random paylods
    std::uniform_int_distribution<PAYLOAD_TYPE> payload_dis(payload_lowerBound, payload_upperBound);    
    // Generates random keys.
    std::uniform_int_distribution<KEY_TYPE> key_dis(key_lowerBound,key_upperBound-1);


    //Stopwatch
    #ifdef TIME
        auto load_start = std::chrono::high_resolution_clock::now();
    #endif
    
    // Creating the ALEX index.
    alex::Alex<KEY_TYPE, PAYLOAD_TYPE> index;

    // Generating num_keys amount of pairs and insert them into values
    #ifndef CONSECUTIVE
        for (int i = 0; i < num_keys; i++) {
	        values[i].first = key_dis(gen);
            values[i].second = payload_dis(gen);
        }
    #endif
    #ifdef CONSECUTIVE
        for (int i = 0; i < num_keys; i++) {
	        values[i].first = static_cast<KEY_TYPE>(i);
            values[i].second = payload_dis(gen);
        }
    #endif
    
    // Sorts the values array. Values has to be sorted before bulk_load
    std::sort(values, values + num_keys, [](auto const& a, auto const& b) { return a.first < b.first; });
    // Bulk load values array
    index.bulk_load(values, num_keys);
    //Stopwatch
    #ifdef TIME
        auto load_stop = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> time = load_stop - load_start;
        printf("Time for generating keys, sort and bulk loading: %f seconds\n" , time.count());
    #endif

    //Delete array
    delete[] values;

    //Used for random request generation.
    const int request_percentage = percent_read + percent_update + percent_insert + percent_delete;
    std::uniform_int_distribution<int> requestDis(0,(request_percentage-1));
    
    
    //Warmup
    for (int i = 0 ; i < 100000 ; i++){
        KEY_TYPE randKey = key_dis(gen);
        index.get_payload(randKey);
    }
    #ifdef VTUNE
        __itt_resume();
    #endif
    //Calculate ints beforehand
    int updateRange = percent_read + percent_update;
    int insertRange = updateRange + percent_insert;
    for (int i = 0; i < num_requests ; i++){
        int r = requestDis(gen);
        if (r < percent_read){
            //Stopwatch
            #ifdef TIME
                auto start = std::chrono::high_resolution_clock::now();
            #endif

            //Perform read
            KEY_TYPE randKey = key_dis(gen);
            PAYLOAD_TYPE* payload = index.get_payload(randKey);
            if (payload == NULL){
                faultyLookup++;
            }
            //Stopwatch
            #ifdef TIME
                auto stop = std::chrono::high_resolution_clock::now();
                std::chrono::duration<double> time = stop - start;
		        read_time += time.count();
            #endif
            numberOfReads++;
        }
        else if (r < updateRange){
            //Stopwatch
            #ifdef TIME
                auto start = std::chrono::high_resolution_clock::now();
            #endif
            
            //Perform update
            KEY_TYPE randKey = key_dis(gen);
            PAYLOAD_TYPE* old_payload = index.get_payload(randKey);

            if (old_payload != NULL){        
                *old_payload = payload_dis(gen);
            }
            else{
                faultyUpdate++;
            }
            //Stopwatch
            #ifdef TIME
                auto stop = std::chrono::high_resolution_clock::now();
                std::chrono::duration<double> time = stop - start;
		        update_time += time.count();
            #endif

            numberOfUpdates++;
        }
        else if (r < insertRange){
            //Stopwatch
            #ifdef TIME
                auto start = std::chrono::high_resolution_clock::now();
            #endif

            // Perform insert
            #ifndef CONSECUTIVE
            KEY_TYPE new_key = key_dis(gen);
            #endif
            #ifdef CONSECUTIVE
            KEY_TYPE new_key = (KEY_TYPE)num_keys + (KEY_TYPE)numberOfInserts;
            #endif
            
            PAYLOAD_TYPE new_payload = payload_dis(gen);
            index.insert(new_key, new_payload);

            //Stopwatch
            #ifdef TIME
                auto stop = std::chrono::high_resolution_clock::now();
                std::chrono::duration<double> time = stop - start;
		        insert_time += time.count();
            #endif

            numberOfInserts++;
        }
        else{
            //Stopwatch
            #ifdef TIME
                auto start = std::chrono::high_resolution_clock::now();
            #endif

            //Perform deletion
            KEY_TYPE randKey = key_dis(gen);
            PAYLOAD_TYPE* payload = index.get_payload(randKey);
            if (payload != NULL){        
                index.erase_one(randKey);
            }
            else{
                faultyDelete++;
            }
            //Stopwatch
            #ifdef TIME
                auto stop = std::chrono::high_resolution_clock::now();
                std::chrono::duration<double> time = stop - start;
		        delete_time += time.count();
            #endif

            numberOfDeletions++;
        }
    }
    #ifdef VTUNE
        __itt_pause();
    #endif

    setlocale(LC_ALL, "");
    //Stopwatch
    #ifdef TIME
        double micro = 1000000.0;
        printf("Total time of reads: %f seconds\n   With an average time of %f microseconds per read\n\n" , (read_time), (read_time*micro/numberOfReads));
        printf("Total time of updates: %f seconds\n   With an average time of %f microseconds per update\n\n" , (update_time), (update_time*micro/numberOfUpdates));
        printf("Total time of inserts: %f seconds\n   With an average time of %f microseconds per insert\n\n" , (insert_time), (insert_time*micro/numberOfInserts));
        printf("Total time of deletions: %f seconds\n   With an average time of %f nanoseconds per deletion\n\n" , (delete_time), (delete_time*micro/numberOfDeletions));
    #endif

    printf("\n\nInitial amount of keys: %'d\n\n", num_keys);
    printf("Total number of reads: %'d\n     Succesful: %'d\n     Unsuccesful: %'d\n\n", numberOfReads, (numberOfReads-faultyLookup), faultyLookup);
    printf("Total number of updates: %'d\n     Succesful: %'d\n     Unsuccesful: %'d\n\n", numberOfUpdates, (numberOfUpdates-faultyUpdate), faultyUpdate);
    printf("Total number of deletions: %'d\n     Succesful: %'d\n     Unsuccesful: %'d\n\n", numberOfDeletions, (numberOfDeletions-faultyDelete), faultyDelete);
    printf("Total number of inserts: %'d\n", numberOfInserts);
    printf("\nCurrent amount of keys: %'d\n", (num_keys+numberOfInserts-(numberOfDeletions-faultyDelete)));

    // Size in bytes of all the model nodes (including pointers) and metadata in data nodes
    printf("\nSize in bytes of all the model nodes (including pointers) and metadata in data nodes: %'lld\n", index.model_size());
    // Size in bytes of all the keys, payloads, and bitmaps stored in this index
    printf("Size in bytes of all the keys, payloads, and bitmaps: %'lld\n\n\n", index.data_size());


    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    return 0;
}
