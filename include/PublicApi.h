#pragma once

#include "Defines.h"
#include "ErrorCode.h"

WINJOBSTER_PUBLIC_API(void*, Initialize)();

WINJOBSTER_PUBLIC_API(ErrorCode, StartProcess)(
    const wchar_t* cmdline,
    const wchar_t* workingDir,
    void* handle
);

WINJOBSTER_PUBLIC_API(bool, IsAlive)(void* handle);

WINJOBSTER_PUBLIC_API(void, Kill)(void* handle);

WINJOBSTER_PUBLIC_API(ErrorCode, Terminate)(void* handle);

WINJOBSTER_PUBLIC_API(void, Cleanup)(void* handle);

WINJOBSTER_PUBLIC_API(void, FreeMemory)(void* memory);

WINJOBSTER_PUBLIC_API(ErrorCode, GetProcessIds)(void* handle, uint64_t** result, size_t* processesCount);
