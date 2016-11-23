#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H

#include <fstream>
#include <vector>
#include <string>


std::string readKey(std::string fileName);
std::vector<char>* fileToVector(std::string fileName);
bool vectorToFile(std::string fileName, std::vector<char> *buffer);
#endif