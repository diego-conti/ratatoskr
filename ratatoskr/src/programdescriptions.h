#include "commandlineparameters.h"

template<typename Parameters, typename Program>
class ProgramDescription {
	string command;
	DescriptionOfCommandLineParameters<Parameters> parameterDescription;
	Program program;
public:
	ProgramDescription(const string& command, const DescriptionOfCommandLineParameters<Parameters>& desc, const Program& program)
		: command{command}, parameterDescription{desc}, program{program} {}
	bool match(const string& command) const {
		return this->command==command;
	}
	const DescriptionOfCommandLineParameters<Parameters>& description() {
		return parameterDescription;
	}
	void run(int argc, const char** argv) const {
		Parameters parameters=parameterDescription.parametersFromCommandLine(argc,argv);
		program(parameters);
	}
};

auto tuple_of_alternative_program_descriptions() {
	return make_tuple();
}

template<typename Parameters, typename Program, typename... T>
auto tuple_of_alternative_program_descriptions(const string& command, DescriptionOfCommandLineParameters<Parameters> parameterDescription, Program program, T... otherPrograms) {
	ProgramDescription programDescription(command,parameterDescription,program);
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
