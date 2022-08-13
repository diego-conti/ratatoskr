#include <cxxtest/TestSuite.h>
#include "test.h"

#include "wedgedependentparameters.h"

using namespace GiNaC;
using namespace Wedge;


struct CommandLineParameters {
	unique_ptr<LieGroup> G;
	unique_ptr<PseudoRiemannianStructure> g;
	ex form;
};

auto description_lie_algebra=make_parameter_description<CommandLineParameters>
(
		"lie-algebra","Lie algebra without parameters",lie_algebra(&CommandLineParameters::G)
);

auto description_metric=make_parameter_description<CommandLineParameters>
(
		"lie-algebra","Lie algebra without parameters",lie_algebra(&CommandLineParameters::G),
		"metric", "pseudo-riemannian metric on the Lie algebra", pseudo_riemannian_metric(&CommandLineParameters::g,&CommandLineParameters::G)
);

auto description_form=make_parameter_description<CommandLineParameters>
(
		"lie-algebra","Lie algebra without parameters",lie_algebra(&CommandLineParameters::G),
		"form","differential form",differential_form(&CommandLineParameters::form,&CommandLineParameters::G)
);

class InterpretParametersTestSuite : public CxxTest::TestSuite
{
public:
	void testParseLieAlgebra() {
		const char* (argv[]) {"program invocation", "--lie-algebra=0,0,2*12"};
		int argc=std::size(argv);
		auto parameters=description_lie_algebra.parametersFromCommandLine(argc,argv);
		stringstream s;
		parameters.G->canonical_print(s);
		TS_ASSERT_EQUALS(s.str(),"(0,0,2*(e1*e2))");
	}

	void testParseMetric() {
		const char* (argv[]) {"program invocation", "--lie-algebra=0,0,12", "--metric=3,-2*2,1"};
		int argc=std::size(argv);
		auto parameters=description_metric.parametersFromCommandLine(argc,argv);
		auto& G=*(parameters.G);
		auto& g=parameters.g->ScalarProduct();
		TS_ASSERT_EQUALS(g.Flat(G.e(1)),G.e(3));
		TS_ASSERT_EQUALS(g.Flat(G.e(2)),-2*G.e(2));
		TS_ASSERT_EQUALS(g.Flat(G.e(3)),G.e(1));
	}

	void testForm() {
		const char* (argv[]) {"program invocation", "--lie-algebra=0,0,12", "--form=1+2*3"};
		int argc=std::size(argv);
		auto parameters=description_form.parametersFromCommandLine(argc,argv);
		auto& G=*(parameters.G);
		TS_ASSERT_EQUALS(parameters.form, G.e(1)+2*G.e(3));
	}
};
