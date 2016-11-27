#include <vector>

#ifndef _HASH_H
#define _HASH_H

template<class T> T rrot(T x, unsigned int moves);
void seed_rand();
std::string hashVector(std::vector<char>* s);
std::vector<char>* hash_and_append(std::vector<char>* s);
std::string salt_and_hash(std::string s);
bool compare_salted(std::string salted_pass, std::string pass_to_check);
std::string remove_hash(std::vector<char>* s);

#endif