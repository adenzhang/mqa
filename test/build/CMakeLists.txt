project(mqatest)

cmake_minimum_required( VERSION 2.6 )

# check 64 bit
if( CMAKE_SIZEOF_VOID_P EQUAL 8 )
    set( ARCH x64 )
else()
    set( ARCH x86 )
endif()

if( NOT CMAKE_BUILD_TYPE )
	set(CMAKE_BUILD_TYPE Release)
endif()

if (CMAKE_BUILD_TYPE STREQUAL Debug)
	set( BUILDTYPE debug)
else()
	set( BUILDTYPE release)
endif()
message("*** ${ARCH}/${BUILDTYPE} ***")

# boost libraries
set (BOOST_LIBS system thread program_options)
if( NOT Boost_DIR )
    if( WIN32 )
    	SET(Boost_DIR ${PROJECT_SOURCE_DIR}/../../../../internal_release)
    else()
        set(Boost_DIR ${PROJECT_SOURCE_DIR}/../../../../3rdparty/boost)
    endif()
endif()
set(INCLUDES ${INCLUDES} ${Boost_DIR}/include)
set(LIBPATHS ${LIBPATHS} ${Boost_DIR}/lib)
set(LIBPATHS ${LIBPATHS} ${Boost_DIR}/lib/${ARCH}/${BUILDTYPE})
if( NOT WIN32 )
	foreach(F ${BOOST_LIBS})
		set(LIBS ${LIBS} boost_${F})
	endforeach()
endif()

set(Boost_USE_STATIC_LIBS OFF) 
set(Boost_USE_MULTITHREADED ON)  
set(Boost_USE_STATIC_RUNTIME OFF) 

add_definitions( -DBOOST_ALL_DYN_LINK )

# winsock library
if( WIN32 )
	set(LIBS ${LIBS} ws2_32)
endif()

set(LIBPATHS ${LIBPATHS} ${CMAKE_CURRENT_BINARY_DIR}/../../lib/${ARCH}/${BUILDTYPE}) #mqa library
# mqa library
set(mqa_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../..)

if( NOT mqa_LIBDIR )
    set(mqa_LIBDIR ${mqa_DIR}/lib/${ARCH}/${BUILDTYPE})
endif()
set(LIBPATHS ${LIBPATHS} ${mqa_LIBDIR} )
set(INCLUDES ${INCLUDES} ${mqa_DIR}/include)

set(LIBS ${LIBS} mqa)
LINK_DIRECTORIES(${LIBPATHS})

message("- link libs: ${LIBS}")
message("- link dirs: ${LIBPATHS}")

#----------------------------------

set(INCLUDES ${INCLUDES} ${CMAKE_CURRENT_SOURCE_DIR}/../src)

message("- INCLUDES dirs: ${INCLUDES}")

include_directories(${INCLUDES})

#file(GLOB SRC
#    "${CMAKE_CURRENT_SOURCE_DIR}/../*.h"
#    "${CMAKE_CURRENT_SOURCE_DIR}/../src/*.cpp"
#)
set(SRC ${SRC} ${CMAKE_CURRENT_SOURCE_DIR}/../../src/StatsFrameParser.cpp)

set( SRC_FILES
LogUtility
PcapReader
StreamManage
StreamTask
mqatest
)
foreach(F ${SRC_FILES})
    set(SRC ${SRC} ${CMAKE_CURRENT_SOURCE_DIR}/../src/${F}.cpp)
endforeach()

add_executable( mqatest ${SRC} )

target_link_libraries(mqatest mqa ${LIBS})

set(OUTPUT_BINDIR ${OUTPUT_DIR})
if( NOT OUTPUT_DIR )
    if( MSVC_IDE )
        set(OUTPUT_DIR ${PROJECT_BINARY_DIR}/../bin/${ARCH})  # for win32 proj file generation
        set(OUTPUT_BINDIR ${OUTPUT_DIR}/${BUILDTYPE})
    else()
        set(OUTPUT_DIR ${PROJECT_BINARY_DIR}/../bin/${ARCH}/${BUILDTYPE})
        set(OUTPUT_BINDIR ${OUTPUT_DIR})
    endif()
endif()
message("- OUTPUT_DIR: ${OUTPUT_DIR}")

set_target_properties(mqatest PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_DIR})

#------- BufferPoolTest ------
set( SRC_FILES
TestBufferPool
LogUtility
)
set(SRC)
foreach(F ${SRC_FILES})
    set(SRC ${SRC} ${CMAKE_CURRENT_SOURCE_DIR}/../src/${F}.cpp)
endforeach()

add_executable( bufferpooltest ${SRC})
set_target_properties(bufferpooltest PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_DIR})

#message("CMAKE_BINARY_DIR:${CMAKE_BINARY_DIR} CMAKE_CURRENT_BINARY_DIR:${CMAKE_CURRENT_BINARY_DIR} PROJECT_BINARY_DIR:${PROJECT_BINARY_DIR}")
#-------- run test --------------
enable_testing()
#set( TEST_PCAP D:/data/calls/fromFranc/CaptureDataTabStats.pcap)
#set( TEST_PCAP "D:/data/calls/MetroPCS_2012_12_07_161635(12_07_161636).pcap")
#set ( TEST_PCAP "D:/data/metro_2013_01_03_170646.pcap")
set( TEST_PCAP ${CMAKE_CURRENT_SOURCE_DIR}/call10.pcap)

add_test(NAME run_mqatest
WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
COMMAND ${CMAKE_COMMAND}
-DTEST_PROG=${OUTPUT_BINDIR}/mqatest
"-DARG1=--traffic=${TEST_PCAP}"
#"-DARG2=--threads=0"
"-DARG3=--time"
"-DLINKDIRS=${LIBPATHS}"
#"-DEXPECTED_FILE=${PROJECT_SOURCE_DIR}/expected.txt"
"-DOUTPUT_FILE=${OUTPUT_BINDIR}/output.txt"
-P ${CMAKE_CURRENT_SOURCE_DIR}/runtest.cmake
)

#add_test(NAME run_mqatest_mt
#WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
#COMMAND ${CMAKE_COMMAND}
#-DTEST_PROG=${OUTPUT_BINDIR}/mqatest
#"-DARG1=--traffic=${TEST_PCAP}"
#"-DARG2=--threads=4"
#"-DARG3=--time"
#"-DLINKDIRS=${LIBPATHS}"
#"-DEXPECTED_FILE=${PROJECT_SOURCE_DIR}/expected.txt"
#"-DOUTPUT_FILE=${OUTPUT_BINDIR}/output_mt.txt"
#-P ${CMAKE_CURRENT_SOURCE_DIR}/runtest.cmake
#)

#add_test(NAME run_BufferPoolTest
#WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
#COMMAND ${CMAKE_COMMAND}
#"-DLINKDIRS=${LIBPATHS}"
#-DTEST_PROG=${OUTPUT_BINDIR}/bufferpooltest
#"-DOUTPUT_FILE=${OUTPUT_BINDIR}/BufferPoolTest.txt"
#-P ${CMAKE_CURRENT_SOURCE_DIR}/runtest.cmake
#)
