#ifndef EX_FROM_STRING_H
#define EX_FROM_STRING_H
ex ex_from_string(const string& x, ex symbols) {
	if (is_a<lst>(symbols)) return ex{x,symbols};
	else return ex{x,lst{symbols}};
}
#endif
