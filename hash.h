//Taylor R. Stiles
#ifndef HASH_H
#define HASH_H
#include <vector>
template<class T> T rrot(T x, unsigned int moves);
void seed_rand();
std::string hash(std::vector<char>* s);
std::vector<char>* hash_and_append(std::vector<char>* s);
std::vector<char>* salt_and_hash(std::vector<char>* s);

#endif