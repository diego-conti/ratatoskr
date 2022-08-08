#include "wedge/wedge.h"
#include "boost/program_options.hpp"

using std::string;

using namespace Wedge;

namespace po = boost::program_options;

class CommandLineParameter {
	string name_;
	string description_;
public:
	CommandLineParameter(string name, string description) : name_{name},description_{description} {}
	string name() const {return name_;}
	string description() const {return description_;}
};

template<typename Parameters>
class TypedCommandLineParameter : public CommandLineParameter {
public:
	using CommandLineParameter::CommandLineParameter;
	virtual void fill(Parameters& structure, const po::variables_map& command_line_variable_map)=0;
};


template<typename Parameters, typename ParameterType, ParameterType Parameters::*parameter>
class StringCommandLineParameter : public TypedCommandLineParameter<Parameters> {
public:
	using TypedCommandLineParameter<Parameters>::TypedCommandLineParameter;
	virtual void fill(Parameters& structure, const po::variables_map& command_line_variable_map) override {
		structure.*parameter=command_line_variable_map[CommandLineParameter::name()].as<string>();
	}
};


template<typename Parameters, typename Description> Parameters parametersFromCommandLine(const Description& description, const po::variables_map& vm) {
	Parameters params;
	for (auto& desc : description)
		desc.fill(params,vm);
	return params;
}

struct CommandLineParameters {
	string liealgebra;
	bool latex_output;
	bool dflt_output;
};

auto CommandLineDescription = {
	StringCommandLineParameter<CommandLineParameters,std::string,&CommandLineParameters::liealgebra>{"lie-algebra","lie algebra"}
};


