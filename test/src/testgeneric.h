#include <cxxtest/TestSuite.h>
#include "test.h"

#include "parameters/parameters.h"
#include "conversions/conversions.h"

using namespace GiNaC;
using namespace Wedge;
using namespace ratatoskr;

struct CommandLineParameters {
	GlobalSymbols symbols;
	lst metric_parameters;
	unique_ptr<LieGroup> G;
	unique_ptr<PseudoRiemannianStructure> g;
};

auto description_diagonal=make_parameter_description
(
		"lie-algebra","Lie algebra with parameters",lie_algebra(&CommandLineParameters::G,&CommandLineParameters::symbols),
		"generic-diagonal-metric","generic diagonal metric",generic_diagonal_metric(&CommandLineParameters::g,&CommandLineParameters::G,&CommandLineParameters::metric_parameters)
);

auto description_nonzero_entries=make_parameter_description
(
		"lie-algebra","Lie algebra with parameters",lie_algebra(&CommandLineParameters::G,&CommandLineParameters::symbols),
		"generic-metric-from-nonzero-entries","generic metric from list of nonzero entries",
			generic_metric_from_nonzero_entries(&CommandLineParameters::g,&CommandLineParameters::G,&CommandLineParameters::metric_parameters)
);

class GenericTestSuite : public CxxTest::TestSuite
{
public:
	void testGenericDiagonalMetric() {
		const char* (argv[]) {"program invocation", "--lie-algebra=0,0,[a]*12,[phi]*13", "--generic-diagonal-metric"};
		int argc=std::size(argv);
		auto parameters=description_diagonal.parametersFromCommandLine(argc,argv);
		auto& G=*(parameters.G);
		auto& g=*(parameters.g);
		TS_ASSERT_EQUALS(g.ScalarProduct().Flat(G.e(1)),parameters.metric_parameters.op(0)*G.e(1));
		TS_ASSERT_EQUALS(g.ScalarProduct().Flat(G.e(2)),parameters.metric_parameters.op(1)*G.e(2));
		TS_ASSERT_EQUALS(g.ScalarProduct().Flat(G.e(3)),parameters.metric_parameters.op(2)*G.e(3));
		TS_ASSERT_EQUALS(g.ScalarProduct().Flat(G.e(4)),parameters.metric_parameters.op(3)*G.e(4));
	}
	void testGenericMetricFromNonzeroEntries() {
		const char* (argv[]) {"program invocation", "--lie-algebra=0,0,[a]*12,[phi]*13", "--generic-metric-from-nonzero-entries=1,2", "3,4"};
		int argc=std::size(argv);
		auto parameters=description_nonzero_entries.parametersFromCommandLine(argc,argv);
		auto& G=*(parameters.G);
		auto& g=*(parameters.g);
		TS_ASSERT_EQUALS(parameters.metric_parameters.nops(),2);
		TS_ASSERT_EQUALS(g.ScalarProduct().Flat(G.e(1)),parameters.metric_parameters.op(0)*G.e(2));
		TS_ASSERT_EQUALS(g.ScalarProduct().Flat(G.e(3)),parameters.metric_parameters.op(1)*G.e(4));
	}
};
