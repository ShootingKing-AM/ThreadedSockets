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

#ifndef _INCLUDE_CSOCKET_H
#define _INCLUDE_CSOCKET_H

// Standard and Socket Include files
#include "Sockets.h"
#include "amxxmodule.h"

#define SOCKET_TCP 1
#define SOCKET_UDP 2

// OpenSocket and ListenSocket Function return errors
enum
{
	ERR_COULD_NOT_CREATE_SOCK = -2,
	ERR_EMPTY_HOSTNAME = -3,
	ERR_DOMAIN_NOT_RESOLVED = -4,
	ERR_COULD_NOT_CONNECT = -5,
	ERR_COULD_NOT_SET_OPT = -6,
	ERR_COULD_NOT_BIND = -7
};

/**
	- CSocket -
	This class is responsible for all low level socket operations.
**/
class CSocket
{
	SOCKET sock;
	int error;
	
public:

	CSocket();
	~CSocket();

	int socketOpen( char *Hostname, unsigned int port, unsigned int protocol );
	int socketClose();
	int socketSend( char *szData, int len );
	int socketRecv( char **szData, unsigned int len );
	int socketListen( char *Hostname, unsigned int port, unsigned int protocol = SOCKET_TCP);
	int socketAccept( char **szClientAddr, unsigned int *clientport, unsigned int *iClientAddrLen );

	int CheckSocket();	
	// CheckSocket() is function is used to detect any data that is ready to be read or any new clients are waiting to accept their connection on the socket

	inline void checkerror()	{ error = SOCKET_LAST_ERR; }
	inline int getErrorCode()	{ return error; }

	inline int getSocketDescriptor() { return sock; }
	inline void setSocketDescriptor( int s ) { sock = s; }
};

#endif //_INCLUDE_CSOCKET_H