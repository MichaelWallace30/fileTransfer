
#include "encryption.h"
#include <iostream>

std::vector<char>* encryption(std::vector<char>* buffer, int length, std::string key)
{
	for (int index = 0; index < length; index++)
	{
		//std::cout<<index<<"\t" <<buffer[index]<<"\t"<<index % KEY_LENGTH<<"\t"<<  key[index%KEY_LENGTH] << std::endl;
		(*buffer)[index] ^= key[index % KEY_LENGTH];
	}

	return buffer;
}