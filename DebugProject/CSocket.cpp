/*
	This file is a part of Threaded Sockets Module.
    Copyright (C) 2015 Harsha Raghu (Shooting King)

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "CSocket.h"

CSocket::CSocket()
{
	// Intialize sock and error
	sock = error = -1;
}

CSocket::~CSocket()
{
	// if sock is not -1, it means that its not properly closed yet
	if(sock != -1)
		socketClose();
}

int CSocket::socketOpen( char *Hostname, unsigned int port, unsigned int protocol )
{
	if(strlen(Hostname) == 0) 
	{ 
		// just to prevent to work with a nonset hostname
        return ERR_EMPTY_HOSTNAME;
    }

    //error = 0;
    struct sockaddr_in server;
    struct hostent *host_info;
    unsigned long addr;

    int contr;

    // Create a Socket
    sock = socket(AF_INET, protocol==SOCKET_TCP?SOCK_STREAM:SOCK_DGRAM, 0);

    if (sock < 0) 
	{
        // Error, couldn't create a socket, so set an error and return.
		checkerror();
        return ERR_COULD_NOT_CREATE_SOCK;
    }
    
    // Clear the server structure (set everything to 0)
    memset( &server, 0, sizeof (server));

    // Test the hostname, and resolve if needed
    if ((addr = inet_addr(Hostname)) != INADDR_NONE) 
	{
        // seems like hostname is a numeric ip, so put it into the structure
        memcpy( (char *)&server.sin_addr, &addr, sizeof(addr));
    }
    else 
	{
        // hostname is a domain, so resolve it to an ip
        host_info = gethostbyname(Hostname);
        if (host_info == NULL) 
		{
            // an error occured, the hostname is unknown
			checkerror();
            return ERR_DOMAIN_NOT_RESOLVED;
        }
        // If not, put it in the Server structure
        memcpy( (char *)&server.sin_addr, host_info->h_addr, host_info->h_length);
    }
    // Set the type of the Socket
    server.sin_family = AF_INET;

    // Change the port to network byte order, and put it into the structure
    server.sin_port = htons(port);
    
    // Now, let's try to open a connection to the server
    contr = connect(sock, (struct sockaddr*)&server, sizeof(server));
    if (contr < 0) 
	{
            // If an error occured cancel		
			checkerror();
            return ERR_COULD_NOT_CONNECT;
    }
	return 0;
}

int CSocket::socketClose()
{
	printf( "in CSocket::sockclose, closing socket..\n" );

	if( closesocket(sock) == SOCKET_ERROR )
	{		
		checkerror();	
		return SOCKET_ERROR;
	}
	// set sock to -1, meaning the sock is closed.
	sock = -1;
    return 0;
}

int CSocket::socketSend( char *szData, int len )
{
	// send data via the socket
	int lensent;

	// try sending
	if( (lensent = send(sock, szData, len, 0)) == SOCKET_ERROR )
	{
		checkerror();
		return SOCKET_ERROR;
	}
	else
	{
		// return sent length which may be less than the actual length of the data recieved to send
		return lensent;
	}
}

int CSocket::socketRecv( char **szData, unsigned int len )
{
    int tmp = -1;
	printf( "in CSocket::socketRecv, allocating\n" );
    // First we dynamicly allocate a block of heap memory for recieving our data

	if( *szData != NULL )
	{
		delete [] *szData;
		*szData = NULL;
	}

    *szData = new char[len];
	printf( "in CSocket::socketRecv, memsetting 0\n" );
    if(*szData == NULL) return -2; // If we didn't get a block, we have to quit here to avoid sigsegv
    // And set it all to 0, because the memory could contain old trash
    memset(*szData, 0, len);
    
	// Now we recieve
	printf( "in CSocket::socketRecv, recieving \n" );
    tmp = recv(sock, *szData, len-1, 0);
	if (tmp == SOCKET_ERROR)
	{
		delete [] *szData;
		*szData = NULL;

		checkerror();
		return SOCKET_ERROR;
	}
	
	//*szData[tmp]='\0'; // this is already done by memset :)
	printf( "in CSocket::socketRecv, returning - %s[%d] - Asked - %d\n", *szData, tmp, len );
	// return the no of bytes recieved
    return tmp;
}

int CSocket::socketListen( char *Hostname, unsigned int port, unsigned int protocol)
{
	if( strlen(Hostname) == 0 ) 
	{
		// just to prevent to work with a nonset hostname
		return ERR_EMPTY_HOSTNAME;
	}

	// Create a sock
	sock = socket(AF_INET , (protocol==SOCKET_TCP?SOCK_STREAM:SOCK_DGRAM) , 0);

	if( sock < 0 ) 
	{
		// Could not create a sock
		checkerror();
		return ERR_COULD_NOT_CREATE_SOCK;
	}

	int bOptVal = TRUE;
	if( setsockopt( sock , SOL_SOCKET , SO_REUSEADDR , (char*)&bOptVal , sizeof(bOptVal) ) == SOCKET_ERROR )
	{
		checkerror();
		return ERR_COULD_NOT_SET_OPT;
	}

	struct sockaddr_in server;
	struct hostent *host_info;
	unsigned long addr;

	memset( &server, 0 , sizeof(server) );
	// Check if the hostname is addr or a domain
	if( (addr = inet_addr( Hostname )) != INADDR_NONE ) 
	{
		memcpy( (char *)&server.sin_addr , &addr , sizeof(addr) );
	}
	else 
	{
		host_info = gethostbyname( Hostname );
		if( host_info == NULL ) 
		{
			// could not resolve hostname
			checkerror();
			return ERR_DOMAIN_NOT_RESOLVED;
		}
		memcpy( (char *)&server.sin_addr , host_info->h_addr , host_info->h_length );
	}
    
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_family = AF_INET;
	server.sin_port = htons( port );

	// Try binding 
	if( (bind(sock , (struct sockaddr*)&server,sizeof(server)) == SOCKET_ERROR) )
	{
		checkerror();
		return ERR_COULD_NOT_BIND;
	}
    
	if( protocol == SOCKET_TCP )
	{
		int contr = 0;
		contr = listen( sock , 10 ); 

		if( contr == SOCKET_ERROR ) 
		{
			checkerror();
			return ERR_COULD_NOT_CONNECT;
		}
	}
	return 0;
}

int CSocket::socketAccept( char** szClientAddr, unsigned int *clientport, unsigned int *iClientAddrLen )
{
	// New connection socket
	int sockNewClient = -1;
		
	struct sockaddr_in saPeerAddr;
#ifdef _WIN32
	int iPeerLen = sizeof(saPeerAddr);
#else
	socklen_t iPeerLen = sizeof(saPeerAddr);
#endif

	// Accept the connection and get the new sock in return from accept()
	if(((sockNewClient = accept(sock , (struct sockaddr*)&saPeerAddr , &iPeerLen)) == INVALID_SOCKET) /*&& ( GetSocketError() != WOULDBLOCK )*/)
	{
		checkerror();
		return SOCKET_ERROR;
	}
	// Set client port
	*clientport = ntohs( saPeerAddr.sin_port );

	const char *tmp = (const char*)inet_ntoa( saPeerAddr.sin_addr );
	unsigned int len = strlen(tmp);

	printf( "in CSocket::socketAccept, len = %d, temp = %s, iClientAddrlen = %d, szClientAddr = %s\n",
		len, tmp, *iClientAddrLen, ((*szClientAddr!=NULL)?*szClientAddr:"NULL") );

	if( len != *iClientAddrLen || *szClientAddr )
	{
		printf( "in CSocket::socketAccept, re allocating ... \n" );
		delete [] *szClientAddr;
		*szClientAddr = new char [len+1];
		*iClientAddrLen = len;
	}
	
	// Since the addr may contaion old trash
	memset( *szClientAddr, 0, len+1 );
	// Copy client address from tmp
	memcpy( *szClientAddr, tmp, len+1 );

	/*int nlen = 0;
	while( ( nlen < addrlen ) && ( nlen < (int)max ) )
	{
		*dest++ = (cell)*tmp++;
		nlen++;
	}
	return nlen;*/

	return sockNewClient;
}

int CSocket::CheckSocket()
{
	fd_set rfds;
	struct timeval tv;

	FD_ZERO( &rfds );
	FD_SET( sock, &rfds );
	tv.tv_sec = 0;
	tv.tv_usec = 0;
	
	return (select(sock+1, &rfds, NULL, NULL, &tv) > 0);
}