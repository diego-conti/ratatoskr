#include "commandlineparameters.h"

template<typename DescriptionOfCommandLineParameters, typename Program>
class ProgramDescription {
	string command_, program_purpose_;
	DescriptionOfCommandLineParameters parameterDescription;
	Program program;
public:
	ProgramDescription(const string& command, const string& program_purpose, const DescriptionOfCommandLineParameters& desc, const Program& program)
		: command_{command}, program_purpose_{program_purpose}, parameterDescription{desc}, program{program} {}
	bool match(const string& command) const {
		return command_==command;
	}
	const DescriptionOfCommandLineParameters& description() {
		return parameterDescription;
	}
	void run(int argc, const char** argv) const {
		auto parameters=parameterDescription.parametersFromCommandLine(argc,argv);
		program(parameters);
	}
	const char* command() const {return command_.c_str();}
	const char* purpose() const {return program_purpose_.c_str();}
};

template<typename DescriptionOfCommandLineParameters, typename Program>
auto make_program_description(const string& command, const string& program_purpose,
		const DescriptionOfCommandLineParameters& desc, const Program& program) {
	return ProgramDescription<DescriptionOfCommandLineParameters,Program>(command,program_purpose,desc,program);
}

auto tuple_of_alternative_program_descriptions() {
	return make_tuple();
}

template<typename DescriptionOfCommandLineParameters, typename Program, typename... T>
auto tuple_of_alternative_program_descriptions(const string& command, const string& program_purpose,
		DescriptionOfCommandLineParameters parameterDescription, Program program, T... otherPrograms) {
	auto programDescription=make_program_description(command,program_purpose,parameterDescription,program);
	return tuple_cat(make_tuple(programDescription),tuple_of_alternative_program_descriptions(otherPrograms...));
}

template<typename TupleOfProgramDescriptionTypes>
class CommandLineProgramDescriptions {
	TupleOfProgramDescriptionTypes programDescriptions;
	po::options_description command_description() const {
		po::options_description commands("Allowed commands");
		auto add_to_options_description = [&commands] (auto& desc) {
			commands.add_options()(desc.command(),desc.purpose());
		};
		iterate_over_tuple(add_to_options_description,programDescriptions);
		return commands;
	}

public:
	CommandLineProgramDescriptions(TupleOfProgramDescriptionTypes programDescriptions)
		: programDescriptions{programDescriptions} {}
	void run(int argc, const char** argv) const {
		auto desc=command_description();
		cout<<desc<<endl;
	}
};

template<typename... T>
auto alternative_program_descriptions(T... programDescriptions) {
	auto tuple=tuple_of_alternative_program_descriptions(programDescriptions...);
	return CommandLineProgramDescriptions<decltype(tuple)>(tuple);
}
