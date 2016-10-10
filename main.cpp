#include "encryption.h"
#include <iostream>


using namespace std;

int main()
{

	// all xor encryptino test
	int const SIZE = 16;
	vector<char>* buffer = new vector<char>{ 'H', 'e', 'l', 'l', 'e', ' ', 'W', 'o', 'r', 'l', 'd', '!', '!', '!'};
	
	string key = "Hello World!!!";
	string key2 = "World!!! Hello";

	encryption(buffer, buffer->size(), key);


	//key one should be all ' '
	cout << "encryption 1: " << endl;
	for (int x = 0; x < buffer->size(); x++)
	{
		cout << (*buffer)[x];
	}
	cout << endl << endl;;


	encryption(buffer, buffer->size(), key);
	//key one should be key
	cout << "encryption 2: " << endl;
	for (int x = 0; x < buffer->size(); x++)
	{
		cout << (*buffer)[x];
	}
	cout << endl << endl;;
	
}