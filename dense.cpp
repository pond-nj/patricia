#include "include/sk/patricia.hxx"
#include <iostream>
#include <string>
#include <set>
#include <fstream>
#include <stack>
#include <cstdint>

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

        if(count % 500000 == 0) printf("%d\n", count);
    }

    printf("read count: %d\n", count);

    infile.close();
    printf("done read data from: %s\n", input.c_str());
}

// get all bit position including more than 64 bits
void get_dense_key_bit_pos(sk::patricia_trie<int>& trie, std::vector<size_t>& bit_list){

    bool done = false ;
    auto current = trie.root_node() ;
    if(current->rightedge() && current->leftedge())
        bit_list.push_back(current->bit) ;

    while(!done){
        do {
            assert(current != nullptr);
            if (current->leftedge()) {
                current = current->leftedge();
                // std::cout << "left\n" ;
                if(!current->value) bit_list.push_back(current->bit) ;
                continue;
            }

            if (current->rightedge()) {
                current = current->rightedge();
                // std::cout << "right\n" ;
                if(!current->value) bit_list.push_back(current->bit) ;
                continue;
            }

            // If we can't go either left or right, back up until we
            // find a node where we went left last time and can go
            // right this time.  If we walk up to the root and don't
            // find any right branch, stop iterating.
            for (;;) {
                SK_PATRICIA_TRACE_MSG("iterator: walking up, current={}\n",
                                        (void *)current);
                assert(current != nullptr);
                assert(current->parent != nullptr);
                auto here_is_left =
                    (current == current->parent->leftedge());
                auto parent_has_right =
                    (current->parent->rightedge() != nullptr);

                if (here_is_left && parent_has_right) {
                    current = current->parent->rightedge();
                    // std::cout << "left to right\n" ;
                    if(!current->value) bit_list.push_back(current->bit) ;
                    break;
                }

                current = current->parent;
                // std::cout << "up\n" ;
                if (!current->parent) {
                    SK_PATRICIA_TRACE_MSG(
                        "iterator: at root and nowhere to go\n");
                    current = nullptr;
                    done = true ;
                    break;
                }
            }
        } while (current && !current->value);
    }

    assert(bit_list.size() > 0) ;
}

void remove_dup(std::vector<size_t>& vec){
    sort(vec.begin(), vec.end()) ;

    int count_i = 1 ;
    for(int i = 1 ; i < vec.size() ; i ++){
        if (vec[count_i - 1] < vec[i])
            vec[count_i ++] = vec[i] ;
    }

    int size = vec.size() ;
    for(; count_i < size ; count_i ++)
        vec.pop_back() ;
}

// returned keys size should be indicated by write_func
void get_keys_and_write_to_file(sk::patricia_trie<int>& trie, std::vector<size_t>& bit_list, std::function<void(std::vector<uint64_t>& bit_set, int count_bit)> write_func){
    // get bits from the position we want
    std::vector<uint64_t> bit_pos;
    int count_bit = 0;
    auto insert_bit_pos = [&](bool bit){
        if (count_bit == 0)
            bit_pos.push_back(bit);
        else{
            int last = bit_pos.size() - 1;
            bit_pos[last] = bit_pos[last] << 1;
            bit_pos[last] |= bit;
        }
        count_bit++;
        count_bit = count_bit % 64;
    };

    // encode key
    int return_size;
    if (bit_list.size() < 64)
        return_size = bit_list.size();
    else
        return_size = 64;

    for(auto it = trie.begin() ; it != trie.end(); it ++){
        bit_pos.clear() ;
        count_bit = 0 ;

        // if( it == trie.begin() )
        //     printf("bit: ");
        for(int i = 0 ; i < return_size ; i+= 1 ){
            int pos = bit_list[i];
            bool bit = ((it.get_node())->key).test_bit(pos) ;
            // if( it == trie.begin() )
            //     printf("%d ", bit );
            insert_bit_pos(bit);
        }
        // if( it == trie.begin() )
        //     printf("\n");

        // printf("bit size %ld\n", ((it.get_node())->key).size_bits()) ;

        // write to file
        write_func(bit_pos, count_bit);
    }
}

