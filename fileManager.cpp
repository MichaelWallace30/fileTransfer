#include "fileManager.h"


std::string readKey(std::string fileName)
{
    std::ifstream tempFile(fileName);
    std::string tempString = "";

    if (tempFile.is_open())
    {
        std::string subString = "";

        while (getline(tempFile, subString))
        {
            tempString += subString;
        }

        tempFile.close();
    }
    return tempString;
}


std::vector<char>* fileToVector(std::string fileName)
{   
    printf("\nLoading file please wait...\n\n");
    std::ifstream myFile(fileName, std::ios_base::binary | std::ios_base::in);
	std::vector<char>* newVector = new std::vector<char>();
    if (myFile.is_open())
    {
        std::vector<char> buffer((std::istreambuf_iterator<char>(myFile)),
            (std::istreambuf_iterator<char>()));
        myFile.close();


        

        for (int x = 0; x < buffer.size(); x++)
        {
            newVector->push_back(buffer[x]);
        }
		printf("Done loading\n");
        return newVector;
    }    
    printf("\nfailed to open exsisting file\n");
    return newVector;    
}


bool vectorToFile(std::string fileName, std::vector<char> *buffer)
{
    printf("\nSaving file please wait...");
    std::ofstream yourFile(fileName, std::ios_base::binary | std::ios_base::out);

    if (yourFile.is_open())
    {
        std::vector<char> tempVector;

        for (int x = 0; x < buffer->size(); x++)
        {
            tempVector.push_back((*buffer)[x]);
        }



        yourFile.write(reinterpret_cast<char*>(&tempVector[0]), tempVector.size());

        yourFile.close();
        return true;
    }
    printf("\nfailed to open new file\n");
    return false;
}
