#include "server.h"
#include "client.h"
#include "userManager.h"
#include "packetHeader.h"


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
            messageHeader myHeader{ PACKET, 1, 10000 };
            data = myHeader.serialize(data);

            for (int x = 0; x < 10000; x++)
            {
                data->push_back(x);
            }

            myClient.sendVector(data);

            myClient.recvVector(data);

            myHeader.deserialize(data);

        }
        else//auth failure
        {
            //we are done exit program
            cout << "auth failure program will exit" << endl;
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

            messageHeader recvHeader;
            myServer.recvVector(data);
            recvHeader.deserialize(data);


            std::vector<char> *packet = new std::vector<char>();

            for (int x = 0; x < recvHeader.dataLength; x++)
            {
                packet->push_back((*data)[x + HEADER_SIZE]);
                cout << (char)(*packet)[x] << endl;
            }


            
            
        }
        else//auth failure
        {
            //we are done exit program
            cout << "auth failure program will exit" << endl;
        }
    }









    //cout << usermanager.validate(name, pass)<<endl;

    


}