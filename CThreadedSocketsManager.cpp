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

#include "CThreadedSocketManager.h"

CThreadedSocketManager::CThreadedSocketManager()
{
	// Initializing count 0 and pointers to NULL 
	count = 0;
	start = end = NULL;
}

CThreadedSocketManager::~CThreadedSocketManager()
{
	// Generally called on AMXX_DETACH
	CThreadedSocketItem* item;
	CThreadedSocketItem* temp;
	item = start;
	
	// If the list is not empty
	while( item != NULL )
	{
		temp = item->nxt();
		item->ThreadState(STATE_DESTROY);
#ifdef _WIN32
		WaitForSingleObject(item->Thread(), INFINITE);
#else
		pthread_join(item->Thread(), 0);
#endif
		// traverse to the next item in the list
		item = temp;
	}
}

void CThreadedSocketManager::AddItemToList( CThreadedSocketItem* item/*, bool Create*/ )
{
	// Add item to the list
	// Set count of an item
	item->id(count);

	// LIST Skeleton : start=item1 item2 itme3 ........ item(n-1) .. itemn=end
	
	if( start == NULL )
	{
		// So this is our first item
		start = item;
	}
	
	item->pev(end);
	
	if( item->pev() != NULL )
	{
		item->pev()->nxt(item);
	}

	end = item;
	count++;
}

void CThreadedSocketManager::RemoveItemFromList( CThreadedSocketItem* item )
{
	// Remove certain item from list

	if( item->pev() != NULL )
	{
		item->pev()->nxt(item->nxt());
	}
	else
	{
		start = item->nxt();
	}
		
	if( item->nxt() != NULL )
	{
		item->nxt()->pev(item->pev());
	}
	else
	{
		end = item->pev();
	}
}


	


