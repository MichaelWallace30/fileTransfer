#include "server.h"
#include "client.h"
#include "userManager.h"
#include "packetHeader.h"
#include "base64.h"
#include "encryption.h"
#include "hash.h"
#include "fileManager.h"
#include <iomanip>
#include <stdlib.h>

#include <iostream>
using namespace std;
char selection;
bool isServer;
bool useBase64 = false;
int forceFailure = 0;
string encryptKey;
string IP;
string IOfileName;
string maxPacketString;




//the order for sending each packet should be
//1. Hash
//2. Encrypt
//3. (Optionally) Base64 encode
std::vector<char> * encodeMessage(std::vector<char>* buffer, std::string key, bool useBase64)
{
    buffer = hash_and_append(buffer);
    buffer = encryption(buffer, key);

    if (forceFailure)
    {
        int index = buffer->size() - 3;
        if (index < 0) index = 0;

        (*buffer)[index] ^= 1337;
    }

    if (useBase64)
    {
        buffer = encode64(buffer);
    }	
    return buffer;
}



//The order for receiving is the opposite of sending
//1. (Optionally) Decode from base64
//2. Decrypt
//3. Verify the hash
std::vector<char> * decodeMessage(std::vector<char>* buffer, std::string key, bool useBase64, bool &validHash)
{


    if (useBase64)
    {
        buffer = decode64(buffer);
    }

    buffer = encryption(buffer, key);

    std::string receivedHash = remove_hash(buffer);
    validHash = (hashVector(buffer) == receivedHash);
    //validHash = true;
    return buffer;
}

