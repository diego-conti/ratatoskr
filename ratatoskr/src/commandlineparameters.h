#include "wedge/wedge.h"
#include "boost/program_options.hpp"

using std::string;

using namespace Wedge;

namespace po = boost::program_options;

template<typename Parameters>
struct CommandLineParameterDescription_impl {
	virtual void fill(Parameters& structure, const po::variable_value& command_line_variable) const=0;
	virtual const po::value_semantic* value() const {return po::value<string>();}
	virtual unique_ptr<CommandLineParameterDescription_impl> duplicate() const=0;
	virtual ~CommandLineParameterDescription_impl() {};
};

template<typename Parameters>
class CommandLineParameterDescription final {
	unique_ptr<CommandLineParameterDescription_impl<Parameters>> pimpl;
	string name_;
	string description_;
public:
	CommandLineParameterDescription(string name, string description,unique_ptr<CommandLineParameterDescription_impl<Parameters>>&& pimpl)
		: name_{name},description_{description}, pimpl{move(pimpl)} {}
	CommandLineParameterDescription(const CommandLineParameterDescription& o) {
			pimpl=o.pimpl->duplicate();
			name_=o.name_;
			description_=o.description_;
	}
	CommandLineParameterDescription(CommandLineParameterDescription&&)=default;
	CommandLineParameterDescription& operator=(const CommandLineParameterDescription& o) {
		pimpl=o.pimpl->duplicate();
		name_=o.name_;
		description_=o.description_;
	}
	CommandLineParameterDescription& operator=(CommandLineParameterDescription&&)=default;
	string name() const {return name_;}
	string description() const {return description_;}
	void fill(Parameters& structure, const po::variables_map& command_line_variable_map) const {
		pimpl->fill(structure,command_line_variable_map[name()]);
	}
	void add_option_description(po::options_description& options) const {
		options.add_options()(name_.c_str(), pimpl->value(),description_.c_str());
	}
};

template<typename Parameters, typename ParameterType, ParameterType Parameters::*parameter>
struct StringCommandLineParameterDescription : public CommandLineParameterDescription_impl<Parameters> {
	void fill(Parameters& structure, const po::variable_value& command_line_variable) const override {
		structure.*parameter=command_line_variable.as<string>();
	}
	unique_ptr<CommandLineParameterDescription_impl<Parameters>> duplicate() const override {
		return make_unique<StringCommandLineParameterDescription>();
	}
};


template<typename Parameters>
class DescriptionOfCommandLineParameters {
	vector<CommandLineParameterDescription<Parameters>> parameter_descriptions;
	po::options_description description() const {
		po::options_description options("Allowed options");
		for (auto& desc : parameter_descriptions)
			desc.add_option_description(options);
		return options;
	}
	Parameters parametersFromVariableMap(const po::variables_map& vm) {
		Parameters params;
		for (auto& desc : parameter_descriptions)
			desc.fill(params,vm);
		return params;
	}
public:
	Parameters parametersFromCommandLine(int argc, const char** argv) {
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
	template<typename ParameterType, ParameterType Parameters::*parameter>
	void addStringCommandLineParameterDescription(string name, string description) {
		using Type = StringCommandLineParameterDescription<Parameters,ParameterType,parameter>;
		parameter_descriptions.emplace_back(name,description,make_unique<Type>());
	}
};


