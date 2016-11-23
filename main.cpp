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
        //get user info
        string name2, pass2;
        cout << "Name: ";
        cin >> name;
        cout << "Password: ";
        cin >> pass;

        //create client
        client myClient;
        myClient.init("127.0.0.1", 8888);

        
        //add user name and pass to data
        userAuth myUserAuth;
        myUserAuth.base64 = false;
        myUserAuth.setName(name);
        myUserAuth.setPass(pass);        

        messageHeader myHeader{ AUTH, 0, sizeof(myUserAuth)};
        myHeader.serialize(data);
        myUserAuth.serialize(data);

        //send user name and data
        myClient.sendVector(data);


        //wait for response
        myClient.recvVector(data);
        messageHeader authUserHeader;
        authUserHeader.deserialize(data);

        if (authUserHeader.messageType == SUCCESS)
        {
            cout << "succes" << endl;
            //move on
        }
        else if (authUserHeader.messageType == FAILED)
        {
            cout << " auth failure" << endl;
        }
        
    }
    else
    {
        //CREATE SERVER 
        server myServer;
        myServer.init("127.0.0.1", 8888);

        //wait for message
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
                authHeader.serialize(data);
                myServer.sendVector(data);

            }
            else
            {
                messageHeader authHeader{ FAILED, 0, 0 };
                authHeader.serialize(data);
                myServer.sendVector(data);
            }

            
        }
    }









    //cout << usermanager.validate(name, pass)<<endl;

    


}