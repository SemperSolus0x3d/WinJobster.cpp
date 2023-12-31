﻿#pragma once
#include <string>
#include <vector>
#define WINNT
#include <sdkddkver.h>
#include <windows.h>
#include "ErrorCode.h"

class Job
{
public:
    Job();
    ~Job();

    ErrorCode StartProcess(
        const std::wstring& cmdline,
        const std::wstring& currentWorkingDir,
        uint32_t processInitTimeoutMs
    );

    bool IsAlive();
    void Kill();
    ErrorCode Terminate(uint32_t timeoutMs);

    ErrorCode Job::GetProcessIds(std::vector<uint64_t>& result);
private:
    HANDLE m_Job = INVALID_HANDLE_VALUE;
    HANDLE m_CompletionPort = INVALID_HANDLE_VALUE;
    bool m_IsAlive = false;

    ErrorCode InitializeJob();
    ErrorCode ReinitializeJob();
};
