#include "Job.h"
#include "Utils.h"
#include "Journal.h"

const ULONG_PTR CompletionKey = 52534;

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

    journal += [=](){ CloseHandle(m_Job); };

    m_CompletionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);

    if (m_CompletionPort == NULL)
        return ErrorCode::CompletionPortCreationFailed;

    journal += [=](){ CloseHandle(m_CompletionPort); };

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

ErrorCode Job::GetProcessIds(std::vector<ULONG_PTR>& result)
{
    JOBOBJECT_BASIC_PROCESS_ID_LIST dummyList;
    dummyList.NumberOfProcessIdsInList = 1;

    if (!QueryInformationJobObject(
        m_Job,
        JobObjectBasicProcessIdList,
        &dummyList,
        sizeof(dummyList),
        NULL
    ) && GetLastError() != ERROR_MORE_DATA)
        return ErrorCode::QueryJobObjectInformationFailed;

    size_t processesCount = dummyList.NumberOfAssignedProcesses;
    size_t requiredBufferSize = sizeof(JOBOBJECT_BASIC_PROCESS_ID_LIST)
        + (processesCount - 1) * sizeof(ULONG_PTR);

    requiredBufferSize *= 1.2;

    auto processIdsListBuffer = std::make_unique<unsigned char[]>(requiredBufferSize);

    auto processIdsListPtr = (JOBOBJECT_BASIC_PROCESS_ID_LIST*)processIdsListBuffer.get();
    processIdsListPtr->NumberOfProcessIdsInList = processesCount;

    if (!QueryInformationJobObject(
        m_Job,
        JobObjectBasicProcessIdList,
        processIdsListPtr,
        requiredBufferSize,
        NULL
    ))
        return ErrorCode::QueryJobObjectInformationFailed;

    auto actualProcessesCount = processIdsListPtr->NumberOfProcessIdsInList;

    result.resize(dummyList.NumberOfAssignedProcesses);

    memcpy(
        result.data(),
        processIdsListPtr->ProcessIdList,
        sizeof(ULONG_PTR) * actualProcessesCount);

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


