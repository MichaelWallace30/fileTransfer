#include "server.h"
#include "client.h"
#include "userManager.h"
#include "packetHeader.h"
#include "base64.h"
#include "encryption.h"
#include "hash.h"
#include "fileManager.h"

#include <stdlib.h>

#include <iostream>
using namespace std;
bool useBase64 = false;


std::vector<char> * encodeMessage(std::vector<char>* buffer, std::string key, bool useBase64)
{
    if (useBase64)
    {
        buffer = encode64(buffer);
    }
    buffer = encryption(buffer, key);
    return buffer;
}

std::vector<char> * decodeMessage(std::vector<char>* buffer, std::string key, bool useBase64)
{

    buffer = encryption(buffer, key);
    if (useBase64)
    {
        buffer = decode64(buffer);
    }

    return buffer;
}

int main()
{

    std::string keyString = readKey("key.txt");



    std::vector<char> * data = new std::vector<char>();

    bool isClient;
    cout << "client? ";
    cin >> isClient;

    if (isClient)
    {
        //user manager is used to add new user and validate exsistsing users
        userManager usermanager{};
        string name, pass;

        //create client
        client myClient;
        myClient.init("127.0.0.1", 8888);

        
        int maxAttempts = 3;
        int attempts = 0;
        bool done = false;

        while (attempts < maxAttempts && !done)
        {
            //get user info
            cout << "Name: ";
            cin >> name;
            cout << "Password: ";
            cin >> pass;

            //add user name and pass to data
            userAuth myUserAuth;
            myUserAuth.base64 = useBase64;
            myUserAuth.setName(name);
            myUserAuth.setPass(pass);

            messageHeader myHeader{ AUTH, 0, sizeof(myUserAuth) };
            data = myHeader.serialize(data);
            data = myUserAuth.serialize(data);

            //send user name and data
            //append hash fist            
            myClient.sendVector(data);


            //wait for response
            //check hash fist
            myClient.recvVector(data);
            messageHeader authUserHeader;
            authUserHeader.deserialize(data);

            if (authUserHeader.messageType == SUCCESS)
            {
                cout << "auth succes" << endl;
                done = true;
            }
            else if (authUserHeader.messageType == FAILED)
            {
                cout << "auth failure" << endl;                
            }
            attempts++;
        }//end attempts  
    

        if (done)
        {
            //move on
            //temp data             load file
            data->resize(0);
            for (int x = 0; x < 10; x++)
            {
                data->push_back('*');
            }
            
            int maxPacketSize = 10;//bytes
            int size = 10;

            int numPackets = size / maxPacketSize;

            done = false;
            bool packetSucces = false;
            std::vector<char>* buffer = new std::vector<char>();
            std::vector<char>* recvBuffer = new std::vector<char>();
            int offset = 0;
            int packetNumber = 1;
            int attempts = 0;
            int maxAttempts = 3;
            
            while (!done)
            {
                int currentPacketSize = 10;
                messageHeader myHeader{ PACKET, (uint32_t)packetNumber, (uint32_t)currentPacketSize};
                buffer->resize(0);
                buffer= myHeader.serialize(buffer);
                for (int x = 0; x < currentPacketSize; x++)
                {
                    buffer->push_back((*data)[x + offset]);
                }


                //the order for sending each packet should be
                //1. Hash
                //2. Encrypt
                //3. (Optionally) Base64 encode
                buffer = hash_and_append(buffer);

                buffer = encodeMessage(buffer, keyString, useBase64);
                myHeader.dataLength = buffer->size();

                packetSucces = false;
                while (attempts < maxAttempts && !packetSucces)
                {

                    //send packet
                    //hash
                    myClient.sendVector(buffer);

                    //wait for response
                    //hash
                    recvBuffer->resize(0);
                    myClient.recvVector(recvBuffer);
                    messageHeader recvHeader;
                    recvHeader.deserialize(recvBuffer);
                    if (recvHeader.messageType == SUCCESS)
                    {
                        offset += currentPacketSize;
                        packetSucces = true;
                        attempts = 0;
                        if (packetNumber == numPackets)done = true;
                        packetNumber++;
                        
                    }
                    else
                    {
                        attempts++;
                        packetSucces = false;
                    }
                }

                if (!packetSucces)
                {
                    //end program
                    cout << "Packet corruption progam done." << endl;
                    exit(0);
                }

            }

            if (done)
            {
                data->resize(0);
                messageHeader myHeader{ END, 0, 0 };
                data = myHeader.serialize(data);
                data = hash_and_append(data);
                data = encodeMessage(data, keyString, useBase64);
                myClient.sendVector(data);
                exit(1);
            }



        }
        else//auth failure
        {
            //we are done exit program
            cout << "auth failure program will exit" << endl;
            exit(0);
        }
        
    }//server mode
    else
    {
        //CREATE SERVER 
        server myServer;
        myServer.init("127.0.0.1", 8888);

        int maxAttempts = 3;
        int attempts = 0;
        bool done = false;

        while (attempts < maxAttempts && !done)
        {
            //wait for message  
            //check hash fist
            myServer.recvVector(data);

            messageHeader myHeader;
            myHeader.deserialize(data);

            if (myHeader.messageType == AUTH)
            {
                userManager myUserManager;
                userAuth myUserAuth;
                myUserAuth.deserialize(data, HEADER_SIZE);
                bool validUser = myUserManager.validate(myUserAuth.getName(), myUserAuth.getPass(), "salt");
                validUser = true;

                if (validUser)
                {
                    useBase64 = myUserAuth.base64;
                    messageHeader authHeader{ SUCCESS, 0, 0 };
                    data = authHeader.serialize(data);
                    //append hash fist
                    myServer.sendVector(data);
                    done = true;
                    cout << "auth success" << endl;

                }
                else
                {
                    messageHeader authHeader{ FAILED, 0, 0 };
                    data = authHeader.serialize(data);
                    //append hash fist
                    myServer.sendVector(data);
                    cout << "auth failure" << endl;

                }
            }
            attempts++;
        }//end attempts

        if (done)
        {
            //move on
            done = false;
            std::vector<char>* recvVector = new std::vector<char>();

            while (!done)
            {           

                myServer.recvVector(data);

                //The order for receiving is the opposite of sending
                //1. (Optionally) Decode from base64
                //2. Decrypt
                //3. Verify the hash
                data = decodeMessage(data, keyString, useBase64);

                std::string receivedHash = remove_hash(data);
                if (hashVector(data) != receivedHash) {
                    cout << "hash wrong, exiting" << endl;
                    exit(2);
                }


                messageHeader recvHeader;
                recvHeader.deserialize(data);

                if (recvHeader.messageType != END)
                {
                    std::vector<char>* tempData = new std::vector<char>();
                    
                    for (int x = 12; x < recvHeader.dataLength + 12; x++)
                    {
                        tempData->push_back((*data)[x]);
                    }

                    for (int x = 0; x < recvHeader.dataLength; x++)
                    {
                        recvVector->push_back((*tempData)[x]);
                    }

                    //check hash                    
                    messageHeader responseHeader{ SUCCESS, 0, 0 };
                    data = responseHeader.serialize(data);
                    myServer.sendVector(data);
                }
                else
                {
                    done = true;                    
                }
            }


            
            for (int x = 0; x < recvVector->size(); x++)
            {
                cout << (char)(*recvVector)[x] << endl;
            }
            exit(1);
            
        }
        else//auth failure
        {
            //we are done exit program
            cout << "auth failure program will exit" << endl;
            exit(0);
        }
    }









    //cout << usermanager.validate(name, pass)<<endl;

    


}
