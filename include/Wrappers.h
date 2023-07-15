#pragma once

#include "Defines.h"
#include "ErrorCode.h"

WINJOBSTER_WRAPPER(void*, Initialize)();

WINJOBSTER_WRAPPER(ErrorCode, StartProcess)(
    const wchar_t* cmdline,
    const wchar_t* workingDir,
    void* handle
);

WINJOBSTER_WRAPPER(bool, IsAlive)(void* handle);

WINJOBSTER_WRAPPER(void, Kill)(void* handle);

WINJOBSTER_WRAPPER(void, Cleanup)(void* handle);

WINJOBSTER_WRAPPER(void, FreeMemory)(void* memory);

WINJOBSTER_WRAPPER(ErrorCode, GetProcessIds)(void* handle, uint64_t** result, size_t* processesCount);
