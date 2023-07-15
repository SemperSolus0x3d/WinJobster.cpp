#include "PublicApi.h"
#include "Job.h"

WINJOBSTER_PUBLIC_API(void*, Initialize)()
{
    auto* job = new Job();
    return job;
}

WINJOBSTER_PUBLIC_API(ErrorCode, StartProcess)(
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

WINJOBSTER_PUBLIC_API(bool, IsAlive)(void* handle)
{
    auto* job = reinterpret_cast<Job*>(handle);

    return job->IsAlive();
}

WINJOBSTER_PUBLIC_API(void, Kill)(void* handle)
{
    auto* job = reinterpret_cast<Job*>(handle);

    job->Kill();
}

WINJOBSTER_PUBLIC_API(ErrorCode, Terminate)(void* handle)
{
    auto* job = reinterpret_cast<Job*>(handle);

    return job->Terminate();
}

WINJOBSTER_PUBLIC_API(void, Cleanup)(void* handle)
{
    auto* job = reinterpret_cast<Job*>(handle);

    delete job;
}

WINJOBSTER_PUBLIC_API(void, FreeMemory)(void* memory)
{
    free(memory);
}

WINJOBSTER_PUBLIC_API(ErrorCode, GetProcessIds)(void* handle, uint64_t** result, size_t* processesCount)
{
    auto* job = reinterpret_cast<Job*>(handle);

    std::vector<uint64_t> processIds;
    ErrorCode errCode = job->GetProcessIds(processIds);

    if (errCode != ErrorCode::Success)
        return errCode;

    *result = (uint64_t*)malloc(processIds.size() * sizeof(uint64_t));
    memcpy(*result, processIds.data(), processIds.size() * sizeof(uint64_t));

    *processesCount = processIds.size();

    return ErrorCode::Success;
}
