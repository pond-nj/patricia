#include "include/sk/patricia.hxx"
#include <iostream>
#include <string>
#include <set>
#include <fstream>

void readByteFormat(std::string input, std::function<void(std::string)> insert){
    std::cout << "reading data from: " << input << std::endl;
    std::ifstream infile(input, std::ios::in | std::ios::out | std::ios::binary);

    std::set<std::string> tmp_keys;
    while (1) {
        // get number of bytes for this string
        uint32_t numChar = 0;
        for(int j=0; j< 4 ; j+=1){
            uint32_t value = (uint32_t)(infile.get());
            numChar = numChar | value << (j*8);
        }

        if(numChar == 0) break;

        char * buffer = (char *) malloc(sizeof(char) * numChar) ;
        for( int j=0; j<numChar; j+=1){
            buffer[j] = static_cast<char>(infile.get());
        }

        std::string str(buffer, numChar);

        insert(str);
    }
    
    infile.close();
    std::cout << "done reading data from byte file" << std::endl;
    return ;
}

int main(){
    sk::patricia_trie<int> trie;

    int count = 0;

    auto insert_key = [&](std::string str){
        char const *c = str.c_str();
        sk::patricia_key pk(c);
        trie.insert(pk, count);
        count += 1;
    };

    readByteFormat( "data/git_repo_name_clean" , insert_key);

        

}