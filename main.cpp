#include "server.h"
#include "client.h"
#include "userManager.h"
#include "packetHeader.h"

#include <stdlib.h>

#include <iostream>
using namespace std;

int main()
{
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
        myClient.init("192.168.1.7", 8888);

        
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
            myUserAuth.base64 = false;
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
            //temp data            
            data->resize(0);
            for (int x = 0; x < 10000; x++)
            {
                data->push_back(x);
            }
            
            int maxPacketSize = 100;//bytes
            int size = 10000;

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
                int currentPacketSize = (size - (packetNumber * maxPacketSize));
                messageHeader myHeader{ PACKET, (uint32_t)packetNumber, (uint32_t)currentPacketSize};
                buffer->resize(0);
                buffer= myHeader.serialize(buffer);
                for (int x = 0; x < currentPacketSize; x++)
                {
                    buffer->push_back((*data)[x + offset]);
                }

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
        myServer.init("192.168.1.7", 8888);

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
                bool validUser = myUserManager.validate(myUserAuth.getName(), myUserAuth.getPass());

                if (validUser)
                {
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
            
                messageHeader recvHeader;
                myServer.recvVector(data);
                recvHeader.deserialize(data);                

                if (recvHeader.messageType != END)
                {
                    for (int x = 0; x < recvHeader.dataLength; x++)
                    {
                        recvVector->push_back((*data)[x + HEADER_SIZE]);
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