#include <stdio.h>
#include <cassert>
#include <string>
#include <fstream>
#include <iostream>

void readDiffBinary(std::ifstream& file1, std::ifstream& file2){
    bool stop = false;
    int count_read = 0;
    while (1) {
        // get number of bits
        int numBit1 = 0;
        for(int j=0 ; j<4 ; j+=1){
            // todo: uint32_t????? when we write we use int
            int value = (int)(file1.get());
            numBit1 = numBit1 | value << (j*8);
        }
        
        
        int numBit2 = 0;
        for(int j=0 ; j<4 ; j+=1){
            // todo: uint32_t????? when we write we use int
            int value = (int)(file2.get());
            numBit2 = numBit2 | value << (j*8);
        }

        if(numBit1 != numBit2){
            printf("diff at count_read: %d\n", count_read);
            break;
        }
        // printf("numBit: %d\n", numBit1);
        

        if(numBit1 == 0) break;
        int numByte1 = (numBit1 + 7)/8;

        for( int j=0; j<numByte1; j+=1){
            uint8_t read1 = (uint8_t)file1.get();
            uint8_t read2 = (uint8_t)file2.get();
            if(read1 != read2){
                stop  = true;
                printf("diff count read: %d\n", count_read);
                printf("read1: %u, read2: %u\n", read1, read2);
                printf("at byte: %d\n", j);
                break;
            }
        }

        if(stop)
            break;

        count_read++;
    }
    
    return ;
}

int main(int argc, char** argv){
    assert(argc == 3);
    std::string filename1(argv[1]);
    std::string filename2(argv[2]);

    std::ifstream file1(filename1, std::ios::in | std::ios::out | std::ios::binary);
    std::ifstream file2(filename2, std::ios::in | std::ios::out | std::ios::binary);
    readDiffBinary(file1, file2);
    file1.close();
    file2.close();
}