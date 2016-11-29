#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <iomanip>
#include <sstream>
#include <ctime>
#include "hash.h"
#include <stdint.h>

template<class T>
T rrot(T x, unsigned int moves)
{
    return (x >> moves) | (x << sizeof(T)*8 - moves);
}

//call this once just to seed rand at the start
void seed_rand(){
    std::srand(std::time(0));
}

//removes the hash from the vector and returns it as a string
std::string remove_hash(std::vector<char>* s){
    std::stringstream ss;
    for(char c : *s) ss << c;
    //int i = ss.str().find_first_of(':');
    //int k = ss.str().find(':');
    //if(isSalted) *salt = ss.str().substr(i+1, 6);
    s->erase(s->end()-32, s->end());
    return ss.str().substr(s->size());
}

//used for just the user's password, so use string not vector. returns as "[SALT]:[HASH]"
static const char salt_chars[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890";
std::string salt_and_hash(std::string s){
    std::vector<char> in;
    for(char c : s) in.push_back(c);
    for(int i = 0; i < 6; ++i)
        in.push_back(salt_chars[std::rand()%(sizeof(salt_chars)-1)]);
    in = *hash_and_append(&in);
    std::stringstream ss;
    for(char c : in) ss << c;
    return ss.str().substr(s.size());
}

//returns whether or not the passwords are the same. the first argument is what should be stored in the
//user file, it is the result from salt_and_hash. the second argument is the user's input, just a password
//that is not hashed
bool compare_salted(std::string salted_pass, std::string pass_to_check){
    std::string salt = salted_pass.substr(0, 6);
    std::string h = salted_pass.substr(6);
    std::cout << salt <<std::endl;
    std::cout << h <<std::endl;
    std::cout << pass_to_check << std::endl;
    std::vector<char> in;
    for(char c : pass_to_check) in.push_back(c);
    for(char c : salt) in.push_back(c);
    for(char c : in) std::cout << c;
    std::cout << std::endl;
    return hashVector(&in) == h;

}

//appends the hash to the vector
std::vector<char>* hash_and_append(std::vector<char>* s){
    std::string h = hashVector(s);
    for(char c : h){
        s->push_back(c);
    }
    return s;
}

//generates the hash
std::string hashVector(std::vector<char>* s){
    std::vector<char> in = *s;
    uint32_t h1 = 0;
    uint32_t h2 = 0;
    uint32_t h3 = 0;
    uint32_t h4 = 0;
    in.push_back(in.size());
    uint32_t k = 0;
    for(int i = 0; i < in.size(); ++i){
        k += in[i];
    }
    in.push_back(k);
    h1 = k ^ in.size();
    h2 = (k % in.size()) << 8 + k;
    h3 = -k | 0xAAAAAAAA;
    h4 = rrot(h1 ^ h2 ^ h3, 7);
    for(int i = 0; in.size() % 64; ++i){
        in.push_back(in[i%in.size()]);
    }
    for(int i = 0; i < in.size(); ++i){
        h1 += (uint32_t)(pow(2, i%32)/in[i]) ^ rrot(in[i], i%64);
        h2 += (uint32_t)(((uint32_t)(sqrt(2)*1000000) << 8)/(in[i]+1)) ^ rrot(i%64, in[i]);
        h3 += rrot(in[i] ^ i, i-7 );
        h4 += -1 ^ in[i] ^ i;
    }
    h1 += h1 ^ h2 ^ h4;
    h2 += h3 ^ h4 & h1;
    h4 += h3 ^ h2 | 1;
    h3 += (h4 ^ h2 ^ h1) & 0xAAAAAAAA;
    for(int i = 0; i < in.size(); ++i){
        h1 += rrot(h1, in[i] ^ ~i);
        h2 += rrot(h2, in[i] ^ i);
        h3 += rrot(h3, in[i] | i) & i;
        h4 += rrot(h4, in[i] | i) ^ i;
    }
    std::stringstream ss;
    ss  << std::hex << std::setfill('0') << std::setw(8) << h1 
        << std::setfill('0') << std::setw(8) << h2 
        << std::setfill('0') << std::setw(8) << h3 
        << std::setfill('0') << std::setw(8) << h4;
    return ss.str();
}


//quick way to hash something with or without salt, i.e. passwords for the users
/*
int main(){

    seed_rand();
    std::string in;
    std::cout << "What to hash? ";
    std::cin >> in;
    std::cout << "Salt it? (y/n) ";
    char x = 'n';
    std::cin >> x;
    if(x == 'y'){
        std::cout << salt_and_hash(in);
    }else {
        std::vector<char> v;
        for(char c : in) v.push_back(c);
        std::cout << hashVector(&v);
    }
    return 0;
}
*/
