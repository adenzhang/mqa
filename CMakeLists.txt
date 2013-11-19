project (mqaall)
cmake_minimum_required(VERSION 2.6)

add_subdirectory(build)

option(test "test" ON)
if( test )
    add_subdirectory(test/build)
endif()
