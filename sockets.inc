/*
 *
 * AMX Mod X Module
 * Basic Socket Functions
 * 
 * Codebase from Ivan, -g-s-ivan@web.de (AMX 0.9.3)
 * Modification by Olaf Reusch, kenterfie@hlsw.de (AMXX 0.16, AMX 0.96)
 * Modification by Harsha Raghu, raghunarnindi002@gmail.com (AMXX 1.8.2)
 * 
 */

#if defined _socket_included
	#endinput
#endif
#define _socket_included

#if AMXX_VERSION_NUM >= 175
 #pragma reqlib sockets
 #if !defined AMXMODX_NOAUTOLOAD
  #pragma loadlib sockets
 #endif
#else
 #pragma library socket
#endif

// Use SOCKET_TCP for TCP Socket connections

#define SOCKET_TCP 1

// Use SOCKET_UDP for UDP Socket connections

#define SOCKET_UDP 2

/* Opens a new connection to hostname:port via protocol (either SOCKET_TCP or SOCKET_UDP),
 * returns a socket (positive) or negative or zero on error.
 * States of error:
 * 0 - no error
 * 1 - error while creating socket
 * 2 - couldn't resolve hostname
 * 3 - couldn't connect to given hostname:port 
*/

native socket_open(const _hostname[], _port, _protocol = SOCKET_TCP, &_error);

/* Closes a Socket */

native socket_close(_socket);

/* Recieves Data to string with the given length */

native socket_recv(_socket, _data[], _length);

/* Sends data to the Socket */

native socket_send(_socket, const _data[], _length);

/* Same as socket_send but Data can contain null bytes */

native socket_send2(_socket, const _data[], _length);

/* This function will return true if the state (buffer content) have changed within the last recieve or
* the timeout, where timeout is a value in µSeconds, (1 sec =1000000 µsec). 
* Use to check if new data is in your socket. */

native socket_change(_socket, _timeout=100000);

/**
 * Threaded Natives
 * 		by Shooting King
 */

// Open Socket and Listen Socket errors from iReturn parameter of CallBackHandler
enum
{
	ERR_COULD_NOT_CREATE_SOCK = -2,
	ERR_EMPTY_HOSTNAME = -3,
	ERR_DOMAIN_NOT_RESOLVED = -4,
	ERR_COULD_NOT_CONNECT = -5,
	ERR_COULD_NOT_SET_OPT = -6,	// Only from socket_listen_t()
	ERR_COULD_NOT_BIND = -7		// Only from socket_listen_t()
};
 
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

#define SOCKET_ERROR 					-1
#define SOCKET_COULDNOT_FIND_CLLBKFUNC	-2	// Could not find Public CallBack function to register it

/**
 * Spawns a new Thread.
 * returns a ThreadHandle (positive) or negative on error (Could not create a Thread).
 * The ThreadHandle is to be used for all other *_t natives.
*/
native socket_create_t();

/** 
 * Sets the passed data to thread object and puts the Thread is STATE_CONNECT which 
 * opens a new connection to hostname:port via protocol (either SOCKET_TCP or SOCKET_UDP).
 * 
 * @param iThreadHandle			A ThreadHandle returned by socket_create_t().
 * @param szHostname				A string containing ip or domain name to be connected to.
 * @param port					An integer representing the port to which the connection is to be made.
 * @param protocol				An integer specifying the protocol, 
 * 								@arg @c SOCKET_TCP for TCP.
 * 								@arg @c SOCKET_UDP for UDP.
 * @param CallBackHandler			A string containing name of a Public function to be registered as callback.
 * 
 * @fn public CallBackHandler( iThreadState, iReturn )
 * @param iThreadState				STATE_CONNECT in this case.
 * @param iReturn 				0 On Success and Negative for Error (Described above).
 * 								Call socket_get_last_error_t() to get more specific errorno from OS APIs.
 * 
 * @note ThreadState is set to STATE_IDLE before CallBack. 
 *
 * @return 						0 on Success, negative on error (SOCKET_ERROR on invalid iThreadHandle and SOCKET_COULDNOT_FIND_CLLBKFUNC).
*/
native socket_open_t( const iThreadHandle, const szHostname[], const port, const protocol, const CallBackHandler[] );

