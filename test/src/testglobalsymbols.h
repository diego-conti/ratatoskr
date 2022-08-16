#include <cxxtest/TestSuite.h>
#include "test.h"

#include "parameters/parameters.h"
#include "conversions/conversions.h"

using namespace GiNaC;
using namespace Wedge;
using namespace ratatoskr;


struct ExParameter {
	GlobalSymbols symbols;
	ex x,y;
};

auto description_ex_no_symbols = make_parameter_description<ExParameter> (
		"x", "a ginac expression without symbols", expression(&ExParameter::x),
		"y", "a ginac expression without symbols", expression(&ExParameter::y)
);
auto description_ex_with_symbols = make_parameter_description<ExParameter> (
		"symbol", "a predefined symbol", expression(&ExParameter::x,&ExParameter::symbols),
		"expression", "a ginac expression with symbol", expression(&ExParameter::y,&ExParameter::symbols)
);

struct CommandLineParameters {
	GlobalSymbols symbols;
	unique_ptr<LieGroup> G;
};
auto description=make_parameter_description<CommandLineParameters>
(
		"lie-algebra","Lie algebra with parameters",lie_algebra(&CommandLineParameters::G,&CommandLineParameters::symbols)
);

class GlobalSymbolsTestSuite : public CxxTest::TestSuite
{
public:

	void testGlobalSymbols() {
		const char* (argv[]) {"program invocation", "--lie-algebra=0,0,[a]*12,[phi]*13"};
		int argc=std::size(argv);
		auto parameters=description.parametersFromCommandLine(argc,argv);
		stringstream s;
		parameters.G->canonical_print(s);
		TS_ASSERT_EQUALS(s.str(),"(0,0,a*(e1*e2),phi*(e1*e3))");
	}
	void testex() {
		const char* (argv[]) {"program invocation", "--x=sqrt(13)+2", "--y=4"};
		int argc=std::size(argv);
		auto parameters=description_ex_no_symbols.parametersFromCommandLine(argc,argv);
		TS_ASSERT_EQUALS(parameters.x,sqrt(ex(13))+2);
		TS_ASSERT_EQUALS(parameters.y,4);
	}
	void testexsymbols() {
		const char* (argv[]) {"program invocation", "--symbol=a", "--expression=sqrt(13)+a"};
		int argc=std::size(argv);
		auto parameters=description_ex_with_symbols.parametersFromCommandLine(argc,argv);
		TS_ASSERT_EQUALS(parameters.y,parameters.x+sqrt(ex(13)));
	}
};
