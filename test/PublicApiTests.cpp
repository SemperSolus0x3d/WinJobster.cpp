#include <iostream>
#include <cassert>
#include <memory>
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <PublicApi.h>
#include "helpers/Constants.h"
#include "helpers/PublicApiHelpers.h"

TEST_CASE("ItStartsAndKillsProcess", "[public-api]")
{
    auto handle = InitializeJob();
    auto errorCode = StartProcess(ExistingExecutable, L"", handle.get());

    REQUIRE(errorCode == ErrorCode::Success);
    REQUIRE(IsAlive(handle.get()) == true);

    Kill(handle.get());

    REQUIRE(IsAlive(handle.get()) == false);
}

TEST_CASE("ItReturnsCorrectProcessIdsCount", "[public-api]")
{
    auto handle = InitializeJob();
    const size_t ProcessesCount = GENERATE(1, 2, 10, 15);

    for (size_t i = 0; i < ProcessesCount; i++)
    {
        auto errorCode = StartProcess(ExistingExecutable, L"", handle.get());
        REQUIRE(errorCode == ErrorCode::Success);
    }

    uint64_t* processIdsRawPtr;
    size_t processesCount;
    auto errCode = GetProcessIds(handle.get(), &processIdsRawPtr, &processesCount);

    REQUIRE(errCode == ErrorCode::Success);

    auto processIds = CreateUniquePtr(processIdsRawPtr);

    REQUIRE(processesCount == ProcessesCount);

    for (size_t i = 0; i < processesCount; i++)
        CHECK(processIds.get()[i] != 0);
}
