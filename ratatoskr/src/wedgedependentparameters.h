#include "dependentparameters.h"
namespace ratatoskr {
using namespace GiNaC;
using namespace Wedge;

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

matrix diagonal_matrix_from_strings(const vector<string>& s, const lst& symbols) {
	matrix r{s.size(),s.size()};
	for (int i=0;i<s.size();++i) {
		try {
			r(i,i)=ex{s[i],symbols};
		}
		catch (...) {
			throw ParseError(s[i]);
		}
	}
	return r;
}

matrix diagonal_matrix_from_string(const string& s, const lst& symbols) {
	stringstream stream{s};
	vector<string> c;
	std::copy(std::istream_iterator<std::string>(stream),std::istream_iterator<std::string>(),std::back_inserter(c));
	return diagonal_matrix_from_strings(c,symbols);
}

template<typename Parameters, typename ParameterType, typename GroupType>
auto diagonal_pseudo_riemannian_metric(unique_ptr<ParameterType> Parameters::*p,unique_ptr<GroupType> Parameters::*G) {
	auto converter=[] (const string& parameter, unique_ptr<LieGroup>& G) {
		lst symbols;
		auto matrix=diagonal_matrix_from_string(parameter,symbols);
		if (matrix.rows()!=G->Dimension())
			throw InvalidParameter("trying to construct diagonal matrix of order "s+to_string(G->Dimension())+" but "+to_string(matrix.rows())+ " entries were specified");
		return make_unique<PseudoRiemannianStructureByMatrix>(G.get(),G->e(),matrix.inverse());
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
}
