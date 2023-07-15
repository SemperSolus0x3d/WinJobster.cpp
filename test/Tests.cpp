#include <iostream>
#include <cassert>
#include <memory>
#include <Wrappers.h>

#define RUN_TEST(name) \
    std::cout << "Running test \"" << #name << "\"" << std::endl; \
    name();

constexpr const wchar_t* ExistingExecutable = L"notepad.exe";
constexpr const wchar_t* NonExistingExecutable = L"sbvhkdskdvdskjbnrmvsh.exe";

std::unique_ptr<void, decltype(&Cleanup)> InitializeJob()
{
    return std::unique_ptr<void, decltype(&Cleanup)>(Initialize(), &Cleanup);
}

template<class T>
auto CreateUniquePtr(T* rawPtr)
{
    auto deleter = [](T* rawPtr){ FreeMemory(rawPtr); };

    return std::unique_ptr<T, decltype(deleter)>(rawPtr, deleter);
}

void ItStartsAndKillsProcess()
{
    auto handle = InitializeJob();
    auto errorCode = StartProcess(ExistingExecutable, L"", handle.get());

    assert(errorCode == ErrorCode::Success);
    assert(IsAlive(handle.get()) == true);

    Kill(handle.get());

    assert(IsAlive(handle.get()) == false);
}

void ItReturnsCorrectProcessIds()
{
    auto handle = InitializeJob();
    const size_t ProcessesCount = 10;

    for (size_t i = 0; i < ProcessesCount; i++)
    {
        auto errorCode = StartProcess(ExistingExecutable, L"", handle.get());
        assert(errorCode == ErrorCode::Success);
    }

    uint64_t* processIdsRawPtr;
    size_t processesCount;
    auto errCode = GetProcessIds(handle.get(), &processIdsRawPtr, &processesCount);

    assert(errCode == ErrorCode::Success);

    auto processIds = CreateUniquePtr(processIdsRawPtr);

    assert(processesCount == ProcessesCount);
}

int main()
{
    RUN_TEST(ItStartsAndKillsProcess);
    RUN_TEST(ItReturnsCorrectProcessIds);
    return 0;
}
