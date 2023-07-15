#include "WinapiHelpers.h"

ProcessIdsListPtr GetProcessIdsListPtr(size_t elementsCount, size_t& size)
{
    ProcessIdsListDeleter deleter = 
        [](JOBOBJECT_BASIC_PROCESS_ID_LIST* ptr){ delete[] (char*)ptr; };

    size_t bufferSize = sizeof(JOBOBJECT_BASIC_PROCESS_ID_LIST)
        + (elementsCount - 1) * sizeof(ULONG_PTR);

    auto bufferPtr = (JOBOBJECT_BASIC_PROCESS_ID_LIST*)new char[bufferSize];
    ZeroMemory(bufferPtr, bufferSize);

    size = bufferSize;

    return ProcessIdsListPtr(bufferPtr, deleter);
}