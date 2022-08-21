#ifndef EX_FROM_STRING_H
#define EX_FROM_STRING_H
#include "splice.h"

namespace ratatoskr {
ex ex_from_string(const string& x, ex symbols) {
	if (is_a<lst>(symbols)) return ex{x,symbols};
	else return ex{x,lst{symbols}};
}

exvector exvector_from_string(const string& v, ex symbols) {
	exvector result;
	for (auto& x: splice(v))
		result.push_back(ex_from_string(x,symbols));
	return result;
}
}
#endif
