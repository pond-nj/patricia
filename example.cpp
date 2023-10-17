#include "include/sk/patricia.hxx"
#include <iostream>
#include <string>
#include <set>
#include <fstream>
#include <iomanip>

// File is written by key size followed by key values (written in uint64_t at one time)
void readBitsFormat(std::string input){
    std::cout << "reading data from: " << input << std::endl;
    std::ifstream infile(input, std::ios::in | std::ios::out | std::ios::binary);

    int count_read = 0;

    while (1) {
        // read number of bits for this key
        int numBit = 0;
        for(int j=0; j< 4 ; j+=1){
            int value = (int)(infile.get());
            numBit = numBit | value << (j*8);
        }

        if(numBit == 0) break;
        // because the file is written by uint64_t at once
        int numUint64ToRead = (numBit + 63) / 64;
        int numBytes = numUint64ToRead * 8;

        uint64_t* read = new uint64_t[numUint64ToRead];
        for( int j=0; j<numBytes; j+=1){
            uint8_t c = infile.get();
            // std::cout << std::hex << (uint64_t)read[0] << std::endl;
            read[ j/8 ] |= ((uint64_t)c) << (j*8);
        }

        int offset = numBit % 64;
        if( offset != 0 )
            read[numUint64ToRead - 1] = numUint64ToRead >> (64 - offset);

        std::string s((char *) read, numBytes);

        count_read++;
    }

    printf("count read: %d\n", count_read);
    
    infile.close();
    return ;
}

int main(){
    readBitsFormat( "dense_keys/git_repo_name_clean.txt_all" );
}