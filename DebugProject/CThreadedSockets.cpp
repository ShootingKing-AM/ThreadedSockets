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
#include "CThreadedSockets.h"

CThreadedSockets::CThreadedSockets()
{
	//Socket = NULL;	
	// Create a CSocket object
	Socket = new CSocket;

	// Set defualt state of the Thread to STATE_IDLE
	State = STATE_IDLE;

	// Initializing all shit to 0 and Null xD
	iDatalen = iRecvDatalen = port = protocol = iClientAddrLen = clientport = 0;
	szData = szRecvData = Hostname = szClientAddr = NULL;

	// Initializing  all fwdID to -1, because, 0 is also a valid fwdID
	for( int i = 0; i < 4; i++ )
		fIDs[i] = -1;
}

CThreadedSockets::~CThreadedSockets()
{
	//if(Socket != NULL)
	//{
		printf( "SOcket deletion -- %p\n", Socket );
		delete Socket;
		Socket = NULL;
	//}

	for( int i = 0; i < 4; i++ )
	{
		if (fIDs[i] > -1)
		{
			// if there is a registered forward, unregister it
			MF_UnregisterSPForward(fIDs[i]);
			fIDs[i] = -1;
		}
	}

	// Delete all shit xD
	delete [] szData;
	delete [] szRecvData;
	delete [] Hostname;
	delete [] szClientAddr;
	szData = szRecvData = Hostname = szClientAddr = NULL;
}

void CThreadedSockets::openSocket()
{
	printf( "SOcket initialisation -- %p\n", Socket );
	// Call CSocket function and return the result in CallBack parameter
	int ret = Socket->socketOpen(Hostname, port, protocol);
	printf( "in CThreSocks::openSocket, ExecuteForward returned %d\n", MF_ExecuteForward(fIDs[FID_OPEN], (cell)STATE_CONNECT, (cell)ret));
}

void CThreadedSockets::closeSocket()
{
	// Call CSocket function and return the result in CallBack parameter
	int ret = Socket->socketClose();
	MF_ExecuteForward(fIDs[FID_CLOSE], (cell)STATE_DISCONNECT, (cell)ret);
}

void CThreadedSockets::sendData()
{
	// Call CSocket function and return the result in CallBack parameter
	int ret = Socket->socketSend(szData, iDatalen);
	printf( "in CThreSocks::sendData, ExecuteForward returned %d\n", MF_ExecuteForward(fIDs[FID_SEND], (cell)STATE_SEND, (cell)ret));
}

void CThreadedSockets::recvData()
{
	// Call CSocket function and return the result in CallBack parameter

	// Check if some data is waiting for us to be read
	if( Socket->CheckSocket() )
	{
		printf( "in CThreSocks::recvData, calling CSocket function\n" );
		int ret = Socket->socketRecv(&szRecvData, iRecvDatalen);	
		printf( "in CThreSocks::recvData, szRecvData - %s[%d]\n", szRecvData, iRecvDatalen );
		printf( "in CThreSocks::recvData, calling callback function\n" );
		printf( "in CThreSocks::recvData, setting state to idle\n" );
		// Set STATE_IDLE only if the connection is closed or error or someone else didnot change to otherstate 
		if((ret <= 0) && (State == STATE_READ))
			State = STATE_IDLE;
		
		// Therefore, the call back is triggered until the Data is completely extracted from the socket
		int retVal = MF_ExecuteForward(fIDs[FID_RECV], (cell)STATE_READ, (cell)ret, szRecvData, (cell)iRecvDatalen);

		// Now for dynamically changing buffer length...
		if( retVal > 0 )
			iRecvDatalen = retVal;
		
		printf( "in CThreSocks::recvData, ExecuteForward returned %d with fid %d\n", retVal, fIDs[FID_RECV]);
	}	
}

void CThreadedSockets::listensocket()
{
	printf( "in CThreSocks::listensocket, calling CSocket function\n" );
	// Call CSocket function and return the result in CallBack parameter
	int ret = Socket->socketListen(Hostname, port, protocol);
	MF_ExecuteForward(fIDs[FID_LISTEN], (cell)STATE_LISTEN, (cell)ret);
}

