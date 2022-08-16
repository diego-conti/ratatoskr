#ifndef RATATOSKR_LIE_ALGEBRAS_H
#define RATATOSKR_LIE_ALGEBRAS_H
#include "parameters/dependentparameters.h"
#include "globalsymbols.h"
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
}
#endif
