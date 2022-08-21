#ifndef GENERIC_H
#define GENERIC_H
#include "input/pairfrom.h"
namespace ratatoskr {
using namespace GiNaC;
using namespace Wedge;

template<typename Container>
matrix generic_diagonal_matrix(unsigned int dimension, Container& store_parameters) {
	matrix g{dimension, dimension};
	for (unsigned int i=0;i<dimension;++i)
		store_parameters.append(g(i,i)=Parameter(N.g(i+1)));
	return g;
}

template<typename Parameters, typename GroupType>
auto generic_diagonal_metric(unique_ptr<PseudoRiemannianStructure> Parameters::*p, unique_ptr<GroupType> Parameters::*G,lst Parameters::*metric_parameters) {
	auto converter=[] (unique_ptr<LieGroup>& G,lst& metric_parameters) {
		unsigned int dimension=G->Dimension();
		auto g=generic_diagonal_matrix(dimension,metric_parameters);
		return make_unique<PseudoRiemannianStructureByMatrix>(G.get(),G->e(),g.inverse());
	};
	return generic_option(p,converter,G,metric_parameters);
}

template<typename Container>
matrix generic_symmetric_matrix_from_list_of_nonzero_entries(int n, const vector<pair<int,int>>& indices, Container& store_parameters) {
	matrix m(n,n);
	for (auto p: indices) {
		ex newparameter=Parameter{N.a(p.first*10+p.second)};
		m(p.second-1,p.first-1)= m(p.first-1,p.second-1)=newparameter;
		Insert(store_parameters,newparameter);
	}
	return m;
}


template<typename Parameters, typename GroupType>
auto generic_metric_from_nonzero_entries(unique_ptr<PseudoRiemannianStructure> Parameters::*p, unique_ptr<GroupType> Parameters::*G,lst Parameters::*metric_parameters) {
	auto converter=[] (const vector<string>& nonzero_entries, unique_ptr<LieGroup>& G,lst& metric_parameters) {
		unsigned int dimension=G->Dimension();
		vector<pair<int,int>> nonzero_indices;
		transform(nonzero_entries.begin(),nonzero_entries.end(),back_inserter(nonzero_indices),pair_from_csv<int,int>);
		auto g=generic_symmetric_matrix_from_list_of_nonzero_entries(G->Dimension(),nonzero_indices,metric_parameters);
		return make_unique<PseudoRiemannianStructureByMatrix>(G.get(),G->e(),g.inverse());
	};
	return generic_converter<vector<string>>(p,converter,G,metric_parameters);
}

}
#endif
