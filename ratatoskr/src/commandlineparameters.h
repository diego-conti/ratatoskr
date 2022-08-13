#include "wedge/wedge.h"
#include "boost/program_options.hpp"
#include "tuplecontainer.h"
#include "errors.h"

using namespace Wedge;

namespace po = boost::program_options;

template<typename Parameters, typename ParameterType>
class CommandLineParameterDescription final {
	string name_;
	string description_;
	ParameterType Parameters::*p_;
public:
	CommandLineParameterDescription(string name, string description, ParameterType Parameters::*p)
		: name_{name}, description_{description}, p_{p}
 {}
	string name() const {return name_;}
	string description() const {return description_;}
	void fill(Parameters& structure, const po::variables_map& command_line_variable_map) const {
		if (!command_line_variable_map.count(name_)) throw MissingParameter(name_);
		auto& variable=command_line_variable_map[name_];
		structure.*p_=variable.as<ParameterType>();
	}
	void add_option_description(po::options_description& options) const {
		options.add_options()(name_.c_str(), po::value<remove_reference_t<ParameterType>>(),description_.c_str());
	}
};


auto tuple_of_parameter_descriptions() {
	return make_tuple();
}

template<typename Parameters, typename ParameterType, typename... T>
auto tuple_of_parameter_descriptions(const string& name, const string& description, ParameterType Parameters::*p,
		T... otherParameters) {
	CommandLineParameterDescription<Parameters,ParameterType> parameter_descriptions(name,description,p);
	return insert_in_tuple(parameter_descriptions,tuple_of_parameter_descriptions(otherParameters...));
}




template<typename Parameters, typename TupleOfParameterDescriptions>
class DescriptionOfCommandLineParameters {
	TupleOfParameterDescriptions parameter_descriptions;
	po::options_description description() const {
		po::options_description options("Allowed options");
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
		po::variables_map vm;
		po::store(po::parse_command_line(argc, argv, description()), vm);
		po::notify(vm);
		return parametersFromVariableMap(vm);
	}
	string human_readable_description() const {
		stringstream s;
		s<<description();
		return s.str();
	}
};

template<typename Parameters, typename... T>
auto make_parameter_description(T... options) {
	auto tuple=tuple_of_parameter_descriptions<Parameters>(options...);
	return DescriptionOfCommandLineParameters<Parameters,decltype(tuple)>(tuple);
}
