#include "include/sk/patricia.hxx"
#include <iostream>
#include <string>
#include <set>
#include <fstream>
#include <stack>

// TODO:
// save whole partial key to a file format

// dense
// count duplicate
// plot cdf
// hardness graph
// check correctness of sparse key

void readStringFile(std::string input, std::function<void(std::string&)> insert){
    std::cout << "reading data from: " << input << std::endl;

    // if(!file_exists(input)) throw std::invalid_argument("read file does not exist");

    std::ifstream infile(input, std::ios::in | std::ios::out | std::ios::binary);
    assert( infile );
    int count = 0;
    
    std::string key;
    while(infile >> key){
        count++;
        insert(key);
    }

    printf("read count: %d\n", count);

    infile.close();
    printf("done read data from: %s\n", input.c_str());
}

int count_print_keysize = 0;

void writeToFile( std::ostream& out_file, std::vector<uint64_t>& bit_set, int count_bit ){
    // write keysize
    if(count_bit == 0) count_bit = 64 ;
    int sixty_four = 64;
    out_file.write( static_cast<const char *>( static_cast<const void*> (&sixty_four)), sizeof(int) );

    // write key val
    uint64_t first = bit_set.size() == 1 ? bit_set[0] << (64 - count_bit) : bit_set[0] ;
    out_file.write( static_cast<const char *>( static_cast<const void*> (&first) ), sizeof(uint64_t) );
}

void readFile(std::string& input, std::vector<uint64_t>& keys){
    std::cout << "reading data from: " << input << std::endl;

    // if(!file_exists(input)) throw std::invalid_argument("read file does not exist");
    std::ifstream infile(input, std::ios::in | std::ios::out | std::ios::binary);

    std::vector<uint64_t> tmp_keys;
    while (1) {
        // get number of bits
        uint32_t numBit = 0;
        for(int j=0; j< 4 ; j+=1){
            // todo: uint32_t????? when we write we use int
            uint32_t value = (uint32_t)(infile.get());
            numBit = numBit | value << (j*8);
        }

        if(numBit == 0) break;

        int numByte = (numBit + 7)/8;

        uint64_t tmp = 0;
        for( int j=0; j<numByte; j+=1){
            if( j < 8 ){
                tmp = tmp << 8;
                // todo: static_cast??? are you sure???
                tmp |= static_cast<uint64_t>(infile.get());
            }else
                infile.get();
        }
        if(numByte < 8) tmp = tmp << (8 * (8 - numByte)) ;

        tmp_keys.push_back(tmp);
        
    }
    
    int cc = 0 ;

    // std::sort(tmp_keys.begin(), tmp_keys.end());
    for( uint64_t val : tmp_keys ){
        if( keys.size() == 0 || val != keys[ keys.size() - 1 ] ){
            keys.push_back(val);
            if(cc < 20) std::cout << val << std::endl ;
            cc ++ ;
        }
    }
        
    infile.close();
    printf("done reading data\n");
}

void doSparse(){
    
}

int main(int argc, char** argv){
    // input data to patricia
    assert( argc == 4 );
    std::string input_file( argv[1] );
    std::string output_dir( argv[2] );
    std::string int_output_dir( argv[3] );
    std::string filename = input_file.substr(input_file.find_last_of("/\\") + 1);

    sk::patricia_trie<int> trie;
    int count = 0;

    auto insert_key = [&](std::string& str){
        char const *c = str.c_str();
        sk::patricia_key pk(c);
        trie.insert(pk, count);
        count += 1;
    };

    readStringFile( input_file , insert_key );

    std::vector<uint64_t> bit_set;
    int count_bit = 0;
    auto insert_bit_set = [&](bool bit){
        if (count_bit == 0)
            bit_set.push_back(bit);
        else{
            int last = bit_set.size() - 1;
            bit_set[last] = bit_set[last] << 1;
            bit_set[last] |= bit;
        }
        count_bit++;
        count_bit = count_bit % 64;
    };

    auto remove_bit_set = [&](){
        if(count_bit == 1){
            assert( bit_set.size() != 0 );
            bit_set.pop_back();
        }else{
            int last = bit_set.size() - 1;
            bit_set[last] = bit_set[last] >> 1;
        }
        count_bit += 63;
        count_bit = count_bit % 64;
    };

    auto current = trie.root_node();
    bool done = false;

    uint64_t last = 0 ;
    int count_write = 0;
    int print_last = 0;

    std::ofstream out_file( output_dir + "/" + filename, std::ios::out | std::ios::binary );

    // also write in int format for cdf
    std::ofstream int_file( int_output_dir + "/" + filename, std::ios::out | std::ios::binary );
    auto writeFirstSixtyFourBitsToUIntFormat = [&]( std::vector<uint64_t>& bit_set ){
        uint64_t first = bit_set.size() == 1 ? bit_set[0] << (64 - count_bit) : bit_set[0] ;
        int_file << first << std::endl;
    };

    while(!done){
        do {
            SK_PATRICIA_TRACE_MSG("iterator: at {}\n", (void *)current);
            if (current->leftedge()) {
                insert_bit_set(0);
                current = current->leftedge();
                continue;
            }

            if (current->rightedge()) {
                insert_bit_set(1);
                current = current->rightedge();
                continue;
            }

            // If we can't go either left or right, back up until we
            // find a node where we went left last time and can go
            // right this time.  If we walk up to the root and don't
            // find any right branch, stop iterating.
            for (;;) {
                SK_PATRICIA_TRACE_MSG("iterator: walking up, current={}\n",
                                        (void *)current);
                auto here_is_left =
                    (current == current->parent->leftedge());
                auto parent_has_right =
                    (current->parent->rightedge() != nullptr);

                if (here_is_left && parent_has_right) {
                    remove_bit_set();
                    insert_bit_set(1);
                    current = current->parent->rightedge();
                    break;
                }

                remove_bit_set();
                current = current->parent;
                
                if (!current->parent) {
                    SK_PATRICIA_TRACE_MSG(
                        "iterator: at root and nowhere to go\n");
                    current = nullptr;
                    done = true;
                    break;
                }
            }
        } while (current && !current->value);
        if(!done){

            // check irder-perserving -> true
            uint64_t first = bit_set.size() == 1 ? bit_set[0] << (64 - count_bit) : bit_set[0] ;
            assert(last <= first); //"not order-preserving"
            last = first ;
            
            // if(count_write < 20){
            //     std::cout << "bit_set[0]: " << bit_set[0] << std::endl;
            //     std::cout << "last: " << last << std::endl;
            // }

            count_write++;
            writeToFile(out_file, bit_set, count_bit);
            writeFirstSixtyFourBitsToUIntFormat(bit_set);
        }
    }

    printf("count_write: %d\n", count_write);

    // get patial key from raw key

    int zero = 0;
    out_file.write(static_cast<const char *>(static_cast<const void*> (&zero)), 4);
}