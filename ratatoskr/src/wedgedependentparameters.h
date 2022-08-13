#include "dependentparameters.h"

template<typename Parameters, typename ParameterType>
auto lie_algebra(unique_ptr<ParameterType> Parameters::*p) {
	auto converter=[] (const string& parameter) {
		return make_unique<AbstractLieGroup<false>>(parameter.c_str());
	};
	return generic_converter(p,converter);
}

matrix metric_from_deflats(const LieGroup& G, const exvector& deflat) {
	matrix g(G.Dimension(),G.Dimension());
	for (int i=0;i<G.Dimension();++i)
	for (int j=0;j<G.Dimension();++j)
		g(i,j)=Hook(G.e()[i],deflat[j]);
	return g;
}

template<typename Parameters, typename ParameterType, typename GroupType>
auto pseudo_riemannian_metric(unique_ptr<ParameterType> Parameters::*p,unique_ptr<GroupType> Parameters::*G) {
	auto converter=[] (const string& parameter, unique_ptr<LieGroup>& G) {
		auto deflat=ParseDifferentialForms(G->e(),parameter.c_str());
		return make_unique<PseudoRiemannianStructureByMatrix>(G.get(),G->e(),metric_from_deflats(*G,deflat).inverse());
	};
	return generic_converter(p,converter,G);
}

template<typename Parameters,  typename GroupType>
auto differential_form(ex Parameters::*p,unique_ptr<GroupType> Parameters::*G) {
	auto converter=[] (const string& parameter, unique_ptr<LieGroup>& G) {
		return ParseDifferentialForm(G->e(),parameter.c_str());
	};
	return generic_converter(p,converter,G);
}
