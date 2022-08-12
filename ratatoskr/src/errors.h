#include <stdexcept>

class MissingParameter : public std::runtime_error {
public:
	MissingParameter(std::string parametername) : runtime_error{"required parameter not indicated: "s+parametername} {}
};
