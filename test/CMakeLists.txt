cmake_minimum_required(VERSION 3.24)

project(WinJobsterTest VERSION 1.0.0 LANGUAGES CXX)

macro(setup_test_executable name add_as_test)
    add_executable(${name} ${name}.cpp)
    target_link_libraries(${name} WinJobster)

    add_custom_command(TARGET ${name} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different 
            $<TARGET_RUNTIME_DLLS:${name}> 
            $<TARGET_FILE_DIR:${name}>
        COMMAND_EXPAND_LISTS
    )

    if(${add_as_test})
        add_test(NAME ${name} COMMAND "$<TARGET_FILE:${name}>")
    endif()
endmacro()

setup_test_executable(ManualTest NO)
setup_test_executable(Tests YES)
