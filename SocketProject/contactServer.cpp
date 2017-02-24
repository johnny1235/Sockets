#include <vector>
#include <utility>      // std::pair
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

using namespace std;
typedef tuple<string,string,int> contact;
typedef vector< pair< string,vector<contact> > > list;

// Functions


void DieWithError(const char *errorMessage) /* External error handling function */
{
    perror(errorMessage);
    exit(1);
}

//query lists
pair<int, vector<string> > query_lists(list allContacts) 
{
	vector<string> names;
	for(list::iterator it = allContacts.begin(); it !=allContacts.end(); it++)
	{
		names.push_back(it->first);
	}
	return make_pair(allContacts.size(), names);
	 
}

// print vector string only
void printVector(vector<string> names)
{
	for(vector<string>::iterator it = names.begin(); it !=names.end(); it++)
	{
		cout << *it << endl;
	}
}

void printVectorTuple(vector<contact> names)
{
	for(vector<contact>::iterator it = names.begin(); it !=names.end(); it++)
	{
		cout << "(" << get<0>(*it) << ", " << get<1>(*it) << ", " << get<2>(*it) << ")" << endl;
	}
}
// register contact-list-name
int registerContactListName(list& contactList, string name)
{
	vector<contact> empty;
	for(list::iterator it = contactList.begin(); it !=contactList.end(); it++)
	{
		if(name == it->first)
		{
			cout << "FAILURE" << endl;
			return 0;
		}
	}
	contactList.push_back(make_pair(name, empty));
	cout << "SUCCESS"<< endl;
	return 1;


}

// leave contact-list-name contact name

int leave(list& contactList, string contactListName, string hostName)
{
	
	for(list::iterator it = contactList.begin(); it !=contactList.end(); it++)
	{
		if(contactListName == it->first)
		{
			if(it->second.size() == 0)
			{
				cout << "Failure" << endl;
				return 0;
			}
			for(int i =0; i <= it->second.size(); i++)
			{
				if(hostName == get<0>(*(it->second.begin()+i)))
				{
					it->second.erase(it->second.begin()+i);
					cout << "SUCCESS" << endl;
					return 1;
				}
			}
			cout << "Failure" << endl;
			return 0;
		}
	}
	cout << "Failure" << endl;
	return 0;

}

// im contact-list-name contact -name
pair<int, vector<contact> > im(list contactList, string contactListName, string hostName)
{
	int size;
	vector <contact> hostContacts;
	for(list::iterator it = contactList.begin(); it !=contactList.end(); it++)
	{
		if(contactListName == it->first)
		{	
			size = it->second.size();
			for(int i =0; i <= it->second.size(); i++)
			{
				if(hostName == get<0>(*(it->second.begin()+i)))
				{
					hostContacts.push_back(*(it->second.begin()+i));
					it->second.erase(it->second.begin()+i);
					hostContacts.insert(hostContacts.end(), it->second.begin(), it->second.end());
					break;
				}
			}
			return make_pair(0, hostContacts);		

		}
	}

	return make_pair(size, hostContacts);


}
// join contact-list-name contact-name Ip address port
int join(list& contactList, string listName, string hostName, string ip, int port)
{
	for(list::iterator it = contactList.begin(); it !=contactList.end(); it++)
	{
		if(listName == it->first)
		{
			for(vector<contact>::iterator it2 = it->second.begin(); it2 !=it->second.end(); it2++)
			{
				if(hostName == get<0>(*it2))
				{
					cout << "FAILURE" << endl;// FAILURE
					return 0;
				}
			}
			it->second.push_back(make_tuple(hostName,ip,port));
			cout << "SUCCESS" << endl;
			return 1;
		}
		
	}
	cout << "FAILURE" << endl;
	return 0;

} 

