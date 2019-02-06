/* Socket programming
 * Group: 29
 * Team members: Kevin Liao, Kevin Van
 */

#include <vector>
#include <utility>      // std::std::pair
#include <iostream>
#include <fstream>
#include <string>
#include <tuple>
#include <typeinfo>
#include <algorithm> 
#include <exception>
#include <stdio.h>      /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket() and bind() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_ntoa() */
#include <stdlib.h>     /* for atoi() and exit() */
#include <string.h>     /* for memset() */
#include <unistd.h>     /* for close() */
#include <sstream>

#define ECHOMAX 255     /* Longest string to echo */

typedef std::tuple<std::string,std::string,int> contact;
typedef std::vector< std::pair< std::string,std::vector<contact> > > list;

// Functions


void DieWithError(const char *errorMessage) /* External error handling function */
{
    perror(errorMessage);
    exit(1);
}

//query lists
std::pair<int, std::vector<std::string> > query_lists(list allContacts) 
{
	std::vector<std::string> names;
	for(list::iterator it = allContacts.begin(); it !=allContacts.end(); it++)
	{
		names.push_back(it->first);
	}
	return make_pair(allContacts.size(), names);
	 
}

// print std::vector string only
void printvector(std::vector<std::string> names)
{
	for(std::vector<std::string>::iterator it = names.begin(); it !=names.end(); it++)
	{
        std::cout << *it << std::endl;
	}
}

// pretty print vector to string
std::vector<std::string> printTovector(std::vector<std::string> names)
{
	std::vector<std::string> printer;
	for(std::vector<std::string>::iterator it = names.begin(); it !=names.end(); it++)
	{
		printer.push_back(*it);
	}
	return printer;

}

// pretty print tuple to string
std::string formatVectorTuple(std::vector<contact> names)
{
    std::string to_return = "";
	for(std::vector<contact>::iterator it = names.begin(); it !=names.end(); it++)
	{
        std::string first = std::get<0>(*it);
        std::string second = std::get<1>(*it);
        std::string third = std::to_string(std::get<2>(*it));

		to_return += "(" + first + ", " + second + ", " + third + ")\n";
	}
    return to_return;
}

// format instant messaging p2p messages
std::string format_im(std::vector<contact> names)
{
    std::string to_return = "";
	for(std::vector<contact>::iterator it = names.begin(); it !=names.end(); it++)
	{
        std::string first = std::get<0>(*it);
        std::string second = std::get<1>(*it);
        std::string third = std::to_string(std::get<2>(*it));

		to_return += "-" + first + "," + second + "," + third + "\n";
	}
    return to_return;
}

// register contact-list-name
std::string registerList(list &contactList, std::string name)
{
	std::vector<contact> empty;
	for(list::iterator it = contactList.begin(); it !=contactList.end(); it++)
	{
		if(name == it->first)
		{
			return "FAILURE";
		}
	}
	contactList.push_back(make_pair(name, empty));
	return "SUCCESS";


}

// leave contact-list-name contact name

std::string leave(list& contactList, std::string contactListName, std::string hostName)
{
	
	for(list::iterator it = contactList.begin(); it !=contactList.end(); it++)
	{
		if(contactListName == it->first)
		{
			if(it->second.size() == 0)
			{
				return "FAILURE";
			}
			for(int i = 0; i < it->second.size(); i++)
			{
				if(hostName == std::get<0>(*(it->second.begin()+i)))
				{
					it->second.erase(it->second.begin()+i);
					return "SUCCESS";
				}
			}
			return "FAILURE";
		}
	}
	return "FAILURE";

}

// im contact-list-name contact -name
std::pair<int, std::vector<contact> > im(list contactList, std::string contactListName, std::string hostName)
{
	int size;
	std::vector <contact> hostContacts;
	for(list::iterator it = contactList.begin(); it !=contactList.end(); it++)
	{
		if(contactListName == it->first)
		{	
			size = it->second.size();
			for(int i =0; i < it->second.size(); i++)
			{
				if(hostName == std::get<0>(*(it->second.begin()+i)))
				{
					hostContacts.push_back(*(it->second.begin()+i));
					it->second.erase(it->second.begin()+i);
					hostContacts.insert(hostContacts.end(), it->second.begin(), it->second.end());
                    return make_pair(size, hostContacts);
				}
			}
			return make_pair(0, hostContacts);		

		}
	}

	return make_pair(0, hostContacts);


}
// join contact-list-name contact-name Ip address port
std::string join(list& contactList, std::string listName, std::string hostName, std::string ip, int port)
{
	for(list::iterator it = contactList.begin(); it !=contactList.end(); it++)
	{
		if(listName == it->first)
		{
			for(std::vector<contact>::iterator it2 = it->second.begin(); it2 !=it->second.end(); it2++)
			{
				if(hostName == std::get<0>(*it2))
				{
					//std::cout << "FAILURE" << std::endl;// FAILURE
					return "FAILURE";
				}
			}
			it->second.push_back(make_tuple(hostName,ip,port));
			//std::cout << "SUCCESS" << std::endl;
			return "SUCCESS";
		}
		
	}
	//std::cout << "FAILURE" << std::endl;
	return "FAILURE";

} 