void CThreadedSockets::accpetsocket()
{
	// Call CSocket function and return the result in CallBack parameter
		
	// Check if some new client is waiting for us to be accepted
	if( Socket->CheckSocket() )
	{		
		printf( "in CThreSocks::accpetsocket, calling CSocket function\n" );
		// Check if the state is still STATE_ACCEPT
		if( State == STATE_ACCEPT )
			State = STATE_IDLE;

		int ret = Socket->socketAccept( &szClientAddr, &clientport, &iClientAddrLen );
		MF_ExecuteForward(fIDs[FID_ACCEPT], (cell)STATE_ACCEPT, (cell)ret, szClientAddr, (cell)iClientAddrLen, (cell)clientport);
	}
}

FUNPREFIX ThreadMain(void* arg)
{
	// Get the CThreadedSocketItem obj from argument passed
	CThreadedSocketItem* threaditem = (CThreadedSocketItem *)arg;
	printf( "In Threadmain, arg - %d\n", (cell)threaditem);

	// Get and cache the ThreadState
	unsigned int state = threaditem->ThreadState();
	printf( "In ThreadMain, state is %d\n", state );

	// Loop until STATE is not STATE_DESTROY
	while( state != STATE_DESTROY )
	{
		switch (state)
		{
			case STATE_CONNECT:
			{
				// The Thread will try to connect, Thread state is set to STATE_IDLE before CallBack
				printf( "In ThreadMain - Conn, state is STATE_CONNECT opening socket\n" );
				threaditem->ThreadState(STATE_IDLE);
				printf( "In ThreadMain - Conn, hostname = %s\n", threaditem->Hostname );
				threaditem->openSocket();

				printf( "In ThreadMain - Conn, state is set to STATE_IDLE\n" );
				break;
			}
			case STATE_DISCONNECT:
			{
				// The Thread will disconnect the socket, Thread state is set to STATE_IDLE before CallBack
				threaditem->ThreadState(STATE_IDLE);
				printf( "In ThreadMain - disc, state is STATE_DISCONNECT closing socket\n" );
				threaditem->closeSocket();
				printf( "In ThreadMain - disc, state is set to STATE_IDLE\n" );
				break;
			}
			case STATE_READ:
			{
				// The Thread will try to read data from the socket, Thread State is set to STATE_IDLE, if there is an error or if the connection is closed
				// printf( "In ThreadMain - read, state is STATE_READ recving socket\n" );
				threaditem->recvData();
				// printf( "In ThreadMain - read, state is now set to %d\n", threaditem->ThreadState() );
				break;
			}
			case STATE_SEND:
			{
				// The Thread will send data to the socket, Thread State is set to STATE_IDLE before CallBack
				threaditem->ThreadState(STATE_IDLE);
				printf( "In ThreadMain - send, state is STATE_SEND sending socket %s[%d]\n", threaditem->szData, threaditem->iDatalen );
				threaditem->sendData();
				printf( "In ThreadMain - send, state is set to STATE_IDLE\n" );
				break;
			}
			case STATE_LISTEN:
			{
				// The Thread will set the socket to listen mode, Thread State is set to STATE_IDLE before CallBack
				threaditem->ThreadState(STATE_IDLE);
				printf( "In ThreadMain - listen, state is STATE_LISTEN\n");
				threaditem->listensocket();
				printf( "In ThreadMain - listen, state is set to STATE_IDLE\n" );
				break;
			}
			case STATE_ACCEPT:
			{
				// The Thread will try to accept new connections, Thread State is set to STATE_IDLE, when a client is accepted, before CallBack 
				// printf( "In ThreadMain - accept, state is STATE_ACCEPT\n" );
				threaditem->accpetsocket();
				// printf( "In ThreadMain - accept, state is STATE_IDLE\n" );
				break;
			}
			default: {}
		}
		// Get state from ThreadItem again
		state = threaditem->ThreadState();
	}
	
	// We have reached here meaning that the State is STATE_DESTROY
	// Delete threaditem and kill the dangling pointer xD
	delete threaditem;
	threaditem = NULL;
	printf( "in ThreadMain, Exiting Thread ... Thanksyou ;)\n" );
	// Exit with status 0
	THREAD_exit(0);
}