/**
 * Set the required data to Thread object, Set its state to STATE_SEND to send data via a socket .
 * 
 * @param iThreadHandle			A ThreadHandle returned by socket_create_t().
 * @param szData					A String containing the data to be sent, can contain null characters.
 * @param iDataLen				Length of the szData array.
 * @param CallBackHandler			A string containing name of a Public function to be registered as callback.
 * 
 * @fn public CallBackHandler( iThreadState, iReturn )
 * @param iThreadState				STATE_SEND in this case.
 * @param iReturn					Positive on Success (Length of the data sent) and SOCKET_ERROR on error.
 *  								Call socket_get_last_error_t() to get more specific errorno from OS APIs.
 * 
 * @note ThreadState is set to STATE_IDLE before CallBack. 
 * 
 * @return 						0 on Success, negative on error (SOCKET_ERROR on invalid iThreadHandle and SOCKET_COULDNOT_FIND_CLLBKFUNC).
*/
native socket_send_t( const iThreadHandle, const szData[], const iDataLen, const CallBackHandler[] );

/**
 * Set the required data to Thread object, Set its state to STATE_READ to recieve data from a socket.
 * 
 * @param iThreadHandle			A ThreadHandle returned by socket_create_t().
 * @param iRecvDataLen				Length of the data that must be recieved at a time.
 * @param CallBackHandler			A string containing name of a Public function to be registered as callback.
 * 
 * @fn public CallBackHandler( iThreadState, iReturn, szRecvData[], iRecvDataLen )
 * @param iThreadState				STATE_READ in this case.
 * @param iReturn 				Positive on successful read (Length of the data actually read), 0 on Connection close and
 * 								SOCKET_ERROR on error. Call socket_get_last_error_t() to get more specific errorno from OS APIs.
 * @param szRecvData[]				A null-terminated string containing the recieved data.
 * @param iRecvDataLen				The length requested by plugin to be read. Actual RecvDataLen may be less than requested. 
 * 								This is the same length passed to the native.
 * @return						If a number greater than 0 is returned by callback, iRecvDataLen is dynamically set to that number.
 *
 * @note ThreadState is set to STATE_IDLE, only when the connection is closed or on error, before CallBack. Therefore the CallBack may
 * be called more than one time passing blocks of szRecvData with length less than or equeal to iRecvDataLen.
 * 
 * @return 						0 on Success, negative on error (SOCKET_ERROR on invalid iThreadHandle and SOCKET_COULDNOT_FIND_CLLBKFUNC)
*/
native socket_recv_t( const iThreadHandle, const CallBackHandler[], const iRecvDataLen );

/**
 * Close the Socket.
 * 
 * @param iThreadHandle			A ThreadHandle returned by socket_create_t().
 * @param CallBackHandler			A string containing name of a Public function to be registered as callback.
 * 
 * @fn public CallBackHandler( iThreadState, iReturn )
 * @param iThreadState				STATE_DISCONNECT in this case.
 * @param iReturn					0 on successful close and SOCKET_ERROR on error.
 *  								Call socket_get_last_error_t() to get more specific errorno from OS APIs.
 * 
 * @note ThreadState is set to STATE_IDLE before CallBack. 
 * @note This function only closes the Socket not the thread. So that the thread can be reused for another new socket communication.
 * 
 * @return 						0 on Success, negative on error (SOCKET_ERROR on invalid iThreadHandle and SOCKET_COULDNOT_FIND_CLLBKFUNC)
 * 
*/
native socket_close_t( const iThreadHandle, const CallBackHandler[] );