// save contact lists
std::string saveFile(list contactList, std::string Filename)
{
    std::ofstream myFile;
	try
	{
		myFile.open(Filename+".txt");
		myFile<< "Number of contact lists: " << contactList.size() << std::endl << std::endl;
		for(list::iterator it = contactList.begin(); it !=contactList.end(); it++)
		{
			myFile<< "+" << it->first << "," << it->second.size() << std::endl;
			for(std::vector<contact>::iterator it2 = it->second.begin(); it2 !=it->second.end(); it2++)
			{
			
				myFile<< "-" << std::get<0>(*it2) << "," << std::get<1>(*it2) << "," << std::get<2>(*it2) << std::endl;
			
			}
			myFile << std::endl;
		}
	
		myFile.close();
		return "SUCCESSS";
	}

	catch(const char* e)
	{
		return "FAILURE";
	}
}

int main(int argc, char *argv[])
{
    int sock;                        /* Socket */
    struct sockaddr_in echoServAddr; /* Local address */
    struct sockaddr_in echoClntAddr; /* Client address */
    unsigned int cliAddrLen;         /* Length of incoming message */
    char echoBuffer[ECHOMAX];        /* Buffer for echo string */
    unsigned short echoServPort;     /* Server port */
    int recvMsgSize;                 /* Size of received message */

    if (argc < 2)         /* Test for correct number of parameters */
    {
        fprintf(stderr,"Usage:  %s <UDP SERVER PORT> [<INPUT FILE NAME>]\n", argv[0]);
        exit(1);
    }

    echoServPort = atoi(argv[1]);  /* First arg:  local port */

    /* Create socket for sending/receiving datagrams */
    if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
        DieWithError("socket() failed");

    /* Construct local address structure */
    memset(&echoServAddr, 0, sizeof(echoServAddr));   /* Zero out structure */
    echoServAddr.sin_family = AF_INET;                /* Internet address family */
    echoServAddr.sin_addr.s_addr = htonl(INADDR_ANY); /* Any incoming interface */
    echoServAddr.sin_port = htons(echoServPort);      /* Local port */

    /* Bind to the local address */
    if (bind(sock, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) < 0)
        DieWithError("bind() failed");

    /* Our code starts here */
    list contactList;                   /* List of contact lists */

    if (argc == 3)
    {
        std::ifstream file(argv[2]);
		std::string line;
        std::string new_list = "";
		while (std::getline(file, line))
		{
            if (line[0]=='+')
            {
                new_list = line;
                std::string comma = ",";
                new_list = new_list.substr(1, new_list.find(comma)-1);
                registerList(contactList, new_list);
            }
            if (line[0] == '-')
            {
                std::string token;
                std::vector<std::string> tuplevector;
                std::istringstream iss(line.substr(1, line.length()));
                while(getline(iss, token, ','))  /* "-" is the delimiter */
                {
                    tuplevector.push_back(token);
                }

                std::string add = join(contactList, new_list, tuplevector.at(0), tuplevector.at(1), stoi(tuplevector.at(2)));
            }
		}
    }
    std::string token;                  /* Token */
    std::string str(echoBuffer);        /* Change echoBuffer to std::string */

    for (;;) /* Run forever */
    {

    /* Set the size of the in-out parameter */
    cliAddrLen = sizeof(echoClntAddr);

    /* Block until receive message from a client */
    if ((recvMsgSize = recvfrom(sock, echoBuffer, ECHOMAX, 0,
        (struct sockaddr *) &echoClntAddr, &cliAddrLen)) < 0)
        DieWithError("recvfrom() failed");
    
    echoBuffer[recvMsgSize] = '\0';

    printf("Handling client %s\n", inet_ntoa(echoClntAddr.sin_addr));

    /* Message parsing */
    std::vector<std::string> tokenvector;
    std::cout << "Message from client: " << echoBuffer << std::endl;
    std::istringstream iss(echoBuffer);
    while(getline(iss, token, '-'))  /* "-" is the delimiter */
    {
        tokenvector.push_back(token);
    }
    int tokenInt = stoi(tokenvector.at(0),nullptr,10);

    /* Interface */
    switch(tokenInt)
    {
        case 1: /* query-lists */
            {
            std::pair<int, std::vector<std::string> > toReturn = query_lists(contactList);
            std::string boss = std::to_string(toReturn.first); 
            std::vector<std::string> bobby = printTovector(toReturn.second); 
            for(std::vector<std::string>::iterator it = bobby.begin(); it !=bobby.end(); it++)
            {
                boss.append(",");
                boss.append(*it);
            }
            const void * a = boss.c_str();
            sendto(sock, a, 800, 0,(struct sockaddr *) &echoClntAddr, sizeof(echoClntAddr));
            break;
            }
        case 2: /* register contact-list-name */
            {
            std::string boss = registerList(contactList, tokenvector.at(1));
            const void * a = boss.c_str();
            sendto(sock, a, 800, 0,(struct sockaddr *) &echoClntAddr, sizeof(echoClntAddr));
            break;
            }			
        case 3: /* join contact-list-name contact-name ip port */
            {
            std::string boss = "";
            try
            {
                int port = stoi(tokenvector.at(4),nullptr,10);
                boss = join(contactList, tokenvector.at(1), tokenvector.at(2), tokenvector.at(3), port);
            }
            catch (std::invalid_argument e)
            {
                boss = "FAILURE";
            }
            const void * a = boss.c_str();
            sendto(sock, a, 800, 0,(struct sockaddr *) &echoClntAddr, sizeof(echoClntAddr));
            break;
            }
        case 4: /* leave contact-list-name contact-name */
            {
            std::string boss = leave(contactList, tokenvector.at(1), tokenvector.at(2));
            const void * a = boss.c_str();
            sendto(sock, a, 800, 0,(struct sockaddr *) &echoClntAddr, sizeof(echoClntAddr));
            break;
            }
        case 5: /* im contact-list-name contact-name */
            {
            std::pair<int, std::vector<contact> > toReturn = im(contactList, tokenvector.at(1), tokenvector.at(2));
            std::string boss = std::to_string(toReturn.first); 
            std::string bobby = formatVectorTuple(toReturn.second); 
            bobby = boss + "\n" + bobby;
            const char * a = bobby.c_str();
            sendto(sock, a, 800, 0,(struct sockaddr *) &echoClntAddr, sizeof(echoClntAddr));
            int im = stoi(boss, nullptr, 10);
            if (im >= 2)
            {
                std::string p2pIP = std::get<1>(toReturn.second.at(0));
                const char * d = p2pIP.c_str();
                int p2pPort = std::get<2>(toReturn.second.at(0));
                unsigned short this_port = (unsigned short) p2pPort;

                /* Forward im */
                struct sockaddr_in p2pAddr; /* Echo server address */
                memset(&p2pAddr, 0, sizeof(p2pAddr));    /* Zero out structure */
                p2pAddr.sin_family = AF_INET;                 /* Internet addr family */
                p2pAddr.sin_addr.s_addr = inet_addr(d);  /* Server IP address */
                p2pAddr.sin_port   = htons(this_port);     /* Server port */

                int stringLen = strlen(a);
                std::string im_message = "0\n";
                std::vector<contact> order = toReturn.second;
                std::rotate( order.begin(), order.begin() + 1, order.end() );
                im_message += format_im(order);
                const char * z = im_message.c_str();
                sendto(sock, z, stringLen, 0,(struct sockaddr *) &p2pAddr, sizeof(p2pAddr));
                break;
            }
            }
        case 6: /* save file-name */
            {
            std::string boss = saveFile(contactList, tokenvector.at(1));
            const void * a = boss.c_str();
            sendto(sock, a, 800, 0,(struct sockaddr *) &echoClntAddr, sizeof(echoClntAddr));
            break;
            }
    }
    tokenvector.clear(); /* clear the std::vector for next message */
    memset(&echoBuffer[0], 0, sizeof(echoBuffer)); /* clear the echo buffer for next message */

    /* Send received datagram back to the client */
    if (sendto(sock, echoBuffer, recvMsgSize, 0, 
         (struct sockaddr *) &echoClntAddr, sizeof(echoClntAddr)) != recvMsgSize)
        DieWithError("sendto() sent a different number of bytes than expected");
    }

	return 0;
}
