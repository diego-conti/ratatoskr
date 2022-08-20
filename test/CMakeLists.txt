cmake_minimum_required(VERSION 3.10)
project(TestRatatoskr)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
add_compile_options(-g -O0)

if (DEFINED ${DEBUG})
	set (CXXTESTFLAGS --error-printer --no-eh)
else()
	set (CXXTESTFLAGS --error-printer)
endif()

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
add_test(NAME derivative_test COMMAND ratatoskr derivative --variable=x --function=x^2)
set_tests_properties(derivative_test PROPERTIES PASS_REGULAR_EXPRESSION "2*x")
add_test(NAME partial_derivative_test COMMAND ratatoskr partial-derivative --variable=x --function=x^2*y) 
set_tests_properties(partial_derivative_test PROPERTIES PASS_REGULAR_EXPRESSION "2*x*y")
add_test(NAME subalgebra_test COMMAND ratatoskr subalgebra-without-parameters --lie-algebra 0,0,12,13 --subalgebra 1,4,2,3 --latex)
set_tests_properties(subalgebra_test PROPERTIES PASS_REGULAR_EXPRESSION "(0,e\\^{14},0,e\\^{13})")
add_test(NAME subalgebra_with_parameters_test COMMAND ratatoskr subalgebra-with-parameters --lie-algebra 0,0,[a]*12,13 --subalgebra [a]*1,4,2,3 --latex)
set_tests_properties(subalgebra_with_parameters_test PROPERTIES PASS_REGULAR_EXPRESSION "(0,a e\\^{14},0,a\\^2 e\\^{13})")
add_test(NAME curvature_on_frame_test COMMAND ratatoskr curvature --lie-algebra 0,0,12,13  --signature=3,1 --metric-by-on-coframe "[1/sqrt(2)]*(1+3),2,4,[1/sqrt(2)]*(1-3)")
set_tests_properties(curvature_on_frame_test PROPERTIES PASS_REGULAR_EXPRESSION "Ricci tensor=\\[\\[1/2,0,0,0\\],\\[0,0,0,0\\],\\[0,0,-1/2,0\\],\\[0,0,0,-1/2\\]\\]")
add_test(NAME curvature_flat_test COMMAND ratatoskr curvature --lie-algebra 0,0,12,13  --metric-by-flat 3,2,1,4)
set_tests_properties(curvature_flat_test PROPERTIES PASS_REGULAR_EXPRESSION "Ricci tensor=\\[\\[0,0,1/2,0\\],\\[0,0,0,0\\],\\[1/2,0,0,0\\],\\[0,0,0,-1/2\\]\\]")
