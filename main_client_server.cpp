#include "client.h"
#include "server.h"
#include <thread>



int main()
{

    std::vector<char>* myVector = new std::vector<char>();

    for (int x = 0; x < 10; x++)
    {
        myVector->push_back(x);
    }
    /*
    
    server  myServer;
    myServer.init();
    */
    
   client myServer;
   myServer.init("127.0.0.1", 5001);
    
  
    
   
    while (true)
    {
        myServer.sendVector(myVector);
        do
        {
            myVector = myServer.recvVector(myVector);
        } while (myVector->size() == 0);

      

    }
    
    
    
}
