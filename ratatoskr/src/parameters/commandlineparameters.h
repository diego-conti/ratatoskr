#ifndef COMMAND_LINE_PARAMETERS_H
#define COMMAND_LINE_PARAMETERS_H
#include "tuplecontainer.h"
#include "errors.h"

namespace po = boost::program_options;
namespace ratatoskr {
template<typename Parameters, typename TupleOfParameterDescriptions>
class DescriptionOfCommandLineParameters {
	TupleOfParameterDescriptions parameter_descriptions;
	po::options_description description() const {
		po::options_description options("Usage");
		auto add_to_options_description = [&options] (auto& desc) {
			desc.add_option_description(options);
		};

		iterate_over_tuple(add_to_options_description,parameter_descriptions);
		return options;
	}

	Parameters parametersFromVariableMap(const po::variables_map& vm) const {
		Parameters params;
		auto fill_parameter = [&vm,&params] (auto& desc) {
			desc.fill(params,vm);
		};
		iterate_over_tuple(fill_parameter,parameter_descriptions);
		return params;
	}
public:
	DescriptionOfCommandLineParameters(TupleOfParameterDescriptions parameter_descriptions) : parameter_descriptions{parameter_descriptions}{}
	Parameters parametersFromCommandLine(int argc, const char** argv) const {
		try {
			po::variables_map vm;
			po::store(po::parse_command_line(argc, argv, description()), vm);
			po::notify(vm);
			return parametersFromVariableMap(vm);
		}
		catch (const po::error& e) {
			throw BoostError(e.what());
		}
	}
	string human_readable_description() const {
		stringstream s;
		s<<description();
		return s.str();
	}
};

}
#endif