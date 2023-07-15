#include "PublicApiHelpers.h"

std::unique_ptr<void, decltype(&Cleanup)> InitializeJob()
{
    return std::unique_ptr<void, decltype(&Cleanup)>(Initialize(), &Cleanup);
}
