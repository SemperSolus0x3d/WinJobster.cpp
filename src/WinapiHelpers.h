#pragma once
#include <memory>
#include <functional>
#include <Windows.h>

using ProcessIdsListDeleter = std::function<void(JOBOBJECT_BASIC_PROCESS_ID_LIST*)>;

using ProcessIdsListPtr = 
    std::unique_ptr<JOBOBJECT_BASIC_PROCESS_ID_LIST, ProcessIdsListDeleter>;

ProcessIdsListPtr GetProcessIdsListPtr(size_t elementsCount, size_t& size);
