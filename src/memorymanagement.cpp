
#include <memorymanagement.h>

using namespace myos;
using namespace myos::common;


MemoryManager* MemoryManager::pActiveMemoryManager = 0;

MemoryManager::MemoryManager(size_t start, size_t size)
{
    pActiveMemoryManager = this;

    if(size < sizeof(MemoryChunk))
    {
        pFirst = 0;
    }
    else
    {
        pFirst = (MemoryChunk*)start;

        pFirst -> isAllocated = false;
        pFirst -> prev = 0;
        pFirst -> next = 0;
        pFirst -> size = size - sizeof(MemoryChunk);
    }
}

MemoryManager::~MemoryManager()
{
    if(pActiveMemoryManager == this)
        pActiveMemoryManager = 0;
}

void* MemoryManager::malloc(size_t size)
{
    MemoryChunk *pResult = 0;

    for(MemoryChunk* pChunk = pFirst; pChunk != 0 && pResult == 0; pChunk = pChunk->next)
        if(pChunk->size > size && !pChunk->isAllocated)
            pResult = pChunk;

    if(pResult == 0)
        return 0;

    if(pResult->size >= size + sizeof(MemoryChunk) + 1)
    {
        MemoryChunk* pTemp = (MemoryChunk*)((size_t)pResult + sizeof(MemoryChunk) + size);

        pTemp->isAllocated = false;
        pTemp->size = pResult->size - size - sizeof(MemoryChunk);
        pTemp->prev = pResult;
        pTemp->next = pResult->next;
        if(pTemp->next != 0)
            pTemp->next->prev = pTemp;

        pResult->size = size;
        pResult->next = pTemp;
    }

    pResult->isAllocated = true;
    return (void*)(((size_t)pResult) + sizeof(MemoryChunk));
}

void MemoryManager::free(void* ptr)
{
    MemoryChunk* pChunk = (MemoryChunk*)((size_t)ptr - sizeof(MemoryChunk));

    pChunk -> isAllocated = false;

    if(pChunk->prev != 0 && !pChunk->prev->isAllocated)
    {
        pChunk->prev->next = pChunk->next;
        pChunk->prev->size += pChunk->size + sizeof(MemoryChunk);
        if(pChunk->next != 0)
            pChunk->next->prev = pChunk->prev;

        pChunk = pChunk->prev;
    }

    if(pChunk->next != 0 && !pChunk->next->isAllocated)
    {
        pChunk->size += pChunk->next->size + sizeof(MemoryChunk);
        pChunk->next = pChunk->next->next;
        if(pChunk->next != 0)
            pChunk->next->prev = pChunk;
    }

}



void* operator new(unsigned size)
{
   if(myos::MemoryManager::pActiveMemoryManager == 0)
      return 0;
   return myos::MemoryManager::pActiveMemoryManager->malloc(size);
}

void* operator new[](unsigned size)
{
   if(myos::MemoryManager::pActiveMemoryManager == 0)
      return 0;
   return myos::MemoryManager::pActiveMemoryManager->malloc(size);
}

void* operator new(unsigned size, void* ptr)
{
   return ptr;
}

void* operator new[](unsigned size, void* ptr)
{
   return ptr;
}


void operator delete(void* ptr)
{
   if(myos::MemoryManager::pActiveMemoryManager != 0)
      myos::MemoryManager::pActiveMemoryManager->free(ptr);
}

void operator delete[](void* ptr)
{
   if(myos::MemoryManager::pActiveMemoryManager != 0)
      myos::MemoryManager::pActiveMemoryManager->free(ptr);
}


