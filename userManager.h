#ifndef USER_MANAGER_H
#define USER_MANAGER_H

#include <string>
#include "hash.h"
#include <fstream>
#define FILE_NAME "user.txt"
#define MAX_LENGTH 100
class userManager
{
public:
    userManager();
    ~userManager();

    
    bool validate(std::string user, std::string pass);
    bool create(std::string user, std::string pass);


};


class userAuth
{
public:
    char name[MAX_LENGTH];
    char pass[MAX_LENGTH];
    bool base64;

    void setName(std::string stringName)
    {
        int x = 0;
        for (; x < stringName.length(); x++)
        {
            name[x] = stringName[x];
        }
        name[x] = '\0';
    }

    std::string getName()
    {
        std::string nameStr = "";
        int x = 0;
        while (name[x] != '\0')
        {
            nameStr += name[x];
            x++;
        }
        return nameStr;
    }

    void setPass(std::string stringPass)
    {
        int x = 0;
        for (; x < stringPass.length(); x++)
        {
            pass[x] = stringPass[x];
        }
        name[x] = '\0';
    }

    std::string getPass()
    {
        std::string passStr = "";
        int x = 0;
        while (pass[x] != '\0')
        {
            passStr += pass[x];
            x++;
        }
        return passStr;
    }
    
    std::vector<char>* serialize(std::vector<char>* buffer)
    {

        for (int x = 0; x < MAX_LENGTH; x++)
        {
            buffer->push_back(name[x]);
        }

        for (int x = 0; x < MAX_LENGTH; x++)
        {
            buffer->push_back(pass[x]);
        }

        buffer->push_back(base64);

        return buffer;
    }

    void deserialize(std::vector<char>* buffer, int offset)
    {

        for (int x = 0; x < MAX_LENGTH; x++)
        {
            name[x] = (*buffer)[offset++];
        }

        
        for (int x = 0; x < MAX_LENGTH; x++)
        {
            pass[x] = (*buffer)[offset++];
        }

        base64 = (*buffer)[offset++];
    }
};

#endif