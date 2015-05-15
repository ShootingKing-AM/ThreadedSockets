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

#ifndef _INCLUDE_THREADS_H
#define _INCLUDE_THREADS_H

#if defined __linux__ || defined __APPLE__

	/** POSIX Threads for Linux and MacOS systems **/
	#include <pthread.h>

	#define THREAD					pthread_t
	#define THREAD_create(t,f,a)	pthread_create(t,NULL,f,a)

	#define THREAD_exit(s)			pthread_exit(s)
	#define THREAD_cancel(t)		pthread_cancel(t)

	#define FUNPREFIX				void*

#elif defined WIN32
	
	/** WinThreads **/
	#include <windows.h>
	#include <tchar.h>
	#include <strsafe.h>

	#define THREAD					HANDLE
	#define THREAD_create(f,a)		CreateThread( NULL, 0, f, a, 0, NULL )

	#define THREAD_exit(s)			ExitThread(s)
	#define THREAD_cancel(t)		TerminateThread(t,0)

	#define FUNPREFIX				DWORD WINAPI

#endif

#endif // _INCLUDE_THREADS_H