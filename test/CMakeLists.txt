cmake_minimum_required(VERSION 3.10)
project(TestRatatoskr)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
add_compile_options(-g -O0)
include_directories(${CMAKE_SOURCE_DIR}/ratatoskr/src)

#set (CXXTESTFLAGS --error-printer --no-eh)
set (CXXTESTFLAGS --error-printer)

set (TESTS testcommandlineparameters testprogramdescriptions testinterpretparameters)
enable_testing()
foreach(test ${TESTS})
	set (runner run${test}.cpp)
	set (testdefinition ${test}.h)
	add_executable(${test} ${runner} src/${testdefinition} src/test.h)
	add_custom_command(
	  OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${runner}
	  COMMAND ${CXXTEST_DIR}/bin/cxxtestgen ${CXXTESTFLAGS} -o ${runner} ${PROJECT_SOURCE_DIR}/src/${testdefinition}
	  DEPENDS src/${testdefinition} src/test.h 
	 )
	add_test(NAME ${test} COMMAND ${test}) 
endforeach()