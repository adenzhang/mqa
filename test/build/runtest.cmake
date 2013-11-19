# user set vars
# TEST_PROG, SOURCEDIR, OUTPUT_FILE, EXPECTED_FILE

message("*** runtest.cmake ***")
if( NOT OUTPUT_FILE )
	set(OUTPUT_FILE output.txt)
endif()

separate_arguments(LINKDIRS)
set(ENV{PATH} "$ENV{PATH};${LINKDIRS}")
#message("ENV PATH: $ENV{PATH}")

#separate_arguments( ARGS )
set(ARGS ${ARG1} ${ARG2} ${ARG3} ${ARG4} ${ARG5} ${ARG6} ${ARG7} ${ARG8} ${ARG9} ${ARGS})
message("executing...: ${TEST_PROG} ${ARGS}")
execute_process(COMMAND ${TEST_PROG} ${ARGS}
                OUTPUT_FILE ${OUTPUT_FILE}
                RESULT_VARIABLE HAD_ERROR
)
if(HAD_ERROR)
    message(FATAL_ERROR "*** Test Failed: ${HAD_ERROR}")
endif()
message("completed: ${TEST_PROG} - ${HAD_ERROR}")

if( EXPECTED_FILE )
	if( NOT NOVERIFY )
	execute_process(COMMAND ${CMAKE_COMMAND} -E compare_files
	    ${OUTPUT_FILE} ${EXPECTED_FILE}
	    RESULT_VARIABLE DIFFERENT)
	if(DIFFERENT)
	    message(FATAL_ERROR "Test failed - files differ ${OUTPUT_FILE} -> ${EXPECTED_FILE}")
	endif()
	endif()
endif()
