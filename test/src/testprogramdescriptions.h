#include <cxxtest/TestSuite.h>
#include "test.h"

#include "parameters/programdescriptions.h"

using namespace GiNaC;
using namespace Wedge;
using namespace ratatoskr;


struct StringCommandLineParameters {
	string string_parameter1;
	string string_parameter2;
};

struct IntCommandLineParameters {
	int int_parameter1;
	int int_parameter2;
};

struct BoolCommandLineParameters {
	bool bool_parameter1;
	bool bool_parameter2;
};


auto description_strings=make_parameter_description<StringCommandLineParameters>
(
		"param1","first parameter",&StringCommandLineParameters::string_parameter1,
		"param2","second parameter",&StringCommandLineParameters::string_parameter2
);
auto description_integers=make_parameter_description<IntCommandLineParameters>
(
		"param1","first parameter",&IntCommandLineParameters::int_parameter1,
		"param2","second parameter",&IntCommandLineParameters::int_parameter2
);
auto description_booleans=make_parameter_description<BoolCommandLineParameters>
(
		"param1","first parameter",&BoolCommandLineParameters::bool_parameter1,
		"param2","second parameter",&BoolCommandLineParameters::bool_parameter2
);

auto program_descriptions=alternative_program_descriptions(
		"stringprogram", "run a test program with string parameters", description_strings,
			[] (auto parameters) {
				TS_ASSERT_EQUALS(parameters.string_parameter1,"number1")},
		"intprogram", "run a test program with int parameters", description_integers,
		[] (auto parameters) {
			TS_ASSERT_EQUALS(parameters.int_parameter1,-2)},
		"boolprogram", "run a test program with bool parameters", description_booleans,
		[] (auto parameters) {
			TS_ASSERT_EQUALS(parameters.bool_parameter1,true)}
);

class CommandLineCommandTestSuite : public CxxTest::TestSuite
{
public:
	void testProgramDescriptions1() {
		const char* (argv[]) {"program invocation", "stringprogram", "--param1=number1", "--param2=2"};
		int argc=std::size(argv);
		TS_ASSERT(program_descriptions.run(argc,argv));
	}
	void testProgramDescriptions2() {
		const char* (argv[]) {"program invocation", "intprogram", "--param1=-2", "--param2=2"};
		int argc=std::size(argv);
		TS_ASSERT(program_descriptions.run(argc,argv));
	}
	void testProgramDescriptions3() {
		const char* (argv[]) {"program invocation", "boolprogram", "--param1=true", "--param2=false"};
		int argc=std::size(argv);
		TS_ASSERT(program_descriptions.run(argc,argv));
	}
	void testProgramDescriptions4() {
		const char* (argv[]) {"program invocation", "noprogram", "--param1=-3", "--param2=2"};
		int argc=std::size(argv);
		TS_ASSERT(!program_descriptions.run(argc,argv));
	}
};