int saveFile(list contactList, string Filename)
{
	ofstream myFile;
	try
	{
		myFile.open(Filename+".txt");
		myFile<< "Number of contact lists: " << contactList.size() << endl << endl;
		for(list::iterator it = contactList.begin(); it !=contactList.end(); it++)
		{
			myFile<< "Contact list: " << it->first << " | Number of contacts: " << it->second.size() << endl;
			for(vector<contact>::iterator it2 = it->second.begin(); it2 !=it->second.end(); it2++)
			{
			
				myFile<< "(" << get<0>(*it2) << ", " << get<1>(*it2) << ", " << get<2>(*it2) << ")" << endl;
			
			}
			myFile << endl;
		}
	
		myFile.close();
		return 1;
	}

	catch(const char* e)
	{
		return 0;
	}
}
int main(int argc, char *argv[])
{
	    list contactList;
	    // Test Data
	    vector<contact> bobs;
	    contactList.push_back(make_pair("bob", bobs));
	    contactList.push_back(make_pair("alice", bobs));
	
	    int sock;                        /* Socket */
	    struct sockaddr_in echoServAddr; /* Local address */
	    struct sockaddr_in echoClntAddr; /* Client address */
	    unsigned int cliAddrLen;         /* Length of incoming message */
	    //string echoBuffer;
	    char echoBuffer[ECHOMAX];        /* Buffer for echo string */
	    unsigned short echoServPort;     /* Server port */
	    int recvMsgSize;                 /* Size of received message */
	    string token;
	    string str(echoBuffer);  // splitting

	    if (argc != 2)         /* Test for correct number of parameters */
	    {
		fprintf(stderr,"Usage:  %s <UDP SERVER PORT>\n", argv[0]);
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
	  
	    for (;;) /* Run forever */
	    {
		/* Set the size of the in-out parameter */
		cliAddrLen = sizeof(echoClntAddr);

		/* Block until receive message from a client */
		if ((recvMsgSize = recvfrom(sock, echoBuffer, ECHOMAX, 0,
		    (struct sockaddr *) &echoClntAddr, &cliAddrLen)) < 0)
		    DieWithError("recvfrom() failed");

		printf("Handling client %s\n", inet_ntoa(echoClntAddr.sin_addr));

		// parsing the message from client
		vector<string> tokenVector;
		cout << echoBuffer << endl;
		istringstream iss(echoBuffer);
		while(getline(iss, token, '-'))
		{
			tokenVector.push_back(token);
		}
		
		// printVector(tokenVector); Test
		int tokenInt = stoi(tokenVector.at(0),nullptr,10);
		switch(tokenInt)
		{
			case 1: // query list
				{
				pair<int, vector<string> > toReturn = query_lists(contactList);
				//cout << toReturn.first << endl;
				//printVector(toReturn.second);
				break;
				}
			case 2: // register contact-list-name
			
				break;
	 		case 3: // join contact-list-name contact-name ip port
				break;
			case 4: // leave contact-list-name contact-name
				break;
			case 5: // im contact-list-name contact-name
				break;
			case 6: // save file-name
				break;
		}
/// 		Test to Send Hello world to Client
		boss = "Hello World";
		const void * a = boss.c_str();
		sendto(sock, a, 255, 0, 
		     (struct sockaddr *) &echoClntAddr, sizeof(echoClntAddr));
///             END TEST
		sendto(sock, echoBuffer, recvMsgSize, 0, 
		     (struct sockaddr *) &echoClntAddr, sizeof(echoClntAddr));
/*
		/* Send received datagram back to the client */
		if (sendto(sock, echoBuffer, recvMsgSize, 0, 
		     (struct sockaddr *) &echoClntAddr, sizeof(echoClntAddr)) != recvMsgSize)
		    DieWithError("sendto() sent a different number of bytes than expected");
	    }

	//vector<contact> bobs;

/*

	// creating contact lists
	contactList.push_back(make_pair("bob", bobs));
	contactList.push_back(make_pair("alice", bobs));
	
	// create pair to Return Query List
	pair<int, vector<string> > toReturn = query_lists(contactList);
	
	cout << toReturn.first << endl;
	printVector(toReturn.second);

	//register contact-list-name 
	registerContactListName(contactList,"newTest");
	registerContactListName(contactList,"bob");

	// testing join function
	join(contactList, "bob", "kevin", "10", 15);
	join(contactList, "bob", "k", "101", 115);
	join(contactList, "alice", "evin", "109", 1445);
	join(contactList, "newTest", "vin", "180", 1885);
	

	auto toReturn2 = im(contactList, "bob", "john");
	cout << toReturn2.first << endl;
	printVectorTuple(toReturn2.second);
	//cout << 'Hello World!\n';

	saveFile(contactList, "test");
*/
	return 0;
}
