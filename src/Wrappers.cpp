#include "Wrappers.h"
#include "Job.h"

WINJOBSTER_WRAPPER(void*, Initialize)()
{
    auto* job = new Job();
    return job;
}

WINJOBSTER_WRAPPER(ErrorCode, StartProcess)(
    const wchar_t* cmdline,
    const wchar_t* workingDir,
    void* handle
)
{
    auto* job = reinterpret_cast<Job*>(handle);

    std::wstring workingDirStr;

    if (workingDir != NULL)
        workingDirStr = workingDir;

    auto errCode = job->StartProcess(cmdline, workingDirStr);

    return errCode;
}

WINJOBSTER_WRAPPER(bool, IsAlive)(void* handle)
{
    auto* job = reinterpret_cast<Job*>(handle);

    return job->IsAlive();
}

WINJOBSTER_WRAPPER(void, Kill)(void* handle)
{
    auto* job = reinterpret_cast<Job*>(handle);

    job->Kill();
}

WINJOBSTER_WRAPPER(void, Cleanup)(void* handle)
{
    auto* job = reinterpret_cast<Job*>(handle);

    delete job;
}

WINJOBSTER_WRAPPER(void, FreeMemory)(void* memory)
{
    delete memory;
}

WINJOBSTER_WRAPPER(ErrorCode, GetProcessIds)(void* handle, uint64_t** result, size_t* processesCount)
{
    static_assert(sizeof(ULONG_PTR) == sizeof(uint64_t), "");

    auto* job = reinterpret_cast<Job*>(handle);

    std::vector<ULONG_PTR> processIds;
    ErrorCode errCode = job->GetProcessIds(processIds);

    if (errCode != ErrorCode::Success)
        return errCode;

    *result = new uint64_t[processIds.size()];
    memcpy(*result, processIds.data(), processIds.size() * sizeof(uint64_t));

    *processesCount = processIds.size();

    return ErrorCode::Success;
}
