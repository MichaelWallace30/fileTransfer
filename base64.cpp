#include<iostream>
using namespace std;
/*
encode: 6bit to 8bit
decode: 8bit to 6bit

needs padding if:
convert 3 bytes of 8 bit data into 4 chunks of 6 bits each
don't add new value of 0 to buffer use = sign

value char
0	A	16	Q	32	g	48	w
1	B	17	R	33	h	49	x
2	C	18	S	34	i	50	y
3	D	19	T	35	j	51	z
4	E	20	U	36	k	52	0
5	F	21	V	37	l	53	1
6	G	22	W	38	m	54	2
7	H	23	X	39	n	55	3
8	I	24	Y	40	o	56	4
9	J	25	Z	41	p	57	5
10	K	26	a	42	q	58	6
11	L	27	b	43	r	59	7
12	M	28	c	44	s	60	8
13	N	29	d	45	t	61	9
14	O	30	e	46	u	62	+
15	P	31	f	47	v	63	/
*/

//A = 255
//a = 97

//example: Hello World!!!   ==    SGVsbG8gV29ybGQhISE=

#include "base64.h"

const std::string assciiBase64LookUp ="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

#include <vector>
std::vector<char>* encode64(std::vector<char>* buffer)
{
	int padding = 0;
	//check for padding and pad extra zeros so we don't go out of scope of vector
	while ((*buffer).size() % BLOCK_SIZE != 0)
	{
		buffer->push_back(0);
		padding++;
	}

	char array[BLOCK_SIZE_64] = { 0 };
	//CALUCALTE NEW SIZE of new vector 
	int newSize = buffer->size();
	if (newSize % BLOCK_SIZE != 0)
	{
		newSize += newSize % BLOCK_SIZE;
	}
	newSize = (newSize / BLOCK_SIZE) * BLOCK_SIZE_64;

	//COPY to new vector
	std::vector<char>* buffer2 = new std::vector<char>;
	buffer2->resize(newSize);


	//encode and copy to new vector
	for (int i = 0; i < buffer->size() /3; i++)
	{
		//convert to base64 by base64 lookuptable index
		array[0] = ((*buffer)[0 + (i * 3)] & 0xfc) >> 2;
		array[1] = (((*buffer)[0 + (i * 3)] & 0x03) << 4) + (((*buffer)[1 + (i * 3)] & 0xf0) >> 4);
		array[2] = (((*buffer)[1 + (i * 3)] & 0x0f) << 2) + (((*buffer)[2 + (i * 3)] & 0xc0) >> 6);
		array[3] = (*buffer)[2 + (i * 3)] & 0x3f;

		//use look up table index to set value of base64
		(*buffer2)[0 + (i * 4)] = assciiBase64LookUp[array[0]];
		(*buffer2)[1 + (i * 4)] = assciiBase64LookUp[array[1]];
		(*buffer2)[2 + (i * 4)] = assciiBase64LookUp[array[2]];
		(*buffer2)[3 + (i * 4)] = assciiBase64LookUp[array[3]];
	}

	//add padding symbol '=' for lengths not module 0 of block size
	for (int x = 1; x <= padding; x++)
	{
		(*buffer2)[newSize - x] = '=';
	}

	//delete old vector
	delete buffer;
	return buffer2;
	
}

std::vector<char>* decode64(std::vector<char>* buffer)
{
	std::vector<char>* buffer2 = new std::vector<char>;
	int newSize = (buffer->size() / BLOCK_SIZE_64) * BLOCK_SIZE;
	
	//check for '=' padding
	int sizeReduction = 0;
	for (int i = buffer->size() - 1; i > buffer->size() - BLOCK_SIZE - 1; i --)
	{
		if ((*buffer)[i] == '=')
		{
			sizeReduction++;
		}
	}

	//reszie for non needed '='
	buffer2->resize(newSize - sizeReduction);
	//decode and copy to new vector
	for (int i = 0; i < newSize / 3; i++)
	{
		int index = i * 3;
		(*buffer2)[index++] = (((assciiBase64LookUp.find((*buffer)[0 + i * 4]) & 0x3f) << 2) + ((assciiBase64LookUp.find((*buffer)[1 + i * 4]) & 0x30) >> 4));
		if(index < (newSize - sizeReduction))(*buffer2)[index++] = (((assciiBase64LookUp.find((*buffer)[1 + i * 4]) & 0x0F) << 4) + ((assciiBase64LookUp.find((*buffer)[2 + i * 4]) & 0x3C) >> 2));
		if(index < (newSize - sizeReduction))(*buffer2)[index] = (((assciiBase64LookUp.find((*buffer)[2 + i * 4]) & 0x03) << 6) + (assciiBase64LookUp.find((*buffer)[3 + i * 4]) & 0x3f));
	}

	//delete old vector buffer return new
	delete buffer;
	return buffer2;
}

