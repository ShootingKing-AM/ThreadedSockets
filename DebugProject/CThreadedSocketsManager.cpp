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

void CThreadedSocketManager::PrintList()
{
	// This function is used to print all the list items
	CThreadedSocketItem* item;

	printf( "Start (%p) ", start );
	item = start;

	while( item != NULL )
	{
		printf( "| %p (%p) %p |", item->pev(), item, item->nxt() );
		item = item->nxt();
	}
	printf( " (%p) END\n", end );
}

CThreadedSocketManager::~CThreadedSocketManager()
{
	// Generally called on AMXX_DETACH
	CThreadedSocketItem* item;
	CThreadedSocketItem* temp;
	item = start;
	PrintList();

	if( start != NULL ) 
	{
		printf( "in CThreadedSocketManager::~CThreadedSocketManager, start item is id %d\n", start->id() );
	}

	// If the list is not empty
	while( item != NULL )
	{
		printf( "in CThreadedSocketManager::~CThreadedSocketManager, setting State Destroy to item with id %d Addr %p\n", ((item!=NULL)?(item->id()):(-1)), item );
		
		temp = item->nxt();
		item->ThreadState(STATE_DESTROY);
#ifdef _WIN32
		WaitForSingleObject(item->Thread(), INFINITE);
#else
		pthread_join(item->Thread(), 0);
#endif
		// traverse to the next item in the list
		item = temp;
		printf( "in CThreadedSocketManager::~CThreadedSocketManager, traversing to next item with id %d ADDr %p\n", ((item!=NULL)?(item->id()):(-1)), item );
	}
}

/*FUNPREFIX ThreadMonitor( void* arg )
{
	// This Thread is used to monitor, a change in address value, i had used it to monitor
	// pev of an item to debug xD
	CThreadedSocketItem* item = (CThreadedSocketItem *)arg;
	CThreadedSocketItem* temp = (CThreadedSocketItem *)-1;

	while( item != NULL )
	{
		if( item->pev() != temp )
		{
			printf( "\n\n** Thread Monitor ** Sumthing Fishy, item-> pev changed to %p\n\n", item->pev() );
			temp = item->pev();
		}
	}
	THREAD_exit(0);
}*/

void CThreadedSocketManager::AddItemToList( CThreadedSocketItem* item/*, bool Create*/ )
{
	// Add item to the list
	// Set count of an item
	item->id(count);
	//THREAD th1;
	//th1 = THREAD_create(ThreadMonitor, item);

	// LIST Skeleton : start=item1 item2 itme3 ........ item(n-1) .. itemn=end
	printf( "in CThreadedSocketManager::AddItemToList, before \n" );
	PrintList();
	printf( "in CThreadedSocketManager::AddItemToList, Adding item to list with id %d Addr %p\n", count, item );
	if( start == NULL )
	{
		// So this is our first item
		start = item;
	}
	printf( "item-Pev = %p end : %p\n", item->pev(), end );
	item->pev(end);
	printf( "item-Pev = %p end : %p\n", item->pev(), end );
	if( item->pev() != NULL )
	{
		item->pev()->nxt(item);
	}

	end = item;
	count++;	
	printf( "in CThreadedSocketManager::AddItemToList, after \n" );
	PrintList();
}

void CThreadedSocketManager::RemoveItemFromList( CThreadedSocketItem* item )
{
	printf( "in CThreadedSocketManager::RemoveItemFromList, removing item from list with id %d with addr %p\n", (item==NULL)?-1:item->id(), item  );
	// Remove certain item from list
	PrintList();

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
	PrintList();
}


	


