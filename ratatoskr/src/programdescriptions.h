#include "commandlineparameters.h"

template<typename DescriptionOfCommandLineParameters, typename Program>
class ProgramDescription {
	string command;
	DescriptionOfCommandLineParameters parameterDescription;
	Program program;
public:
	ProgramDescription(const string& command, const DescriptionOfCommandLineParameters& desc, const Program& program)
		: command{command}, parameterDescription{desc}, program{program} {}
	bool match(const string& command) const {
		return this->command==command;
	}
	const DescriptionOfCommandLineParameters& description() {
		return parameterDescription;
	}
	void run(int argc, const char** argv) const {
		auto parameters=parameterDescription.parametersFromCommandLine(argc,argv);
		program(parameters);
	}
};

template<typename DescriptionOfCommandLineParameters, typename Program>
auto make_program_description(const string& command, const DescriptionOfCommandLineParameters& desc, const Program& program) {
	return ProgramDescription<DescriptionOfCommandLineParameters,Program>(command,desc,program);
}

auto tuple_of_alternative_program_descriptions() {
	return make_tuple();
}

template<typename DescriptionOfCommandLineParameters, typename Program, typename... T>
auto tuple_of_alternative_program_descriptions(const string& command, DescriptionOfCommandLineParameters parameterDescription, Program program, T... otherPrograms) {
	auto programDescription=make_program_description(command,parameterDescription,program);
	return tuple_cat(make_tuple(programDescription),tuple_of_alternative_program_descriptions(otherPrograms...));
}

template<typename TupleOfProgramDescriptionTypes>
class CommandLineProgramDescriptions {
	TupleOfProgramDescriptionTypes programDescriptions;
public:
	CommandLineProgramDescriptions(TupleOfProgramDescriptionTypes programDescriptions)
		: programDescriptions{programDescriptions} {}
	void run(int argc, const char** argv) const {
	}
};

template<typename... T>
auto alternative_program_descriptions(T... programDescriptions) {
	auto tuple=tuple_of_alternative_program_descriptions(programDescriptions...);
	return CommandLineProgramDescriptions<decltype(tuple)>(tuple);
}
