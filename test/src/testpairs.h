#include <cxxtest/TestSuite.h>
#include "test.h"

#include "parameters/parameters.h"
#include "conversions/conversions.h"

using namespace GiNaC;
using namespace Wedge;
using namespace ratatoskr;

struct CommandLineParameters {
	pair<int,float> int_and_float;
	pair<string,string> string_and_string;
	pair<int,string> int_and_string;
	pair<string,int> string_and_int;
};

auto description=make_parameter_description<CommandLineParameters>
(
		"int-and-float","int and float", comma_separated_pair(&CommandLineParameters::int_and_float),
		"string-and-string","string and string", comma_separated_pair(&CommandLineParameters::string_and_string),
		"int-and-string","int and string", comma_separated_pair(&CommandLineParameters::int_and_string),
		"string-and-int","string and int", comma_separated_pair(&CommandLineParameters::string_and_int)
);

class PairsTestSuite : public CxxTest::TestSuite
{
public:
	void testPairs() {
		const char* (argv[]) {"program invocation", "--int-and-float=2,2.01", "--string-and-string=abc,def",
			"--int-and-string=-1,def", "--string-and-int=abc,1"
		};
		int argc=std::size(argv);
		auto parameters=description.parametersFromCommandLine(argc,argv);
		TS_ASSERT_EQUALS(parameters.int_and_float,make_pair(2,2.01f));
		TS_ASSERT_EQUALS(parameters.string_and_string,make_pair("abc"s,"def"s));
		TS_ASSERT_EQUALS(parameters.int_and_string,make_pair(-1,"def"s));
		TS_ASSERT_EQUALS(parameters.string_and_int,make_pair("abc"s,1));
	}
};