#include "pgm_metric.h"
#include <bits/stdc++.h>
#include <iostream>
#include <fstream>

#define skip_string_input 0
#define skip_byte_input 0

#ifndef debug
#define debug 1
#endif

using namespace std;

void print_pgm(uint64_t * _keys, int count, std::string& output){
    printf("write hardness to file %s\n", output.c_str());
    
    std::ofstream osout(output);
    assert(osout);

    auto one_hardness = pgmMetric::PGM_metric(_keys, count, 1);
    auto local_hardness = pgmMetric::PGM_metric(_keys, count, 32) ;
    auto global_hardness = pgmMetric::PGM_metric(_keys, count, 4096) ;

    std::cout << "One hardness = " << one_hardness << ",\t\t num = " << count << ",\t ratio = " << (1.0 * one_hardness)/ count << std::endl ;
    std::cout << "Local hardness = " << local_hardness << ",\t\t num = " << count << ",\t ratio = " << (1.0 * local_hardness)/ count << std::endl ;
    std::cout << "Global hardness = " << global_hardness << ",\t\t num = " << count << ",\t ratio = " << (1.0 * global_hardness)/ count << std::endl ;

    osout << "One hardness = " << one_hardness << ",\t\t num = " << count << ",\t ratio = " << (1.0 * one_hardness)/ count << std::endl ;
    osout << "Local hardness = " << local_hardness << ",\t\t num = " << count << ",\t ratio = " << (1.0 * local_hardness)/ count << std::endl ;
    osout << "Global hardness = " << global_hardness << ",\t\t num = " << count << ",\t ratio = " << (1.0 * global_hardness)/ count << std::endl ;

    osout.close();

    return ;
}

void readFile(std::string& input, std::vector<uint64_t>& keys){
    std::cout << "reading data from: " << input << std::endl;

    // if(!file_exists(input)) throw std::invalid_argument("read file does not exist");
    std::ifstream infile(input, std::ios::in | std::ios::out | std::ios::binary);

    int count_num_bit = 0;
    int count_read = 0;

    std::vector<uint64_t> tmp_keys;
    while (1) {
        // get number of bits
        int numBit = 0;
        for(int j=0; j< 4 ; j+=1){
            // todo: uint32_t????? when we write we use int
            int value = (int)(infile.get());
            numBit = numBit | value << (j*8);
        }

        // 00. .. 10 // ????????

        // bit 10
        // bit 10000000
        // byte 80
        // write 80
        // read 80 00 00 00 00 ... 00



        // if( count_num_bit ++ <= 20 )
        //     printf("numBit: %d\n", numBit);

        if(numBit == 0) break;
        int numByte = (numBit + 7)/8;

        uint64_t tmp = 0;
        for( int j=0; j<numByte; j+=1){
            if( j < 8 ){
                // todo: static_cast??? are you sure???
                uint64_t read = (uint64_t)infile.get();

                // if(count_read < 20)
                    // printf("%d ", read);

                tmp = tmp | read << (j*8);
            }else
                infile.get();
        }
        // if(count_read < 20)
        //     printf("\n");

        // if(count_read < 20)
        //     std::cout << "tmp      : " << tmp << std::endl;

        if(numByte < 8)
            tmp = tmp << (8 * (8 - numByte)) ;

        // if(count_read < 20)
        //     std::cout << "tmp shift: " << tmp << std::endl;

        count_read++;
        tmp_keys.push_back(tmp);
    }

    printf("read %lu\n", tmp_keys.size());
    
    int cc = 0 ;

    // std::sort(tmp_keys.begin(), tmp_keys.end());
    for( uint64_t val : tmp_keys ){
        if( keys.size() == 0 || val != keys[ keys.size() - 1 ] ){
            keys.push_back(val);
            // if(cc < 20) std::cout << val << std::endl ;
            cc ++ ;
        }
    }
        

    infile.close();
    printf("done reading data\n");
}

int main(int argc, char** argv) {
    assert( argc == 3 );
    std::string input( argv[1] );
    std::string out_dir( argv[2] );
    std::vector<uint64_t> keys;

    readFile( input , keys );

    std::string output_filename = out_dir + "/" + input.substr(input.find_last_of("/\\") + 1);
    printf("output file: %s\n", output_filename.c_str());
    print_pgm( keys.data() , keys.size() , output_filename);

    return 0;
}