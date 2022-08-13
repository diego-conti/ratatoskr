#include <stdexcept>

class MissingParameter : public std::runtime_error {
public:
	MissingParameter(std::string parametername) : runtime_error{"required parameter not indicated: "s+parametername} {}
};

class InvalidParameter : public std::runtime_error {
public:
	InvalidParameter(std::string error) : runtime_error{"invalid parameter value: "s+error} {}
};

class ParseError : public std::runtime_error {
public:
	ParseError(std::string error) : runtime_error{"error parsing parameter: "s+error} {}
};
