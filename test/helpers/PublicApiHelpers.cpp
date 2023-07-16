#include "PublicApiHelpers.h"

std::unique_ptr<void, decltype(&DestroyJob)> InitializeJob()
{
    return std::unique_ptr<void, decltype(&DestroyJob)>(CreateJob(), &DestroyJob);
}
