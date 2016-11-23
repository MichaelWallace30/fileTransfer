//Taylor R. Stiles
#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <iomanip>
#include <sstream>
#include <ctime>
#include "hash.h"

template<class T>
T rrot(T x, unsigned int moves)
{
    return (x >> moves) | (x << sizeof(T)*8 - moves);
}

//call this once just to seed rand at the start
void seed_rand(){
    std::srand(std::time(0));
}

//this adds a salt and appends it
static const char salt_chars[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890";
std::string salt_and_hash(std::string s){
    std::stringstream ss;
    for(int i = 0; i < 6; ++i)
        ss << salt_chars[std::rand()%(sizeof(salt_chars)-1)];
    s.append(":");
    s.append(ss.str());
    std::vector<char> in;
    for (char c : s) in.push_back(c);
    return hash(&in);
}

//appends the hash to the vector
std::vector<char>* hash_and_append(std::vector<char>* s){
    std::string h = hash(s);
    s->push_back(':');
    for(char c : h){
        s->push_back(c);
    }
    return s;
}

//generates the hash
std::string hash(std::vector<char>* s){
    std::vector<char> in = *s;
    unsigned long h1 = 0;
    unsigned long h2 = 0;
    unsigned long h3 = 0;
    unsigned long h4 = 0;
    in.push_back(in.size());
    unsigned long k = 0;
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
        h1 += (unsigned long)(pow(2, i%32)/in[i]) ^ rrot(in[i], i%64);
        h2 += (unsigned long)(((unsigned long)(sqrt(2)*1000000) << 8)/(in[i]+1)) ^ rrot(i%64, in[i]);
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

