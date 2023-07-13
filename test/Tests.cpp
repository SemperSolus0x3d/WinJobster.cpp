#include <iostream>
#include <cassert>
#include <memory>
#include <Wrappers.h>

#define RUN_TEST(name) \
    std::cout << "Running test \"" << #name << "\"" << std::endl; \
    name();

constexpr wchar_t* ExistingExecutable = L"cmd.exe";
constexpr wchar_t* NonExistingExecutable = L"sbvhkdskdvdskjbnrmvsh.exe";


void ItStartsAndKillsProcess()
{
    void* rawHandle;
    auto errorCode = StartProcess(ExistingExecutable, L"", &rawHandle);

    std::unique_ptr<void, decltype(&Cleanup)> handle(rawHandle, &Cleanup);

    assert(errorCode == ErrorCode::Success);
    assert(IsAlive(handle.get()) == true);

    Kill(handle.get());

    assert(IsAlive(handle.get()) == false);
}

int main()
{
    RUN_TEST(ItStartsAndKillsProcess);
    return 0;
}