int main()
{

    do
    {
        cout << "1. Server.\n"
            << "2. Client.\n"
            << "Please enter your selection : ";
        cin >> selection;
        if (selection == '1')
        {
            isServer = true;
        }
        else if (selection == '2')
        {
            isServer = false;
        }
        else
        {
            cout << "Invalid selection.\n";
        }

    } while (selection != '1' && selection != '2');


    cout << "Whats Server IP? ";
    cin >> IP;

    cout << "Whats the encryption key file name? ";
    cin >> encryptKey;

    cout << "Whats input or output file name? ";
    cin >> IOfileName;

    do
    {
        cout << "Base64?\n";
        cout << "1. Yes\n"
            << "2. No\n";
        cin >> selection;
        if (selection == '1')
            useBase64 = true;
        else if (selection == '2')
            useBase64 = false;
    } while (selection != '1' && selection != '2');

    if (!isServer)
    {
        cout << "What is the max packet size? ";
        cin >> maxPacketString;

        do
        {
            cout << "Force failure? \n";
            cout << "1. No failure\n"
                << "2. Partial failure\n"
                << "3. Full failure\n";
            cin >> selection;
            if (selection == '1')
                forceFailure = 0;
            else if (selection == '2')
                forceFailure = 1;
            else if (selection == '3')
                forceFailure = 2;

        } while (selection != '1' && selection != '2' && selection != '3');
    }
    std::string keyString = readKey(encryptKey);

    std::vector<char> * data = new std::vector<char>();

    if (!isServer)
    {
        //user manager is used to add new user and validate exsistsing users
        userManager usermanager{};
        string name, pass;

        //create client
        client myClient;
        myClient.init(IP, 8888);


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
            myUserAuth.setName(name);
            myUserAuth.setPass(pass);

            messageHeader myHeader{ AUTH, 0, sizeof(myUserAuth) };
            data = myHeader.serialize(data);
            data = myUserAuth.serialize(data);

            //send user name and data  			
            data = encodeMessage(data, keyString, useBase64);
            myClient.sendVector(data);


            //wait for response
            myClient.recvVector(data);
            bool validHash = false;
            data = decodeMessage(data, keyString, useBase64, validHash);
            messageHeader authUserHeader;
            authUserHeader.deserialize(data);

            if (authUserHeader.messageType == SUCCESS)
            {
                cout << "auth succes" << endl;
                done = true;
            }

            else if (authUserHeader.messageType == FAILED)
            {
                cout << "auth failure" << endl << endl;;
            }
            attempts++;
        }//end attempts  


        if (done)
        {
            data = fileToVector(IOfileName);
			if (data->size() == 0) return 1;
            int maxPacketSize = std::stoi(maxPacketString);//bytes
            int size = data->size();
            int numPackets = (size % maxPacketSize) ? size / maxPacketSize + 1 : size / maxPacketSize;
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
                int currentPacketSize = size - (maxPacketSize * (packetNumber - 1));
                currentPacketSize = currentPacketSize > maxPacketSize ? maxPacketSize : currentPacketSize;


                messageHeader myHeader{ PACKET, (uint32_t)numPackets, (uint32_t)currentPacketSize };
                buffer->clear();
                buffer = myHeader.serialize(buffer);
                for (int x = 0; x < currentPacketSize; x++)
                {
                    buffer->push_back((*data)[x + offset]);
                }

                myHeader.dataLength = buffer->size();
                buffer = encodeMessage(buffer, keyString, useBase64);
                packetSucces = false;



                while (attempts < maxAttempts && !packetSucces)
                {

                    //send packet                    
                    myClient.sendVector(buffer);

                    //wait for response
                    recvBuffer->clear();
                    myClient.recvVector(recvBuffer);
                    bool validHash = false;
                    recvBuffer = decodeMessage(recvBuffer, keyString, useBase64, validHash);
                    messageHeader recvHeader;
                    recvHeader.deserialize(recvBuffer);
                    if (recvHeader.messageType == SUCCESS)
                    {
                        offset += currentPacketSize;
                        packetSucces = true;
                        attempts = 0;
                        if (packetNumber == numPackets)done = true;
                        packetNumber++;

                        //fix formating
                        double progress = (double(packetNumber - 1) / double(numPackets)) * 100;

                        cout << "Progress: ";
                        for (int x = 0; x <= 100; x += 2)
                        {
                            if (x < progress)
                            {
                                cout << "=";
                            }
                            else
                            {
                                cout << " ";
                            }
                        }
                        cout << " >" ;
						cout <<setw(3)<<setprecision(3)<< (int)progress;
						cout << "%\r";
                        
                        

                    }
                    else
                    {
                        cout << "\nCorrupt packet sent retyring." << endl;
                        if (forceFailure == 1) forceFailure = 0;
                        attempts++;
                        packetSucces = false;
                    }
                }

                if (!packetSucces)
                {
                    //end program
                    cout << "\nPacket corruption progam done." << endl;
                    return 0;
                }

            }

            if (done)
            {
                data->clear();
                messageHeader myHeader{ END, 0, 0 };
                data = myHeader.serialize(data);
                data = encodeMessage(data, keyString, useBase64);
                myClient.sendVector(data);
                printf("\nFile transfer complete\n");
                return 1;
            }



        }
        else//auth failure
        {
            //we are done exit program
            cout << "auth failure program will exit" << endl;
            return 0;
        }

    }//server mode
    else
    {
        //CREATE SERVER 
        server myServer;
        myServer.init(IP, 8888);

        int maxAttempts = 3;
        int attempts = 0;
        bool done = false;

        while (attempts < maxAttempts && !done)
        {
            //wait for message              
            myServer.recvVector(data);
            bool validHash = false;
            data = decodeMessage(data, keyString, useBase64, validHash);

            messageHeader myHeader;
            myHeader.deserialize(data);

            if (myHeader.messageType == AUTH)
            {
                userManager myUserManager;
                userAuth myUserAuth;
                myUserAuth.deserialize(data, HEADER_SIZE);
                bool validUser = myUserManager.validate(myUserAuth.getName(), myUserAuth.getPass());
                //validUser = true;

                if (validUser)
                {                    
                    messageHeader authHeader{ SUCCESS, 0, 0 };
                    data = authHeader.serialize(data);      
                    data = encodeMessage(data, keyString, useBase64);
                    myServer.sendVector(data);
                    done = true;
                    cout << "auth success\n" << endl;

                }
                else
                {
                    messageHeader authHeader{ FAILED, 0, 0 };
                    data = authHeader.serialize(data);     
                    data = encodeMessage(data, keyString, useBase64);
                    myServer.sendVector(data);
                    cout << "auth failure\n" << endl << endl;;

                }
            }
            attempts++;
        }//end attempts

        std::vector<char>* recvVector = new std::vector<char>();

         //get start and num packets
        bool validHash = false;

		int numberOfPacketsExpected = 0;
        int numberOfPacketsRecieved = 0;
        
        attempts = 1;
        if (done)
        {
            //move on
            done = false;


            while (!done && attempts <= maxAttempts)
            {

                myServer.recvVector(data);
                bool validHash = false;
                data = decodeMessage(data, keyString, useBase64, validHash);


                if (!validHash) {
                    cout << "\nHash wrong, retry requested." << endl;

                    messageHeader responseHeader{ FAILED, 0, 0 };
                    data = responseHeader.serialize(data);
                    data = encodeMessage(data, keyString, useBase64);
                    myServer.sendVector(data);
                    attempts++;
                }
                else
                {
                    messageHeader recvHeader;
                    recvHeader.deserialize(data);
					if (numberOfPacketsExpected == 0)numberOfPacketsExpected = recvHeader.packet;
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

                        attempts = 1;
                        messageHeader responseHeader{ SUCCESS, 0, 0 };
                        data = responseHeader.serialize(data);
                        data = encodeMessage(data, keyString, useBase64);
                        myServer.sendVector(data);
                        numberOfPacketsRecieved++;

                        //fix format
                        double progress = ((double)numberOfPacketsRecieved / (double)numberOfPacketsExpected) * 100;

						cout << "Progress: ";
						for (int x = 0; x <= 100; x += 2)
						{
							if (x < progress)
							{
								cout << "=";
							}
							else
							{
								cout << " ";
							}
						}
						cout << "> ";
						cout << setw(3) << setprecision(3) << (int)progress;
						cout << "%\r";
                        
                    }
                    else
                    {
                        done = true;
                    }
                }
            }

            if (attempts < maxAttempts)
            {
                vectorToFile(IOfileName, recvVector);
                printf("\nFile transfer complete\n");
                return 0;
            }
            else
            {
                cout << "\npacket corruption program done." << endl;
            }
          
            return 1;

        }
        else//auth failure
        {
            //we are done exit program
            cout << "auth failure program will exit" << endl;
            return 0;
        }
    }



}
