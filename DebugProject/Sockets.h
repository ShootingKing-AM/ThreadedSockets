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

#ifndef _INCLUDE_SOCKETSSTD_H
#define _INCLUDE_SOCKETSSTD_H

#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>

#ifdef _WIN32

	/* Windows */
	#include <winsock.h>
	#include <io.h>
	#define socklen_t		int
	#define SOCKET_LAST_ERR	WSAGetLastError()

#else

	/* Unix/Linux */
	#include <unistd.h>
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <netdb.h>
	#include <arpa/inet.h>
	#include <errno.h>

	#ifndef SOCKET_ERROR
		#define SOCKET_ERROR		-1
	#endif
	#ifndef SOCKET
		#define SOCKET			int
	#endif
	#ifndef	TRUE
		#define TRUE			true
	#endif
	#define INVALID_SOCKET			-1

	#define closesocket(s)		close(s)
	#define SOCKET_LAST_ERR		errno

#endif

#endif // _INCLUDE_SOCKETSSTD_H