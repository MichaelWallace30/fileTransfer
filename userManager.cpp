#include "userManager.h"
#define SALT_SIZE 6

userManager::userManager()
{
}


userManager::~userManager()
{
}


bool userManager::validate(std::string user, std::string pass)
{

    //hash pass and store results in pass

    std::ifstream userFile(FILE_NAME);

    if (userFile.is_open())
    {
        std::string subString;
        while (getline(userFile, subString))
        {
            std::size_t pos = subString.find(' ');
            std::string name = subString.substr(0, pos);

            //remove salt from hash
            std::string passWord = subString.substr(pos + 1);

            if (user == name && pass == passWord)
            {
                return true;
            }
        }
        userFile.close();
    }

    return false;
}

bool userManager::create(std::string user, std::string pass)
{
    
    for (int x = 0; x < user.length(); x++)
    {
        if (user[x] == ' ')
        {
            printf("Error user name may not contain spaces!\n");
            return false;
        }
    }

    
    std::fstream userFile;
    userFile.open(FILE_NAME, std::ios_base::app);

    if (userFile.is_open())
    {
        //hash this pass word
        std::string userInfo = user + ' ' + pass;
        userFile << userInfo;
        userFile.close();
        return true;
    }
    

    
    printf("user file failed to open");
    return false;
}