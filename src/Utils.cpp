#include <utility>
#include "Utils.h"

void SafeCloseHandle(HANDLE& handle)
{
    if (handle != INVALID_HANDLE_VALUE)
        CloseHandle(std::exchange(handle, INVALID_HANDLE_VALUE));
}

std::unique_ptr<wchar_t[]> ToCharBuffer(const std::wstring& string)
{
    auto buffer = std::make_unique<wchar_t[]>(string.size() + 1);

    memcpy(buffer.get(), string.data(), string.size() * sizeof(wchar_t));
    buffer[string.size()] = 0;

    return buffer;
}
