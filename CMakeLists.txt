cmake_minimum_required(VERSION 3.24)

project(WinJobster VERSION 1.0.0 LANGUAGES CXX)

set(BUILD_SHARED_LIBS ON)

set(WINJOBSTER_SRC
    include/Defines.h
    include/Wrappers.h
    src/Utils.h
    src/Utils.cpp
    src/Wrappers.cpp
    src/Job.h
    src/Job.cpp
    src/Journal.h
    src/Journal.cpp
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

if(BUILD_TESTING AND PROJECT_IS_TOP_LEVEL)
    include(CTest)
    enable_testing()
    add_subdirectory(test)
endif()

install(TARGETS ${PROJECT_NAME})
