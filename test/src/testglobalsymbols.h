#include <cxxtest/TestSuite.h>
#include "test.h"

#include "parameters/alternativeparameters.h"
#include "conversions/convertedparameters.h"

using namespace GiNaC;
using namespace Wedge;
using namespace ratatoskr;

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
};
