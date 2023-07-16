#include <set>
#include "Job.h"
#include "Utils.h"
#include "Journal.h"
#include "WinapiHelpers.h"
#include <cassert>

const ULONG_PTR CompletionKey = 52534;

BOOL CALLBACK EnumChildWindowsCallback(
    _In_ HWND   hwnd,
    _In_ LPARAM lParam);

BOOL CALLBACK EnumWindowsCallback(
    _In_ HWND   hwnd,
    _In_ LPARAM lParam);

BOOL TerminateProcessIfItIsInTheJob(HWND hwnd, LPARAM lParam);


Job::Job()
{
}

Job::~Job()
{
    Kill();
    SafeCloseHandle(m_CompletionPort);
    SafeCloseHandle(m_Job);
}

ErrorCode Job::InitializeJob()
{
    Journal journal;

    m_Job = CreateJobObjectW(NULL, NULL);

    if (m_Job == INVALID_HANDLE_VALUE)
        return ErrorCode::JobObjectCreationFailed;

    journal += [&](){ SafeCloseHandle(m_Job); };

    m_CompletionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);

    if (m_CompletionPort == NULL)
        return ErrorCode::CompletionPortCreationFailed;

    journal += [&](){ SafeCloseHandle(m_CompletionPort); };

    JOBOBJECT_ASSOCIATE_COMPLETION_PORT info;
    info.CompletionKey = (PVOID)CompletionKey;
    info.CompletionPort = m_CompletionPort;

    if (!SetInformationJobObject(
        m_Job,
        JobObjectAssociateCompletionPortInformation,
        &info,
        sizeof(info))
    )
        return ErrorCode::FailedToAssociateJobWithCompletionPort;

    journal.Commit();
    return ErrorCode::Success;
}

ErrorCode Job::GetProcessIds(std::vector<uint64_t>& result)
{
    auto errCode = ErrorCode::Success;

    if (m_Job == INVALID_HANDLE_VALUE)
        if ((errCode = InitializeJob()) != ErrorCode::Success)
            return errCode;
    
    size_t elementsCount = 64;
    size_t processIdsListSize;
    ProcessIdsListPtr processIdsListPtr;

    do
    {
        processIdsListPtr = GetProcessIdsListPtr(elementsCount, processIdsListSize);

        if (!QueryInformationJobObject(
            m_Job,
            JobObjectBasicProcessIdList,
            processIdsListPtr.get(),
            processIdsListSize,
            NULL
        ) && GetLastError() != ERROR_MORE_DATA)
            return ErrorCode::QueryJobObjectInformationFailed;

        if (processIdsListPtr->NumberOfAssignedProcesses != 0 &&
            processIdsListPtr->NumberOfAssignedProcesses ==
            processIdsListPtr->NumberOfProcessIdsInList)
            break;

        elementsCount *= 2;
    } while(GetLastError() == ERROR_MORE_DATA);

    size_t actualProcessesCount = processIdsListPtr->NumberOfProcessIdsInList;

    result.resize(actualProcessesCount);

    std::copy(
        processIdsListPtr->ProcessIdList,
        processIdsListPtr->ProcessIdList + actualProcessesCount,
        result.begin());

    return ErrorCode::Success;
}

ErrorCode Job::StartProcess(
    const std::wstring& cmdline,
    const std::wstring& workingDir)
{
    ErrorCode errCode;

    if (m_Job == INVALID_HANDLE_VALUE)
        if ((errCode = InitializeJob()) != ErrorCode::Success)
            return errCode;

    PROCESS_INFORMATION processInfo;
    ZeroMemory(&processInfo, sizeof(processInfo));

    STARTUPINFOW startupInfo;
    ZeroMemory(&startupInfo, sizeof(startupInfo));
    startupInfo.cb = sizeof(startupInfo);

    startupInfo.hStdOutput =
        startupInfo.hStdError =
        startupInfo.hStdInput = INVALID_HANDLE_VALUE;

    auto cmdLineBuffer = ToCharBuffer(cmdline);
    auto workingDirBuffer = ToCharBuffer(workingDir);

    if (!CreateProcessW(
        NULL,
        cmdLineBuffer.get(),
        NULL,
        NULL,
        TRUE,
        CREATE_SUSPENDED,
        NULL,
        workingDir == L"" ? NULL : workingDirBuffer.get(),
        &startupInfo,
        &processInfo
    ))
        return ErrorCode::ProcessCreationFailed;

    AssignProcessToJobObject(m_Job, processInfo.hProcess);
    ResumeThread(processInfo.hThread);

    CloseHandle(processInfo.hProcess);
    CloseHandle(processInfo.hThread);

    m_IsAlive = true;

    return ErrorCode::Success;
}

bool Job::IsAlive()
{
    while (true)
    {
        DWORD completionCode;
        ULONG_PTR completionKey;
        LPOVERLAPPED overlapped;

        if (!GetQueuedCompletionStatus(
            m_CompletionPort,
            &completionCode,
            &completionKey,
            &overlapped,
            0
        ))
            break;

        if (completionKey == CompletionKey &&
            completionCode == JOB_OBJECT_MSG_ACTIVE_PROCESS_ZERO)
            m_IsAlive = false;
    }

    return m_IsAlive;
}

void Job::Kill()
{
    auto retValue = TerminateJobObject(m_Job, 0);
    m_IsAlive = false;
}

ErrorCode Job::Terminate()
{
    std::vector<uint64_t> processIdsList;
    auto errorCode = GetProcessIds(processIdsList);

    if (errorCode != ErrorCode::Success)
        return errorCode;

    std::set<uint64_t> processIds(processIdsList.begin(), processIdsList.end());

    EnumWindows(&EnumWindowsCallback, (LPARAM)&processIds);

    Kill();

    return ErrorCode::Success;
}

BOOL CALLBACK EnumWindowsCallback(
  _In_ HWND   hwnd,
  _In_ LPARAM lParam)
{
    EnumChildWindows(hwnd, &EnumChildWindowsCallback, lParam);
    return TerminateProcessIfItIsInTheJob(hwnd, lParam);
}

BOOL CALLBACK EnumChildWindowsCallback(
  _In_ HWND   hwnd,
  _In_ LPARAM lParam)
{
    return TerminateProcessIfItIsInTheJob(hwnd, lParam);
}

BOOL TerminateProcessIfItIsInTheJob(HWND hwnd, LPARAM lParam)
{
    DWORD processId;

    if (!GetWindowThreadProcessId(hwnd, &processId))
        return TRUE;

    auto processIdsPtr = (std::set<uint64_t>*)lParam;

    if (processIdsPtr->find(processId) != processIdsPtr->end())
        EndTask(hwnd, FALSE, TRUE);

    return TRUE;
}
