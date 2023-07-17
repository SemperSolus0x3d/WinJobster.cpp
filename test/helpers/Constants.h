#pragma once
#include <cstdint>

constexpr const wchar_t* ExistingExecutable = L"notepad.exe";
constexpr const wchar_t* NonExistingExecutable = L"sbvhkdskdvdskjbnrmvsh.exe";

constexpr const uint32_t DefaultTerminateTimeoutMs = 10000; // 10 s
constexpr const uint32_t DefaultProcessInitTimeoutMs = 10000; // 10 s
