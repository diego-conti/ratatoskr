struct CommandLineParameters {
	string liealgebra;
	bool latex_output;
	bool dflt_output;
};

auto CommandLineDescription = {
	StringCommandLineParameter<CommandLineParameters,string,CommandLineParameters::liealgebra> liealgebra{"lie-algebra","lie algebra"};
};


template<typename Parameters>
class CommandLineParameter {
	string name;
	string description;
public:
	CommandLineParameter(string name, string description) : name{name},description{description} {}
	virtual void fill(Parameters& structure, const po::variables_map&)=0;
};


template<typename Parameters, typename ParameterType, ParameterType Parameters::*parameter>
class StringCommandLineParameter : public CommandLineParameter<Parameters> {
public:
	virtual void fill(Parameters& structure, const po::variables_map&) override {
		structure.*parameters=commandline.vm[name].as<string>();
	}
};


template<typename Parameters, typename Description> parametersFromCommandLine(const Description& description, const po::variables_map& vm) {
	Parameters params;
	for (auto& desc : description)
		desc.fill(params,vm);
	return params;
}
