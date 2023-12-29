#pragma once

/*
source code from:
https://blog.csdn.net/dustpg/article/details/38756241
*/
#pragma warning ( disable : 4200 )
#include <stdint.h>

class CAllocMemPool
{
	// node
	struct Node {
		// posterior node
		Node*               next = nullptr;
		// Allocated quantity
		size_t              allocated = 0;
		// Last assigned position
		uint8_t*               last_allocated = nullptr;
		// buffer
		uint8_t                buffer[0];
	};

public:
	CAllocMemPool(size_t _PoolSize);
	~CAllocMemPool();

	// Apply for memory
	void*               Alloc(size_t size, uint32_t align = sizeof(size_t));
	// free memory
	void                Free(void* address);

private:

	// Apply for node
	__forceinline Node* new_Node();
	// first node
	Node*	m_pFirstNode;

	size_t	m_EachPoolSize;
};

