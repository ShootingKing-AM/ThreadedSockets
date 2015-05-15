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

#ifndef _INCLUDE_CTHREADEDSOCKETS_H
#define _INCLUDE_CTHREADEDSOCKETS_H

#include "CSocket.h"

// Thread includes for different OS
#include "Threads.h"

// Thread Entry fuction
FUNPREFIX ThreadMain(void* arg);

// Thread states
enum
{
	STATE_DESTROY,	
	/* The Thread will destroy itself as well as the CThreadedSocketItem obj it is associated with */
	STATE_CONNECT,	
	/* The Thread will try to connect, Thread state is set to STATE_IDLE before CallBack */
	STATE_DISCONNECT,
	/* The Thread will disconnect the socket, Thread state is set to STATE_IDLE before CallBack */
	STATE_READ,
	/* The Thread will try to read data from the socket, Thread State is set to STATE_IDLE, if there is an error or if the connection is closed */
	STATE_SEND,
	/* The Thread will send data to the socket, Thread State is set to STATE_IDLE before CallBack */
	STATE_LISTEN,
	/* The Thread will set the socket to listen mode, Thread State is set to STATE_IDLE before CallBack */
	STATE_ACCEPT,
	/* The Thread will try to accept new connections, Thread State is set to STATE_IDLE, when a client is accepted, before CallBack */
	STATE_IDLE
	/* Idle state of the thread, thread does nothing except checking for change in ThreadState */
};

// CallBack Function ID's INDEXES
enum
{
	FID_OPEN = 0,
	FID_LISTEN,
	FID_ACCEPT,
	FID_SEND,
	FID_RECV,
	FID_CLOSE
};
/**
	- CThreadedSockets -
	This class is a combination of Sockets and Threads. Setting thread states, thead operations 
	like connect, open, listen, destroy etc are defined here. This Class also stores
	required data for these operations, acts as a bridge between Mainthread an Child(spawned)
	thread and makes the communications synchronous.
	For socket operations, this class implements/uses CSocket class.
**/
class CThreadedSockets
{
	CSocket *Socket;
	THREAD thread;
	unsigned int State;

public:
	
	char *Hostname;
	unsigned int port;
	unsigned int protocol;

	char *szData;
	unsigned int iDatalen;

	char *szRecvData;
	unsigned int iRecvDatalen;

	char *szClientAddr;
	unsigned int iClientAddrLen;
	unsigned int clientport;

	// Forward ID's
	int fIDs[6];

	CThreadedSockets();
	~CThreadedSockets();
	
	void openSocket();
	void closeSocket();
	void sendData();
	void recvData();
	void listensocket();
	void accpetsocket();

	// Set thread
	inline void Thread( THREAD th ) { thread = th;  }

	// We have to wait for the thread to properly close in pthreads.
	inline THREAD Thread() { return thread;  }

	// Destroy thread forcefully
	inline void DestroyThread() { THREAD_cancel(thread); }

	// Set/Get Thread State
	inline void ThreadState( unsigned int s ) { State = s; }
	inline unsigned int ThreadState() { return State; }

	// Get last socketError from CSocket Class
	inline int getLastSocketError() { return Socket->getErrorCode(); }

	// Set/Get Socket Descriptor
	inline int getSocket() { return Socket->getSocketDescriptor(); }
	inline void setSocket( int s ) { Socket->setSocketDescriptor(s); }
};

/** 
	- CThreadedSocketItem -
	Inheriting all members from CThreadedSocket Class, This class makes CThreadedSocket as an Item
	for CThreadedSocketManager to save in its linked list.
**/
class CThreadedSocketItem : public CThreadedSockets
{
	// ID of the item
	int iID;
	CThreadedSocketItem *prev;
	CThreadedSocketItem *next;

public:
	CThreadedSocketItem() 
	{
		iID = 0; 
		prev = next = NULL;
	}
	~CThreadedSocketItem() {}
	
	// Set/Get ID
	inline void id( int i ) { iID = i; }
	inline int id() { return iID; }

	// Set/Get Previous CThreadedSocketItem
	inline void pev( CThreadedSocketItem* item ) { prev = item; }
	inline CThreadedSocketItem* pev() { return prev; }
	
	// Set/Get Next CThreadedSocketItem
	inline void nxt( CThreadedSocketItem* item ) { next = item; }
	inline CThreadedSocketItem* nxt() { return next; }
};

#endif // _INCLUDE_CTHREADEDSOCKETS_H