#ifndef RATATOSKR_LIE_ALGEBRAS_H
#define RATATOSKR_LIE_ALGEBRAS_H
namespace ratatoskr {
using namespace GiNaC;
using namespace Wedge;

template<typename Parameters, typename ParameterType>
auto lie_algebra(unique_ptr<ParameterType> Parameters::*p) {
	auto converter=[] (const string& parameter) {
		return make_unique<AbstractLieGroup<false>>(parameter);
	};
	return generic_converter(p,converter);
}
template<typename Parameters, typename ParameterType>
auto lie_algebra(unique_ptr<ParameterType> Parameters::*p, GlobalSymbols Parameters::*symbols) {
	auto converter=[] (const string& parameter, const GlobalSymbols& symbols) {
		return make_unique<AbstractLieGroup<true>>(parameter,symbols.symbols());
	};
	return generic_converter(p,converter,symbols);
}
template<typename Parameters, typename ParameterType>
auto lie_algebra(unique_ptr<ParameterType> Parameters::*p, ex Parameters::*symbols) {
	auto converter=[] (const string& parameter, ex symbols) {
		return make_unique<AbstractLieGroup<true>>(parameter,symbols);
	};
	return generic_converter(p,converter,symbols);
}

unique_ptr<AbstractLieSubgroup<true>>
make_subgroup(const LieGroupHasParameters<true>& G, const exvector& subalgebra) {
	return make_unique<AbstractLieSubgroup<true>>(G,subalgebra);
}
unique_ptr<AbstractLieSubgroup<false>>
make_subgroup(const LieGroupHasParameters<false>& G, const exvector& subalgebra) {
	return make_unique<AbstractLieSubgroup<false>>(G,subalgebra);
}

template<typename Parameters, typename LieSubgroupType, typename LieGroupType>
auto lie_subalgebra(unique_ptr<LieSubgroupType> Parameters::*p, unique_ptr<LieGroupType> Parameters::*G) {
	auto converter=[] (const string& parameter, const unique_ptr<LieGroupType>& G) {
		return make_subgroup(*G,ParseDifferentialForms(G->e(),parameter.c_str()));
	};
	return generic_converter(p,converter,G);
}

template<typename Parameters, typename LieSubgroupType, typename LieGroupType>
auto lie_subalgebra(unique_ptr<LieSubgroupType> Parameters::*p, unique_ptr<LieGroupType> Parameters::*G, GlobalSymbols Parameters::*symbols) {
	auto converter=[] (const string& parameter, const unique_ptr<LieGroupType>& G, const GlobalSymbols& symbols) {
		return make_unique<AbstractLieSubgroup<true>>(*G,ParseDifferentialForms(G->e(),parameter.c_str(),symbols.symbols()));
	};
	return generic_converter(p,converter,G,symbols);
}

}
#endif
