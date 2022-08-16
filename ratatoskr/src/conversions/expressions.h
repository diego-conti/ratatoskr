#ifndef RATATOSKR_EXPRESSIONS_H
#define RATATOSKR_EXPRESSIONS_H
#include "input/exfromstring.h"
namespace ratatoskr {
using namespace GiNaC;
using namespace Wedge;

template<typename Parameters,  typename GroupType>
auto differential_form(ex Parameters::*p,unique_ptr<GroupType> Parameters::*G) {
	auto converter=[] (const string& parameter, unique_ptr<LieGroup>& G) {
		return ParseDifferentialForm(G->e(),parameter.c_str());
	};
	return generic_converter(p,converter,G);
}

template<typename Parameters,  typename GroupType>
auto differential_form(ex Parameters::*p,unique_ptr<GroupType> Parameters::*G,GlobalSymbols Parameters::*symbols) {
	auto converter=[] (const string& parameter, unique_ptr<LieGroup>& G, const GlobalSymbols& global_symbols) {
		return ParseDifferentialForm(G->e(),parameter.c_str(),global_symbols.symbols);
	};
	return generic_converter(p,converter,G);
}

template<typename Parameters>
auto expression(ex Parameters::*p) {
	auto converter=[] (const string& parameter) {
		return ex{parameter,lst{}};
	};
	return generic_converter(p,converter);
}

template<typename Parameters>
auto expression(ex Parameters::*p, ex Parameters::*symbols) {
	auto converter=[] (const string& parameter, ex symbols) {
		return ex_from_string(parameter,symbols);
	};
	return generic_converter(p,converter,symbols);
}

template<typename Parameters>
auto expression(ex Parameters::*p, GlobalSymbols Parameters::*symbols) {
	auto converter=[] (const string& parameter, const GlobalSymbols& symbols) {
		return ex{parameter,symbols.symbols};
	};
	return generic_converter(p,converter,symbols);
}
}
#endif
