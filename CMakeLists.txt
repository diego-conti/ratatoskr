cmake_minimum_required(VERSION 3.10)
include(FetchContent)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
project(Ratatoskr VERSION 0.1)

set(CXXTEST_DIR ${CMAKE_SOURCE_DIR}/cxxtest) 
include_directories(${CXXTEST_DIR} ${CXXTEST_DIR}/cxxtest $ENV{WEDGE_PATH}/include)
link_libraries(wedge ginac cocoa gmp boost_program_options cln)
link_directories($ENV{WEDGE_PATH}/lib)

include_directories(${CMAKE_SOURCE_DIR}/ratatoskr/src)
add_subdirectory(ratatoskr)
add_subdirectory(test)

FetchContent_Declare(cxxtest
	GIT_REPOSITORY  https://github.com/CxxTest/cxxtest
	GIT_TAG 191adddb3876ab389c0c856e1c03874bf70f8ee4
	GIT_SHALLOW true 
	SOURCE_DIR ${CXXTEST_DIR}
	CONFIGURE_COMMAND ""
	TEST_COMMAND ""
	BUILD_COMMAND ""
	INSTALL_COMMAND ""
)
FetchContent_Populate(cxxtest)

