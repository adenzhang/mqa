project(mqa)

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
set (BOOST_LIBS system thread)

if( NOT Boost_DIR )
    if( WIN32 )
        SET(Boost_DIR  ${CMAKE_CURRENT_SOURCE_DIR}/../../../internal_release) # include boost
    else()
        SET(Boost_DIR  ${CMAKE_CURRENT_SOURCE_DIR}/../../../3rdparty/boost)
    endif()
endif()
message("* Boost_DIR: ${Boost_DIR}")
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

message("- link libs: ${LIBS}")
message("- link dirs: ${LIBPATHS}")


#----------------------------------
set(INCLUDES ${INCLUDES} ${CMAKE_CURRENT_SOURCE_DIR}/../include)

message("- INCLUDES: ${INCLUDES}")
include_directories(${INCLUDES})

set( SRC_FILES
MQmonIf
RtpPacketParser
RtpStream
RtpTypes
ThreadPool
emodel
)

foreach(F ${SRC_FILES})
    set(SRC ${SRC} ${CMAKE_CURRENT_SOURCE_DIR}/../src/${F}.cpp)
endforeach()

#file(GLOB SRC
#    "${CMAKE_CURRENT_SOURCE_DIR}/../src/*.cpp"
#)

LINK_DIRECTORIES(${LIBPATHS})

add_definitions( -DMQA_EXPORTS )

add_library( mqa SHARED ${SRC} )

target_link_libraries(mqa ${LIBS})

set(OUTPUT_BINDIR ${OUTPUT_DIR})
if( NOT OUTPUT_DIR )
    if( MSVC_IDE )
        set(OUTPUT_DIR ${PROJECT_BINARY_DIR}/../lib/${ARCH})
        set(OUTPUT_BINDIR ${OUTPUT_DIR}/${BUILDTYPE})
    else()
        set(OUTPUT_DIR ${PROJECT_BINARY_DIR}/../lib/${ARCH}/${BUILDTYPE})
        set(OUTPUT_BINDIR ${OUTPUT_DIR})
    endif()
endif()

set_target_properties(mqa PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_DIR})
set_target_properties(mqa PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${OUTPUT_DIR})
set_target_properties(mqa PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${OUTPUT_DIR})

INSTALL(TARGETS mqa DESTINATION ${CMAKE_INSTALL_PREFIX_MRI}/lib COMPONENT mqa)
