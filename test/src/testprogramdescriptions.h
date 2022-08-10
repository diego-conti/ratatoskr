#include <cxxtest/TestSuite.h>
#include "test.h"

#include "programdescriptions.h"

using namespace GiNaC;
using namespace Wedge;


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


class CommandLineCommandTestSuite : public CxxTest::TestSuite
{
	DescriptionOfCommandLineParameters<StringCommandLineParameters> description_strings;
	DescriptionOfCommandLineParameters<IntCommandLineParameters> description_integers;
	DescriptionOfCommandLineParameters<BoolCommandLineParameters> description_booleans;

public:
	CommandLineCommandTestSuite() {
		description_strings.addStringCommandLineParameterDescription<&StringCommandLineParameters::string_parameter1>("param1","first parameter");
		description_strings.addStringCommandLineParameterDescription<&StringCommandLineParameters::string_parameter2>("param2","second parameter");

		description_integers.addIntegerCommandLineParameterDescription<&IntCommandLineParameters::int_parameter1>("param1","first parameter");
		description_integers.addIntegerCommandLineParameterDescription<&IntCommandLineParameters::int_parameter2>("param2","second parameter");

		description_booleans.addBooleanCommandLineParameterDescription<&BoolCommandLineParameters::bool_parameter1>("param1","first parameter");
		description_booleans.addBooleanCommandLineParameterDescription<&BoolCommandLineParameters::bool_parameter2>("param2","second parameter");
	}

	void testProgramDescriptions() {
		auto program_descriptions=alternative_program_descriptions(
				"stringprogram", description_strings,
					[] (auto parameters) {
						TS_ASSERT_EQUALS(parameters.string_parameter1,"number1")},
				"intprogram", description_integers,
				[] (auto parameters) {
					TS_ASSERT_EQUALS(parameters.int_parameter1,-2)},
				"boolprogram", description_booleans,
				[] (auto parameters) {
					TS_ASSERT_EQUALS(parameters.bool_parameter1,true)}
		);
		const char* (argv[]) {"program invocation", "stringprogram", "--param1=number1"};
		int argc=std::size(argv);
		program_descriptions.run(argc,argv);
	}
};
