cmake_minimum_required(VERSION 3.24)

project(WinJobster VERSION 1.0.0 LANGUAGES CXX)

set(BUILD_SHARED_LIBS ON)

set(WINJOBSTER_SRC
    include/Defines.h
    include/ErrorCode.h
    include/PublicApi.h
    src/Utils.h
    src/Utils.cpp
    src/PublicApi.cpp
    src/Job.h
    src/Job.cpp
    src/Journal.h
    src/Journal.cpp
    src/WinapiHelpers.h
    src/WinapiHelpers.cpp
)

set(WINJOBSTER_TEST_SRC
    test/Tests.cpp
    test/PublicApiTests.cpp
    test/helpers/Constants.h
    test/helpers/PublicApiHelpers.h
    test/helpers/PublicApiHelpers.cpp
)

add_library(${PROJECT_NAME} ${WINJOBSTER_SRC})
set_target_properties(${PROJECT_NAME} PROPERTIES DEBUG_POSTFIX "D")

target_compile_definitions(
    ${PROJECT_NAME}
    PRIVATE
    WINJOBSTER_EXPORT
)

target_include_directories(${PROJECT_NAME}
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

install(TARGETS ${PROJECT_NAME})

if(BUILD_TESTING AND PROJECT_IS_TOP_LEVEL)
    find_package(Catch2 3 REQUIRED)

    add_executable(WinJobsterTests
        ${WINJOBSTER_SRC}
        ${WINJOBSTER_TEST_SRC}
    )

    include(Catch)
    include(CTest)
    enable_testing()

    target_link_libraries(WinJobsterTests PRIVATE Catch2::Catch2WithMain)
    target_include_directories(WinJobsterTests PRIVATE include)
    target_compile_definitions(WinJobsterTests PRIVATE WINJOBSTER_TESTING)

    catch_discover_tests(WinJobsterTests)
endif()
