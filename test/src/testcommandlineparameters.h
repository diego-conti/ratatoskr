#include <cxxtest/TestSuite.h>
#include "test.h"

#include "commandlineparameters.h"

using namespace GiNaC;
using namespace Wedge;


struct CommandLineParameters {
	string string_parameter1;
	string string_parameter2;
};


class CommandLineTestSuite : public CxxTest::TestSuite
{
	DescriptionOfCommandLineParameters<CommandLineParameters> description;
public:
	CommandLineTestSuite() {
		description.addStringCommandLineParameterDescription<std::string,&CommandLineParameters::string_parameter1>("param1","first parameter");
		description.addStringCommandLineParameterDescription<std::string,&CommandLineParameters::string_parameter2>("param2","second parameter");
	}
	void testDescription() {
		auto desc=description.description();
		stringstream s;
		s<<desc;
		TS_ASSERT_DIFFERS(s.str().find("--param1 arg"),string::npos);
		TS_ASSERT_DIFFERS(s.str().find("first parameter"),string::npos);
		TS_ASSERT_DIFFERS(s.str().find("--param2 arg"),string::npos);
		TS_ASSERT_DIFFERS(s.str().find("second parameter"),string::npos);

		TS_ASSERT(s.str().find("--param1 arg")<s.str().find("first parameter"));
		TS_ASSERT(s.str().find("--param2 arg")<s.str().find("second parameter"));
		TS_ASSERT(s.str().find("--param1 arg")<s.str().find("--param2 argr"));
}

	void testParseStringsWithEquals() {
		const char* (argv[]) {"program invocation", "--param1=number1", "--param2=number2"};
		int argc=std::size(argv);
		auto desc=description.description();

		po::variables_map vm;
		po::store(po::parse_command_line(argc, argv, description.description()), vm);
		po::notify(vm);
		auto parameters=description.parametersFromCommandLine(vm);
		TS_ASSERT_EQUALS(parameters.string_parameter1,"number1");
		TS_ASSERT_EQUALS(parameters.string_parameter2,"number2");
	}
	void testParseStringsWithSpaces() {
		const char* (argv[]) {"program invocation", "--param2", "number2", "--param1", "number1"};
		int argc=std::size(argv);
		auto desc=description.description();

		po::variables_map vm;
		po::store(po::parse_command_line(argc, argv, description.description()), vm);
		po::notify(vm);
		auto parameters=description.parametersFromCommandLine(vm);
		TS_ASSERT_EQUALS(parameters.string_parameter1,"number1");
		TS_ASSERT_EQUALS(parameters.string_parameter2,"number2");
	}
};
