#include "parameters/dependentparameters.h"
#include "globalsymbols.h"
namespace ratatoskr {
using namespace GiNaC;
using namespace Wedge;

template<typename Parameters, typename GroupType>
auto generic_diagonal_metric(unique_ptr<PseudoRiemannianStructure> Parameters::*p, unique_ptr<GroupType> Parameters::*G,lst Parameters::*metric_parameters) {
	auto converter=[] (unique_ptr<LieGroup>& G,lst& metric_parameters) {
		unsigned int dimension=G->Dimension();
		matrix g{dimension, dimension};
		for (unsigned int i=0;i<G->Dimension();++i)
			metric_parameters.append(g(i,i)=Parameter(N.g(i+1)));
		return make_unique<PseudoRiemannianStructureByMatrix>(G.get(),G->e(),g.inverse());
	};
	return generic_option(p,converter,G,metric_parameters);
}

}
