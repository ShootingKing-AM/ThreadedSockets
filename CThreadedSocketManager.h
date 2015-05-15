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

#ifndef _INCLUDE_CTHREADSOCKETMANAGER_H
#define _INCLUDE_CTHREADSOCKETMANAGER_H

#include "CThreadedSockets.h"

/** 
	- CThreadedSocketManager -
	This Class maintains a double linked list of all the spawned Threads. If the spawned 
	theads are not properly destroyed, this Class forcefully destroys the threads at
	AMXX_DETACH.
**/
class CThreadedSocketManager
{
	int count;
	CThreadedSocketItem *start;
	CThreadedSocketItem *end;
	
public:
	CThreadedSocketManager();
	~CThreadedSocketManager();

	void PrintList();
	void AddItemToList( CThreadedSocketItem* item );
	void RemoveItemFromList( CThreadedSocketItem* item );
};

#endif // _INCLUDE_CTHREADSOCKETMANAGER_H