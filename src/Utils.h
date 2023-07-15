#pragma once
#include <string>
#include <memory>
#include <Windows.h>

void SafeCloseHandle(HANDLE& handle);

std::unique_ptr<wchar_t[]> ToCharBuffer(const std::wstring& string);