/**
 * Destroyes the Thread aswell as the Socket by putting the thread is STATE_DESTROY. 
 * 
 * @param iThreadHandle			A ThreadHandle returned by socket_create_t().
 * 
 * @note After destroying no futher references should be made with iThreadHandle, Doing so may result is SEGSIGV faults.
 * The iThreadHandle must be a vaild iThreadHandle, if not, again may result in SEGSIGV faults. So you have to be more careful
 * while using this function. Best way to use this function is,
 * 
 * Example :
 * if( g_iThreadHandle )
 * 		socket_destroy_t(g_iThreadHandle);
 * 
 * @return 						0
*/
native socket_destroy_t( const iThreadHandle );

/**
 * Get last error from the OS Socket APIs.
 * 
 * @param iThreadHandle			A ThreadHandle returned by socket_create_t().
 * @return 						ErrorCode of the error.
*/
native socket_get_last_error_t( const iThreadHandle );

/** 
 * Sets the passed data to thread object and puts the Thread in STATE_LISTEN which 
 * puts the socket is listen mode for a new connection to localip:port via protocol (either SOCKET_TCP or SOCKET_UDP).
 * 
 * @param iThreadHandle			A ThreadHandle returned by socket_create_t().
 * @param szHostname				A string containing ip or domain name to be connected to.
 * @param port					An integer representing the port to which the connection is to be made.
 * @param protocol				An integer specifying the protocol, 
 * 								@arg @c SOCKET_TCP for TCP.
 * 								@arg @c SOCKET_UDP for UDP.
 * @param CallBackHandler			A string containing name of a Public function to be registered as callback.
 * 
 * @fn public CallBackHandler( iThreadState, iReturn )
 * @param iThreadState				STATE_LISTEN in this case.
 * @param iReturn 				0 On Success and Negative for Error (Described above).
 * 								Call socket_get_last_error_t() to get more specific errorno from OS APIs.
 * 
 * @note ThreadState is set to STATE_IDLE before CallBack. 
 *
 * @return 						0 on Success, negative on error (SOCKET_ERROR on invalid iThreadHandle and SOCKET_COULDNOT_FIND_CLLBKFUNC)
*/
native socket_listen_t( const iThreadHandle, const szHostname[], const port, const protocol, const CallBackHandler[] );

/**
 * Registers the CallBackHandler and puts the Thread in STATE_ACCEPT state which
 * will try to accept ONE connection. Therefore, if one needs to accept more connections, this function must again
 * be called in CallBackHandler. The maximum length of the queue of pending connections is 10. 11th client will
 * recieve 'Connection Refused' error.
 * 
 * @param iThreadHandle			A ThreadHandle returned by socket_create_t().
 * @param CallBackHandler			A string containing name of a Public function to be registered as callback.
 * 
 * @fn public CallBackHandler( iThreadState, iReturn, szClientAddr[], iClientAddrLen, clientport )
 * @param iThreadState				STATE_ACCEPT in this case.
 * @param iReturn					SocketDescriptor on Success and SOCKET_ERROR on error.
 * 								Call socket_get_last_error_t() to get more specific errorno from OS APIs.
 * @param szClientAddr				On Success, This is a string containing the ip of the new accepted client.
 * @param iClientAddrLen			On Success, This is the length of ClientAddr.
 * @param clientport				On Success, This will represent the clientport.
 * 
 * @note ThreadState is set to STATE_IDLE, after accepting on client connection, before CallBack. 
 * 
 * @return 						0 on Success, negative on error (SOCKET_ERROR on invalid iThreadHandle and SOCKET_COULDNOT_FIND_CLLBKFUNC).
*/
native socket_accept_t( const iThreadHandle, const CallBackHandler[] );

/**
 * Get SocketDescriptor of the iThreadHandle passed.
 * 
 * @param iThreadHandle			A ThreadHandle returned by socket_create_t().
 * 
 * @return 						Returns iSocketDescriptor of iThreadHandle.
*/
native socket_get_sd_t( const iThreadHandle );

/**
 * Set SocketDescriptor of the iThreadHandle passed.
 * 
 * @param iThreadHandle			A ThreadHandle returned by socket_create_t().
 * @param iSocketDescriptor			A SocketDescriptor that must be used by the iThreadHandle obj.
 * 
 * @return 						0
*/
native socket_set_sd_t( const iThreadHandle, const iSocketDescriptor );