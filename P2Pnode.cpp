/* Socket programming project
 * Group: 29
 * Team members: Kevin Liao, Kevin Van
 */

#include <unistd.h>
#include <iostream>
#include <cstring>
#include <string>
#include <sstream>
#include <vector>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>      /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket(), connect(), sendto(), and recvfrom() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_addr() */
#include <stdlib.h>     /* for atoi() and exit() */
#include <string.h>     /* for memset() */
#include <unistd.h>     /* for close() */
#include <stdlib.h>
#define ECHOMAX 800     /* Longest string to echo */
//#define ECHOMAX 255     /* Longest string to echo */

pthread_mutex_t lock;
sem_t mutex;

void DieWithError(const char *errorMessage) /* External error handling function */
{
    perror(errorMessage);
    exit(1);
}


void *im_socket(void *port)
{
    int * local_port = (int *) port;
    unsigned short this_port = (unsigned short) *local_port;
    int sock;                        /* Socket */
    struct sockaddr_in echoServAddr; /* Local address */
    struct sockaddr_in echoClntAddr; /* Client address */
    unsigned int cliAddrLen;         /* Length of incoming message */
    char echoBuffer[ECHOMAX];        /* Buffer for echo string */
    int recvMsgSize;                 /* Size of received message */
    
    /* Create socket for sending/receiving datagrams */
    if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
        DieWithError("socket() failed");

    /* Construct local address structure */
    memset(&echoServAddr, 0, sizeof(echoServAddr));   /* Zero out structure */
    echoServAddr.sin_family = AF_INET;                /* Internet address family */
    echoServAddr.sin_addr.s_addr = htonl(INADDR_ANY); /* Any incoming interface */
    echoServAddr.sin_port = htons(this_port);      /* Local port */

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

        echoBuffer[recvMsgSize] = '\0';

        std::string str(echoBuffer);
        std::vector<std::string> lines;
        std::string line;
        std::istringstream iss(echoBuffer);
        std::string text_message;
        while (getline(iss, line, '\n'))
        {
            lines.push_back(line);
        }
        if (lines.size() == 1) /* If no more forwarding */
        {
            std::cout << "-----Receieved message: " << lines.at(0).substr(1) << "-----" << std::endl;
        }
        else if (lines.at(0)[0] == '0') /* Very first message */
        {
            pthread_mutex_lock(&lock);
            std::cout << "Please enter your message:" << std::endl;
            getline(std::cin, text_message);
            pthread_mutex_unlock(&lock);

            std::string token2;
            std::vector<std::string> tokenvector2;
            std::istringstream iss2(lines.at(1));
            while(getline(iss2, token2, ','))
            {
                tokenvector2.push_back(token2);
            }

            const char * d = tokenvector2.at(1).c_str();
            unsigned short this_port = (unsigned short) stoi(tokenvector2.at(2), nullptr, 10);

            /* Forward im */
            struct sockaddr_in p2pAddr; /* Echo server address */
            memset(&p2pAddr, 0, sizeof(p2pAddr));    /* Zero out structure */
            p2pAddr.sin_family = AF_INET;                 /* Internet addr family */
            p2pAddr.sin_addr.s_addr = inet_addr(d);  /* Server IP address */
            p2pAddr.sin_port   = htons(this_port);     /* Server port */

            std::string im_message = "1" + text_message +"\n";
            lines.erase(lines.begin(), lines.begin() + 2);
            for(std::vector<std::string>::iterator it = lines.begin(); it != lines.end(); it++)
            {
                im_message += *it +"\n";
            }
            const char * z = im_message.c_str();
            int stringLen = strlen(z);
            sendto(sock, z, stringLen, 0,(struct sockaddr *) &p2pAddr, sizeof(p2pAddr));
        }
        else /* Forward message */
        {
            std::cout << "-----Received message: " << lines.at(0).substr(1) << "-----" << std::endl;
            std::string token2;
            std::vector<std::string> tokenvector2;
            std::istringstream iss2(lines.at(1));
            while(getline(iss2, token2, ','))
            {
                tokenvector2.push_back(token2);
            }

            const char * d = tokenvector2.at(1).c_str();
            unsigned short this_port = (unsigned short) stoi(tokenvector2.at(2), nullptr, 10);

            /* Forward im */
            struct sockaddr_in p2pAddr; /* Echo server address */
            memset(&p2pAddr, 0, sizeof(p2pAddr));    /* Zero out structure */
            p2pAddr.sin_family = AF_INET;                 /* Internet addr family */
            p2pAddr.sin_addr.s_addr = inet_addr(d);  /* Server IP address */
            p2pAddr.sin_port   = htons(this_port);     /* Server port */

            std::string im_message = lines.at(0) + "\n";
            lines.erase(lines.begin(), lines.begin() + 2);
            for(std::vector<std::string>::iterator it = lines.begin(); it != lines.end(); it++)
            {
                im_message += *it +"\n";
            }
            const char * z = im_message.c_str();
            int stringLen = strlen(z);
            sendto(sock, z, stringLen, 0,(struct sockaddr *) &p2pAddr, sizeof(p2pAddr));
        }
     }
}

