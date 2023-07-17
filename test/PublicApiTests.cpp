#include <iostream>
#include <cassert>
#include <memory>
#include <thread>
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <PublicApi.h>
#include "helpers/Constants.h"
#include "helpers/PublicApiHelpers.h"

TEST_CASE("It starts and kills a single process", "[public-api]")
{
    auto handle = InitializeJob();
    auto errorCode = StartProcess(
        handle.get(),
        ExistingExecutable,
        L"",
        DefaultProcessInitTimeoutMs);

    REQUIRE(errorCode == ErrorCode::Success);
    REQUIRE(IsAlive(handle.get()) == true);

    SECTION("It kills process")
    {
        Kill(handle.get());
        REQUIRE(IsAlive(handle.get()) == false);
    }

    SECTION("It terminates process")
    {
        errorCode = Terminate(handle.get(), DefaultTerminateTimeoutMs);
        REQUIRE(errorCode == ErrorCode::Success);
        REQUIRE(IsAlive(handle.get()) == false);
    }
}

TEST_CASE("It works with multiple processes", "[public-api]")
{
    auto handle = InitializeJob();
    const size_t ProcessesCount = GENERATE(1, 2, 10, 15);

    for (size_t i = 0; i < ProcessesCount; i++)
    {
        auto errorCode = StartProcess(
            handle.get(),
            ExistingExecutable,
            L"",
            DefaultProcessInitTimeoutMs);

        REQUIRE(errorCode == ErrorCode::Success);
    }

    REQUIRE(IsAlive(handle.get()) == true);

    SECTION("It returns correct process ids")
    {
        uint64_t* processIdsRawPtr;
        size_t processesCount;
        auto errCode = GetProcessIds(handle.get(), &processIdsRawPtr, &processesCount);

        REQUIRE(errCode == ErrorCode::Success);

        auto processIds = CreateUniquePtr(processIdsRawPtr);

        REQUIRE(processesCount == ProcessesCount);

        for (size_t i = 0; i < processesCount; i++)
            CHECK(processIds.get()[i] != 0);
    }

    SECTION("Kill and termination")
    {
        SECTION("It kills processes")
        {
            Kill(handle.get());
        }

        SECTION("It terminates processes")
        {
            auto errorCode = Terminate(handle.get(), DefaultTerminateTimeoutMs);

            REQUIRE(errorCode == ErrorCode::Success);
        }

        REQUIRE(IsAlive(handle.get()) == false);

        uint64_t* processIdsRawPtr;
        size_t processesCount;
        auto errCode = GetProcessIds(handle.get(), &processIdsRawPtr, &processesCount);

        REQUIRE(errCode == ErrorCode::Success);
        REQUIRE(processesCount == 0);
    }
}

TEST_CASE("It terminates processes if no process is started", "[public-api]")
{
    auto handle = InitializeJob();
    REQUIRE(IsAlive(handle.get()) == false);

    auto errorCode = Terminate(handle.get(), DefaultTerminateTimeoutMs);

    REQUIRE(errorCode == ErrorCode::Success);
    REQUIRE(IsAlive(handle.get()) == false);
}

TEST_CASE("It starts process after terminate or kill", "[public-api]")
{
    auto handle = InitializeJob();
    REQUIRE(IsAlive(handle.get()) == false);

    auto errorCode = StartProcess(
        handle.get(),
        ExistingExecutable,
        L"",
        DefaultProcessInitTimeoutMs);

    REQUIRE(errorCode == ErrorCode::Success);

    SECTION("Terminate")
    {
        errorCode = Terminate(handle.get(), DefaultTerminateTimeoutMs);
        REQUIRE(errorCode == ErrorCode::Success);
    }

    SECTION("Kill")
    {
        Kill(handle.get());
    }

    REQUIRE(IsAlive(handle.get()) == false);

    errorCode = StartProcess(
        handle.get(),
        ExistingExecutable,
        L"",
        DefaultProcessInitTimeoutMs);

    REQUIRE(errorCode == ErrorCode::Success);
    REQUIRE(IsAlive(handle.get()) == true);
}
