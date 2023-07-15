#pragma once
#include <string>
#include <vector>
#include <windows.h>
#include "ErrorCode.h"

class Job
{
public:
    Job();
    ~Job();

    ErrorCode StartProcess(
        const std::wstring& cmdline,
        const std::wstring& currentWorkingDir
    );

    bool IsAlive();
    void Kill();
    void Terminate();

    ErrorCode Job::GetProcessIds(std::vector<ULONG_PTR>& result);
private:
    HANDLE m_Job = INVALID_HANDLE_VALUE;
    HANDLE m_CompletionPort = INVALID_HANDLE_VALUE;
    bool m_IsAlive = false;

    ErrorCode InitializeJob();

};
