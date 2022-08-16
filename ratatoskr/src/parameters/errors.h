#ifndef ERRORS_H
#define ERRORS_H

#include <stdexcept>
namespace ratatoskr {

class CommandLineError  : public std::runtime_error {
public:
	using runtime_error::runtime_error;
};

class BoostError : public CommandLineError {
public:
	BoostError(const char* what) : CommandLineError(what) {}
};

class MissingParameter : public CommandLineError {
public:
	MissingParameter(const std::string& parametername) : CommandLineError{"required parameter not indicated: "s+parametername} {}
};

class TooManyAlternatives : public CommandLineError {
public:
	TooManyAlternatives(const std::string& parametername) : CommandLineError{"parameter indicated more than once: "s+parametername} {}
};

class InvalidParameter : public CommandLineError {
public:
	InvalidParameter(const std::string& error) : CommandLineError{"invalid parameter value: "s+error} {}
};

class ParseError : public CommandLineError {
public:
	ParseError(const std::string& error) : CommandLineError{"error parsing parameter: "s+error} {}
};
}
#endif