void test(){
    sk::patricia_trie<int> trie;

    int count = 0;
    auto insert = [&](sk::patricia_key& pk){
        trie.insert(pk, count++);
    };

    std::vector<std::byte> byte_vec1 = {std::byte{0b01},std::byte{0b10100101}};
    sk::patricia_key key1(byte_vec1);
    insert(key1);

    std::vector<std::byte> byte_vec2 = {std::byte{0b01}, std::byte{0b10100110}};
    sk::patricia_key key2(byte_vec2);
    insert(key2);

    std::vector<std::byte> byte_vec3 = {std::byte{0b01}, std::byte{0b10101010}};
    sk::patricia_key key3(byte_vec3);
    insert(key3);

    std::vector<std::byte> byte_vec4 = {std::byte{0b01}, std::byte{0b10101011}};
    sk::patricia_key key4(byte_vec4);
    insert(key4);

    std::vector<std::byte> byte_vec5 = {std::byte{0b01}, std::byte{0b11010110}};
    sk::patricia_key key5(byte_vec5);
    insert(key5);

    std::vector<std::byte> byte_vec6 = {std::byte{0b01}, std::byte{0b11101001}};
    sk::patricia_key key6(byte_vec6);
    insert(key6);

    std::vector<std::byte> byte_vec7 = {std::byte{0b01}, std::byte{0b11101011}};
    sk::patricia_key key7(byte_vec7);
    insert(key7);

    // std::vector<std::byte> byte_vec8 = {std::byte{0b10000011}, std::byte{0b11101011}};
    // sk::patricia_key key8(byte_vec8);
    // insert(key8);

    std::vector<size_t> bit_list ;
    get_dense_key_bit_pos(trie, bit_list); 

    remove_dup(bit_list);

    // for(int i = 0 ; i < bit_list.size() ; i ++){
    //     std::cout << bit_list[i] << " " ;
    // }

    std::cout << std::endl ;

    std::vector<int> expect_bit_list = {3, 4, 6, 8, 9};
    assert(bit_list.size() == expect_bit_list.size());
    for(int i =0; i<expect_bit_list.size(); i++)
        assert(bit_list[i] == 6 + expect_bit_list[i]);

    std::set<std::byte> assert_outputs = {
        std::byte{0b01001},
        std::byte{0b01010},
        std::byte{0b01110},
        std::byte{0b01111},
        std::byte{0b10010},
        std::byte{0b11101},
        std::byte{0b11111},
    };
    auto test_write_func = [&](std::vector<uint64_t>& bit_set, int count_bit ){
        // fprintf(stderr, "called\n");
        assert(bit_set.size() == 1);

        // fprintf(stderr, "count_bit: %d\n", count_bit);
        assert(count_bit == 5);
        uint8_t first_byte = bit_set[0] & 255;
        assert(assert_outputs.contains( std::byte{first_byte} ));
    };
    get_keys_and_write_to_file(trie, bit_list, test_write_func);
}

void do_dense(int argc, char** argv){
    // input data to patricia
    assert( argc == 4 );
    std::string int_output_dir( argv[3] );
    std::string output_dir( argv[2] );
    std::string input_file( argv[1] );
    std::string filename = input_file.substr(input_file.find_last_of("/\\") + 1);

    sk::patricia_trie<int> trie;

    int count = 0;

    auto insert_key = [&](std::string& str){
        char const *c = str.c_str();
        sk::patricia_key pk(c);
        trie.insert(pk, count);
        count += 1;
    };

    // read and insert to patricia
    readStringFile( input_file , insert_key );

    std::vector<size_t> bit_list ;
    get_dense_key_bit_pos(trie, bit_list);    

    // TODO(Pond): what is this??
    // for(auto it = trie.begin() ; it != trie.end(); it++ ){
    //     std::size_t bit = (it.get_node())->bit;
    //     bit_list.push_back(bit) ;
    // }

    // sort and delete duplicated bit position
    remove_dup(bit_list);

    std::ofstream out_file(  output_dir + "/" + filename, std::ios::out | std::ios::binary );
    std::ofstream int_file( int_output_dir + "/" + filename, std::ios::out | std::ios::binary );

    int count_write = 0;
    auto writeToFile = [&](std::vector<uint64_t>& bit_set, int count_bit ){
        // write keysize
        if(count_bit == 0) count_bit = 64 ;
        int sixty_four = 64;
        out_file.write( static_cast<const char *>( static_cast<const void*> (&sixty_four)), sizeof(int) );

        // write key val
        uint64_t first = bit_set.size() == 1 ? bit_set[0] << (64 - count_bit) : bit_set[0] ;
        out_file.write( static_cast<const char *>( static_cast<const void*> (&first) ), sizeof(uint64_t) );

        // write to int format
        int_file << first << std::endl;

        count_write++;
    };
    get_keys_and_write_to_file(trie, bit_list, writeToFile);
    printf("count_write: %d\n", count_write);


    int zero = 0;
    out_file.write(static_cast<const char *>(static_cast<const void*> (&zero)), 4);
}

int main(int argc, char** argv){
    test();
    do_dense(argc, argv);
}