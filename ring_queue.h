#pragma once

#if defined(__GNUC__)
#define __CACHE_ALIGNED __attribute__((aligned(64)))
#define MemoryBarrier() __sync_synchronize()
#elif defined(_MSC_VER)
#include <Windows.h>
#define __CACHE_ALIGNED __declspec(align(64))
#define MemoryBarrier() __faststorefence()
#else
#error "Compiler no support"
#endif

template <typename TElem, unsigned int CAPACITY>
class CRingQueue {
public:
	CRingQueue():m_head(0), m_tail(0) {}
	~CRingQueue() {}

	bool EnQueue(const TElem &a_elem)
	{
		if (EnQueueAble()) {
			m_elemList[Mask(m_head)].e = a_elem;

			MemoryBarrier();
			m_head += 1;
			return true;
		}
		return false;
	}

	bool DeQueue(TElem &a_elem)
	{
		if (DeQueueAble()) {
			a_elem = m_elemList[Mask(m_tail)].e;

			MemoryBarrier();
			m_tail += 1;
			return true;
		}
		return false;
	}

	bool Get(TElem *a_elem)
	{
		if (DeQueueAble()) {
			a_elem = &(m_elemList[Mask(m_tail)].e);
			return true;
		}
		return false;
	}

	void Remove()
	{
		if (DeQueueAble()) {
			m_tail += 1;
		}
	}

	bool EnQueueAble()
	{
		return (unsigned int)(m_head - m_tail) < CAPACITY;
	}

	bool DeQueueAble()
	{
		return (unsigned int)(m_head - m_tail) > 0;
	}

private:
	unsigned int Mask(unsigned int a_idx)
	{
		return a_idx % CAPACITY;
	}
private:
	struct elem_t {
		TElem e;
	} __CACHE_ALIGNED;
	struct elem_t m_elemList[CAPACITY];

#if defined(__GNUC__)
	volatile unsigned int m_head __CACHE_ALIGNED;
	volatile unsigned int m_tail __CACHE_ALIGNED;
#else
	volatile unsigned int m_head;
	volatile unsigned int m_tail;
#endif
};
