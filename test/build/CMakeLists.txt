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

# AnalyzerCommon
if( NOT AnalyzerCommon_DIR )
    if( WIN32 )
        set(AnalyzerCommon_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../AnalyzerCommon/include)
    else()
        set(AnalyzerCommon_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../../../3rdparty/AnalyzerCommon/include/AnalyzerCommon)
    endif()
endif()
message("* AnalyzerCommon_DIR: ${AnalyzerCommon_DIR}")
set(INCLUDES ${INCLUDES} ${AnalyzerCommon_DIR})

# boost libraries
set (BOOST_LIBS system thread program_options)
if( NOT Boost_DIR )
    if( WIN32 )
    	SET(Boost_DIR ${PROJECT_SOURCE_DIR}/../../../../internal_release)
    else()
    	set(Boost_DIR ${PROJECT_SOURCE_DIR}/../../../../3rdparty)
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

if( $ENV{BOOST_DIR} STREQUAL "")
#	find_package( Boost REQUIRED ${BOOST_LIBS} )
#	if(Boost_FOUND)
#		set(INCLUDES ${INCLUDES} ${Boost_INCLUDE_DIRS})
#		set(LIBS ${LIBS} ${Boost_LIBRARIES})
#	else()
#		message("Error! No proper boost libraries found")
#	endif()
else()
	message("Found BOOST_DIR env: $ENV{BOOST_DIR}" )
	set(INCLUDES ${INCLUDES} $ENV{BOOST_DIR}/include)
	set(LIBPATHS ${LIBPATHS} $ENV{BOOST_DIR}/lib/${ARCH}/${BUILDTYPE})
endif()

add_definitions( -DBOOST_ALL_DYN_LINK )

# winsock library
if( WIN32 )
	set(LIBS ${LIBS} ws2_32)
endif()

set(mqa_DIR "../.." CACHE STRING "Set mqa library path containing /include and /lib" FORCE)

set(LIBPATHS ${LIBPATHS} ${mqa_DIR}/lib/${ARCH}/${BUILDTYPE})
set(INCLUDES ${INCLUDES} ${mqa_DIR}/include)

set(LIBS ${LIBS} mqa)
LINK_DIRECTORIES(${LIBPATHS})

message("- link libs: ${LIBS}")
message("- link dirs: ${LIBPATHS}")

#----------------------------------

set(INCLUDES ${INCLUDES} ${CMAKE_CURRENT_SOURCE_DIR}/../src)

include_directories(${INCLUDES})

file(GLOB SRC
#    "${CMAKE_CURRENT_SOURCE_DIR}/../*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/*.cpp"
)

add_executable( mqatest ${SRC} )

target_link_libraries(mqatest mqa ${LIBS})

if( WIN32 )
	set(OUTPUT_DIR ${PROJECT_SOURCE_DIR}/../bin/${ARCH})
else()
	set(OUTPUT_DIR ${PROJECT_SOURCE_DIR}/../bin/${ARCH}/${BUILDTYPE})
endif()

set_target_properties(mqatest PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_DIR})
