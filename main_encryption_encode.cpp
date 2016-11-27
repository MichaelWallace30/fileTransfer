#include "encryption.h"
#include "base64.h"

#include <iostream>
using namespace std;


int main()
{

	/*
	// all xor encryptino test
	int const SIZE = 16;
	vector<char>* buffer = new vector<char>{ 'H', 'e', 'l', 'l', 'e', ' ', 'W', 'o', 'r', 'l', 'd', '!', '!', '!'};
	
	string key = "Hello World!!!";
	string key2 = "World!!! Hello";

	encryption(buffer, key);


	//key one should be all ' '
	cout << "encryption 1: " << endl;
	for (int x = 0; x < buffer->size(); x++)
	{
		cout << (*buffer)[x];
	}
	cout << endl << endl;;


	encryption(buffer, key);
	//key one should be key
	cout << "encryption 2: " << endl;
	for (int x = 0; x < buffer->size(); x++)
	{
		cout << (*buffer)[x];
	}
	cout << endl << endl;
	*/

	//Hel == SGVs
	
    vector<char>* buffer = new vector<char>();
    vector<char>* buffer2 = new vector<char>();

    string key = "Hello World!!!";
    

    
    std::string value = "*&**&&*(*(*(*&(*(*(*(*(**&";

    for (int x = 0; x < value.length(); x++)
    {
        buffer->push_back(value[x]);
    }

    encryption(buffer, key);
	buffer = encode64(buffer);

    for (int x = 0; x < buffer->size(); x++)
    {
        cout << (*buffer)[x];
    }
    cout << endl;

	buffer2 = decode64(buffer);
    encryption(buffer2, key);

	cout << buffer2->size() << endl;
	for (int x = 0; x < buffer2->size(); x++)
	{
		cout << (*buffer2)[x];
	}
	cout << endl;	
}

