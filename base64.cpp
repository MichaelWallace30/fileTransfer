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

//A = 65
//a = 97

//example: Hello World!!!   ==    SGVsbG8gV29ybGQhISE=

#include "base64.h"
//function will take 6 bit integer and conver to assic letter
char sixToEight(uint8_t value)
{
	if (value == 0) return '=';
	if (value <= 90)
	{
		return value + 'A';
	}
	//else
	return value + 'a';

}

//function will take 8 bit letter and convert to 6 bit integer
uint8_t EightToSix(char value)
{
	if (value == '=')return 0;
	if (value <= 'Z')
	{
		return value - 'A';
	}
	//else
	return value - 'a';
}

#include <vector>
std::vector<char>* encode64(std::vector<char>* buffer)
{

	//check for padding and pad
	while ((*buffer).size() % BLOCK_SIZE != 0)
	{
		buffer->push_back(0);
	}

	char array[BLOCK_SIZE_64] = { 0 };

	array[0] = ((*buffer)[0] & 0xfc) >> 2;
	array[1] = (((*buffer)[0] & 0x03) << 4) + (((*buffer)[1] & 0xf0) >> 4);
	array[2] = (((*buffer)[1] & 0x0f) << 2) + (((*buffer)[2] & 0xc0) >> 6);
	array[3] = (*buffer)[2] & 0x3f;

	//CALUCALTE NEW SIZE of vector
	int newSize = buffer->size();	
	if (newSize % BLOCK_SIZE != 0)
	{
		newSize += newSize % BLOCK_SIZE;
	}
	newSize = (newSize / BLOCK_SIZE) * BLOCK_SIZE_64;

	//COPY to new vector
	std::vector<char>* buffer2 = new std::vector<char>;
	buffer2->resize(newSize);
	
	(*buffer2)[0] = sixToEight(array[0]);
	(*buffer2)[1] = sixToEight(array[1]);
	(*buffer2)[2] = sixToEight(array[2]);
	(*buffer2)[3] = sixToEight(array[3]);


	//delte old vector
	delete buffer;
	return buffer2;
	
}

std::vector<char>* decode64(std::vector<char>* buffer)
{
	return NULL;
}