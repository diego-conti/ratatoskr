cmake_minimum_required(VERSION 3.10)
project(TestRatatoskr)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
add_compile_options(-g -O0)

#set (CXXTESTFLAGS --error-printer --no-eh)
set (CXXTESTFLAGS --error-printer)

set (TESTS testcommandlineparameters testprogramdescriptions testdependentparameters testalternativeparameters testsymbols testgeneric
	testpairs)
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

#verify that the examples in README.md actually give the expected output
add_test(NAME ext_derivative_test COMMAND ratatoskr ext-derivative --lie-algebra 0,0,12 --form 3)
set_tests_properties(ext_derivative_test PROPERTIES PASS_REGULAR_EXPRESSION "e1\\*e2")
add_test(NAME ext_derivative_test_latex COMMAND ratatoskr ext-derivative --lie-algebra 0,0,12 --form 3 --latex)
set_tests_properties(ext_derivative_test_latex PROPERTIES PASS_REGULAR_EXPRESSION "e\\^{12}")
add_test(NAME convert_test COMMAND ratatoskr convert --amount 10 --conversion-ratio 6/5) 
set_tests_properties(convert_test PROPERTIES PASS_REGULAR_EXPRESSION "12")
add_test(NAME derivative_test COMMAND ratatoskr convert derivative --variable=x --function=x^2)
set_tests_properties(derivative_test PROPERTIES PASS_REGULAR_EXPRESSION "2*x")
add_test(NAME partial_derivative_test COMMAND ratatoskr partial-derivative --variable=x --function=x^2*y) 
set_tests_properties(partial_derivative_test PROPERTIES PASS_REGULAR_EXPRESSION "2*x*y")
add_test(NAME subalgebra_test COMMAND ratatoskr subalgebra-without-parameters --lie-algebra 0,0,12,13 --subalgebra 1,4,2,3 --latex)
set_tests_properties(subalgebra_test PROPERTIES PASS_REGULAR_EXPRESSION "(0,e\\^{14},0,e\\^{13})")
add_test(NAME subalgebra_with_parameters_test COMMAND ratatoskr subalgebra-with-parameters --lie-algebra 0,0,[a]*12,13 --subalgebra [a]*1,4,2,3 --latex)
set_tests_properties(subalgebra_with_parameters_test PROPERTIES PASS_REGULAR_EXPRESSION "(0,a e\\^{14},0,a\\^2 e\\^{13})")
