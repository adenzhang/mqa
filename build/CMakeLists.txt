project(mqa)

cmake_minimum_required( VERSION 2.6 )

# check 64 bit
if( CMAKE_SIZEOF_VOID_P EQUAL 8 )
    set( ARCH x64 )
else()
    set( ARCH x86 )
endif()

if( NOT CMAKE_BUILD_TYPE )
	set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build, options are: Release Debug" FORCE)
endif()

if (CMAKE_BUILD_TYPE STREQUAL Debug)
	set( BUILDTYPE debug)
else()
	set( BUILDTYPE release)
endif()

message("*** ${ARCH}/${BUILDTYPE} ***")
  
# boost libraries
set (BOOST_LIBS system thread)

set(Boost_USE_STATIC_LIBS OFF) 
set(Boost_USE_MULTITHREADED ON)  
set(Boost_USE_STATIC_RUNTIME OFF) 

if( $ENV{BOOSTDIR} STREQUAL "")
	find_package( Boost REQUIRED ${BOOST_LIBS} )
	if(Boost_FOUND)
		include_directories(${Boost_INCLUDE_DIRS})
		set(LIBS ${LIBS} ${Boost_LIBRARIES})
	else()
		message("Error! No proper boost libraries found")
	endif()
else()
	message("Found BOOSTDIR env: $ENV{BOOSTDIR}" )
	include_directories($ENV{BOOSTDIR}/include)
	set(LIBPATHS ${LIBPATHS} $ENV{BOOSTDIR}/lib/${ARCH}/${BUILDTYPE})
endif()

add_definitions( -DBOOST_ALL_DYN_LINK )

message("- link libs: ${LIBS}")
message("- link dirs: ${LIBPATHS}")

# AnalyzerCommon
SET(AnalyzerCommon_DIR
	${CMAKE_CURRENT_SOURCE_DIR}../AnalyzerCommon CACHE
	PATH "AnalyzerCommon path" FORCE)
include_directories(${AnalyzerCommon_DIR})

#----------------------------------
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../include)

file(GLOB SRC
#    "${CMAKE_CURRENT_SOURCE_DIR}/../*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/*.cpp"
)

LINK_DIRECTORIES(${LIBPATHS})

add_definitions( -DMQA_EXPORTS )

add_library( mqa SHARED ${SRC} )

target_link_libraries(mqa ${LIBS})

if( WIN32 )
	set(OUTPUT_DIR ${PROJECT_SOURCE_DIR}/../lib/${ARCH})
else()
	set(OUTPUT_DIR ${PROJECT_SOURCE_DIR}/../lib/${ARCH}/${BUILDTYPE})
endif()

set_target_properties(mqa PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_DIR})
set_target_properties(mqa PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${OUTPUT_DIR})
set_target_properties(mqa PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${OUTPUT_DIR})