int main(int argc, char *argv[])
{
    /* Initialize mutex */
	if (pthread_mutex_init(&lock, NULL) != 0)
    {
        printf("\n mutex init failed\n");
		exit(1);
    }
    int local_port = atoi(argv[3]);

    /* Create new thread */
    pthread_t im;
	if(pthread_create(&im, NULL, im_socket, (void *) &local_port))
	{
		fprintf(stderr, "Error creating thread\n");
		return 1;
	}
    if (argc != 4)    /* Test for correct number of arguments */
    {
        fprintf(stderr,"Usage: %s <SERVER IP> <SERVER PORT> <LOCAL PORT>\n", argv[0]);
        exit(1);
    }

    int sock;                        /* Socket descriptor */
    struct sockaddr_in echoServAddr; /* Echo server address */
    struct sockaddr_in fromAddr;     /* Source address of echo */
    unsigned short echoServPort;     /* Echo server port */
    unsigned int fromSize;           /* In-out of address size for recvfrom() */
    char *servIP;                    /* IP address of server */
    char *echoString;                /* String to send to echo server */
    char echoBuffer[ECHOMAX+1];      /* Buffer for receiving echoed string */
    int echoStringLen;               /* Length of string to echo */
    int respStringLen;               /* Length of received response */
    std::string input;
    int input_int;

    while(1)
    {
        /* Text interface */
        printf("Please choose one of the following options (1-5):\n 1. query-lists\n 2. register \n 3. join\n 4. leave\n 5. im\n 6. save \n 7. exit\n");

        getline(std::cin, input);
        try
        {
            input_int = stoi(input);
        }
        catch(std::invalid_argument e)
        {
            std::cout << "Invalid argument." << std::endl;
            exit(1);
        }
        std::string token;
        std::vector<std::string> tokenvector;
        std::string user_input;
        std::string query;
        switch(input_int)
        {
            case 1: // query list
            {
                query = "1";
                break;
            }
            case 2: // register contact-list-name
            {
                query = "2";
                std::cout << "Please enter <contact-list-name>:";
                getline(std::cin, user_input);
                std::istringstream iss(user_input);
                while(getline(iss, token, ' '))
                {
                    tokenvector.push_back(token);
                }
                for(std::vector<std::string>::iterator it = tokenvector.begin(); it != tokenvector.end(); it++)
                {
                   query += "-" + *it;
                }
                break;
            }
            case 3: // join contact-list-name contact-name ip port
            {
                query = "3";
                std::cout << "Please enter <contact-list-name> <contact-name> <ip> <port>:";
                getline(std::cin, user_input);
                std::istringstream iss(user_input);
                while(getline(iss, token, ' '))
                {
                    tokenvector.push_back(token);
                }
                for(std::vector<std::string>::iterator it = tokenvector.begin(); it != tokenvector.end(); it++)
                {
                   query += "-" + *it;
                }
                break;
            }
            case 4: // leave contact-list-name contact-name
            {
                query = "4";
                std::cout << "Please enter <contact-list-name> <contact-name>:";
                getline(std::cin, user_input);
                std::istringstream iss(user_input);
                while(getline(iss, token, ' '))
                {
                    tokenvector.push_back(token);
                }
                for(std::vector<std::string>::iterator it = tokenvector.begin(); it != tokenvector.end(); it++)
                {
                   query += "-" + *it;
                } 
                break;
            }
            case 5: // im contact-list-name contact-name
            {
                query = "5";
                std::cout << "Please enter <contact-list-name> <contact-name>:";
                getline(std::cin, user_input);
                std::istringstream iss(user_input);
                while(getline(iss, token, ' '))
                {
                    tokenvector.push_back(token);
                }
                for(std::vector<std::string>::iterator it = tokenvector.begin(); it != tokenvector.end(); it++)
                {
                   query += "-" + *it;
                } 
 
                break;
            }
            case 6: // save file-name
            {
                query = "6";
                std::cout << "Please enter <file-name>:";
                getline(std::cin, user_input);
                std::istringstream iss(user_input);
                while(getline(iss, token, ' '))
                {
                    tokenvector.push_back(token);
                }
                for(std::vector<std::string>::iterator it = tokenvector.begin(); it != tokenvector.end(); it++)
                {
                   query += "-" + *it;
                } 
                break;
            }
            case 7:
            {
                exit(1);
            }
            default:
            {
                std::cout << "Invalid argument." << std::endl;
                exit(1);
            }
        }

		echoString = new char[query.size() + 1];
		std::copy(query.begin(), query.end(), echoString);
		echoString[query.size()] = '\0';
        

        servIP = argv[1];           /* First arg: server IP address (dotted quad) */

        if ((echoStringLen = strlen(echoString)) > ECHOMAX)
            DieWithError("Echo word too long");

        echoServPort = atoi(argv[2]);  /* Use given port, if any */

        /* Create a datagram/UDP socket */
        if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
            DieWithError("socket() failed");

        /* Construct the server address structure */
        memset(&echoServAddr, 0, sizeof(echoServAddr));    /* Zero out structure */
        echoServAddr.sin_family = AF_INET;                 /* Internet addr family */
        echoServAddr.sin_addr.s_addr = inet_addr(servIP);  /* Server IP address */
        echoServAddr.sin_port   = htons(echoServPort);     /* Server port */

        /* Send the string to the server */
        if (sendto(sock, echoString, echoStringLen, 0, (struct sockaddr *)
                   &echoServAddr, sizeof(echoServAddr)) != echoStringLen)
            DieWithError("sendto() sent a different number of bytes than expected");

        /* Give standard input to thread */
        pthread_mutex_unlock(&lock);
		usleep(100000);
        pthread_mutex_lock(&lock);


        fromSize = sizeof(fromAddr);
        respStringLen = recvfrom(sock, echoBuffer, ECHOMAX, 0, (struct sockaddr *) &fromAddr, &fromSize);   // edited to receive buffer -- Echomax set to bigger number
        echoBuffer[respStringLen] = '\0';       /* null-terminate the received data */
        printf("Received:\n%s\n\n", echoBuffer);

        if (echoServAddr.sin_addr.s_addr != fromAddr.sin_addr.s_addr)
        {
            fprintf(stderr,"Error: received a packet from unknown source.\n");
            exit(1);
        }
    }
    close(sock);
    exit(0);
}
