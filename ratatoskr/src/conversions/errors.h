#ifndef CONVERSION_ERRORS_H
#define CONVERSION_ERRORS_H

#include <stdexcept>
namespace ratatoskr {

class ConversionError : public CommandLineError {
public:
	ConversionError(const std::string& error) : CommandLineError{"error parsing parameter: "s+error} {}
};

}
#endif
