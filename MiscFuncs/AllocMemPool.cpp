#include <memory>
#include "AllocMemPool.h"
#include <assert.h>

CAllocMemPool::CAllocMemPool(size_t _PoolSize) :
	m_EachPoolSize(_PoolSize)
{
	m_pFirstNode = new_Node();
}

CAllocMemPool::~CAllocMemPool()
{
	Node* pNode = m_pFirstNode;
	Node* pNextNode = nullptr;
	// sequential release
	while (pNode)
	{
		pNextNode = pNode->next;
		free(pNode);
		pNode = pNextNode;
	}
}

// Apply for node
CAllocMemPool::Node* CAllocMemPool::new_Node()
{
	Node* pointer = reinterpret_cast<Node*>(malloc(m_EachPoolSize));
	//pointer->Node::Node();
	memset(pointer, 0, m_EachPoolSize);

	return pointer;
}


void* CAllocMemPool::Alloc(size_t size, uint32_t align) {
	if (size > (m_EachPoolSize - sizeof(Node))) {
#ifdef _DEBUG
		assert(!"<AllocOnlyMPool<EachPoolSize>::Alloc>:: Alloc too big");
#endif
		return nullptr;
	}
	// Get free position
	auto* now_pos = m_pFirstNode->buffer + m_pFirstNode->allocated;
	// Get the aligned position
	auto aligned = (reinterpret_cast<size_t>(now_pos)& (align - 1));
	if (aligned) aligned = align - aligned;
	now_pos += aligned;
	// increase count
	m_pFirstNode->allocated += size + aligned;
	// Check for overflow
	if (m_pFirstNode->allocated > (m_EachPoolSize - sizeof(Node))) {
		Node* node = new_Node();
		if (!node) return nullptr;
		node->next = m_pFirstNode;
		m_pFirstNode = node;
		// Recursion (only once)
		return Alloc(size, align);
	}
	// Record the last release position
	m_pFirstNode->last_allocated = now_pos;
	return now_pos;
}

// free memory 
void CAllocMemPool::Free(void* address) {
	// This is what I applied for last time.
	if (address && m_pFirstNode->last_allocated == address) {
		m_pFirstNode->allocated =
			(m_pFirstNode->last_allocated - m_pFirstNode->buffer);
		m_pFirstNode->last_allocated = nullptr;
	}
}

