#pragma once
#include <memory>
#include <PublicApi.h>

std::unique_ptr<void, decltype(&DestroyJob)> InitializeJob();

template<class T>
auto CreateUniquePtr(T* rawPtr)
{
    auto deleter = [](T* rawPtr){ FreeMemory(rawPtr); };

    return std::unique_ptr<T, decltype(deleter)>(rawPtr, deleter);
}
