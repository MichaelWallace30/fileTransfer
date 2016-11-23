
#include <stdint.h>
#include <vector>
#include <cstring>//memcpy

#ifndef _HEADER_PACKET_H
#define _HEADER_PACKET_H

enum packetType {AUTH, START, END, SUCCESS, FAILED, PACKET};

#define HEADER_SIZE 12
class messageHeader
{
public:
	packetType messageType;
	uint32_t packet;
	uint32_t dataLength;	

    messageHeader() :dataLength(0), packet(0), messageType(START)
    {

    }
    messageHeader(packetType messageType, uint32_t packet, uint32_t datalength)
    {
        this->dataLength = datalength;
        this->packet = packet;
        this->messageType = messageType;
    }

	void deserialize(std::vector<char>* buffer)
	{

        if (buffer->size() >= HEADER_SIZE)
        {
            char arr[HEADER_SIZE] = { 0 };
            for (int x = 0; x < HEADER_SIZE; x++)
            {
                arr[x] = (*buffer)[x];
            }

            //seralize header
            memcpy(&this->messageType, arr, 4);
            memcpy(&this->packet, arr + 4, 4);
            memcpy(&this->dataLength, arr + 8, 4);

        }
	}

    std::vector<char>* serialize(std::vector<char>* buffer)
    {

        //wipe buffer
        delete buffer;
        buffer = new std::vector<char>();
        char arr[HEADER_SIZE] = { 0 };

        

        memcpy(arr, &this->messageType, 4);
        memcpy(arr + 4, &this->packet, 4);
        memcpy(arr+ 8, &this->dataLength, 4);

        for (int x = 0; x < HEADER_SIZE; x++)
        {
            buffer->push_back(arr[x]);
        }
        
		return buffer;
	}
};




#endif