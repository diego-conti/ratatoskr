#include <cxxtest/TestSuite.h>
#include "test.h"

#include "commandlineparameters.h"

using namespace GiNaC;
using namespace Wedge;


struct CommandLineParameters {
	string string_parameter1;
	string string_parameter2;
	int int_parameter1;
	int int_parameter2;
	bool bool_parameter1;
	bool bool_parameter2;
};


class CommandLineTestSuite : public CxxTest::TestSuite
{
	DescriptionOfCommandLineParameters<CommandLineParameters> description_strings;
	DescriptionOfCommandLineParameters<CommandLineParameters> description_integers;
	DescriptionOfCommandLineParameters<CommandLineParameters> description_booleans;
public:
	CommandLineTestSuite() {
		description_strings.addStringCommandLineParameterDescription<std::string,&CommandLineParameters::string_parameter1>("param1","first parameter");
		description_strings.addStringCommandLineParameterDescription<std::string,&CommandLineParameters::string_parameter2>("param2","second parameter");

		description_integers.addIntegerCommandLineParameterDescription<int,&CommandLineParameters::int_parameter1>("param1","first parameter");
		description_integers.addIntegerCommandLineParameterDescription<int,&CommandLineParameters::int_parameter2>("param2","second parameter");

		description_booleans.addBooleanCommandLineParameterDescription<bool,&CommandLineParameters::bool_parameter1>("param1","first parameter");
		description_booleans.addBooleanCommandLineParameterDescription<bool,&CommandLineParameters::bool_parameter2>("param2","second parameter");
	}
	void testDescription() {
		string s=description_strings.human_readable_description();
		TS_ASSERT_DIFFERS(s.find("--param1 arg"),string::npos);
		TS_ASSERT_DIFFERS(s.find("first parameter"),string::npos);
		TS_ASSERT_DIFFERS(s.find("--param2 arg"),string::npos);
		TS_ASSERT_DIFFERS(s.find("second parameter"),string::npos);

		TS_ASSERT(s.find("--param1 arg")<s.find("first parameter"));
		TS_ASSERT(s.find("--param2 arg")<s.find("second parameter"));
		TS_ASSERT(s.find("--param1 arg")<s.find("--param2 argr"));

		s=description_integers.human_readable_description();
		TS_ASSERT_DIFFERS(s.find("--param1 arg"),string::npos);
		TS_ASSERT_DIFFERS(s.find("first parameter"),string::npos);
		TS_ASSERT_DIFFERS(s.find("--param2 arg"),string::npos);
		TS_ASSERT_DIFFERS(s.find("second parameter"),string::npos);

		TS_ASSERT(s.find("--param1 arg")<s.find("first parameter"));
		TS_ASSERT(s.find("--param2 arg")<s.find("second parameter"));
		TS_ASSERT(s.find("--param1 arg")<s.find("--param2 argr"));

		s=description_booleans.human_readable_description();
		TS_ASSERT_DIFFERS(s.find("--param1 arg"),string::npos);
		TS_ASSERT_DIFFERS(s.find("first parameter"),string::npos);
		TS_ASSERT_DIFFERS(s.find("--param2 arg"),string::npos);
		TS_ASSERT_DIFFERS(s.find("second parameter"),string::npos);

		TS_ASSERT(s.find("--param1 arg")<s.find("first parameter"));
		TS_ASSERT(s.find("--param2 arg")<s.find("second parameter"));
		TS_ASSERT(s.find("--param1 arg")<s.find("--param2 argr"));
}

	void testParseStringsWithEquals() {
		const char* (argv[]) {"program invocation", "--param1=number1", "--param2=number2"};
		int argc=std::size(argv);
		auto parameters=description_strings.parametersFromCommandLine(argc,argv);
		TS_ASSERT_EQUALS(parameters.string_parameter1,"number1");
		TS_ASSERT_EQUALS(parameters.string_parameter2,"number2");
	}
	void testParseStringsWithSpaces() {
		const char* (argv[]) {"program invocation", "--param2", "number2", "--param1", "number1"};
		int argc=std::size(argv);
		auto parameters=description_strings.parametersFromCommandLine(argc,argv);
		TS_ASSERT_EQUALS(parameters.string_parameter1,"number1");
		TS_ASSERT_EQUALS(parameters.string_parameter2,"number2");
	}
	void testParseIntegers() {
		const char* (argv[]) {"program invocation", "--param1=1", "--param2=-2"};
		int argc=std::size(argv);
		auto parameters=description_integers.parametersFromCommandLine(argc,argv);
		TS_ASSERT_EQUALS(parameters.int_parameter1,1);
		TS_ASSERT_EQUALS(parameters.int_parameter2,-2);

	}

	void testParseBooleans() {
		const char* (argv[]) {"program invocation", "--param1=true", "--param2=false"};
		int argc=std::size(argv);
		auto parameters=description_booleans.parametersFromCommandLine(argc,argv);
		TS_ASSERT_EQUALS(parameters.bool_parameter1,true);
		TS_ASSERT_EQUALS(parameters.bool_parameter2,false);

	}
};
