namespace ratatoskr {

template<typename DescriptionOfCommandLineParameters, typename Program>
class ProgramDescription {
	string command_, program_purpose_;
	DescriptionOfCommandLineParameters parameterDescription;
	Program program;
	static po::options_description output_options() {
		auto options=empty_options_description();
		options.add_options()("latex","latex output");
		return options;
	}
	ostream& output_stream(int argc, const char** argv) const {
		po::variables_map vm;
		po::store(po::command_line_parser(argc, argv).options(output_options()).allow_unregistered().run(), vm);
		po::notify(vm);
		if (vm.count("latex")) cout<<latex;
		return cout;
	}
public:
	ProgramDescription(const string& command, const string& program_purpose, const DescriptionOfCommandLineParameters& desc, const Program& program)
		: command_{command}, program_purpose_{program_purpose}, parameterDescription{desc}, program{program} {}
	bool match(const string& command) const {
		return command_==command;
	}
	const DescriptionOfCommandLineParameters& description() const {
		return parameterDescription;
	}
	void run(int argc, const char** argv) const {
		try {
			auto parameters=parameterDescription.parametersFromCommandLine(argc,argv,output_options());
			auto& output=output_stream(argc,argv);
			program(parameters,output);
		}
		catch (const CommandLineError& error) {
			cerr<<error.what()<<endl;
			cerr<<description().human_readable_description();
		}
	}
	void run(int argc, char** argv) const {
		run(argc,const_cast<const char**>(argv));
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
auto tuple_of_alternative_program_descriptions(ProgramDescription<DescriptionOfCommandLineParameters,Program>&& program_description, T... otherPrograms) {
	return insert_in_tuple(std::move(program_description),tuple_of_alternative_program_descriptions(otherPrograms...));
}

template<typename DescriptionOfCommandLineParameters, typename Program, typename... T>
auto tuple_of_alternative_program_descriptions(const ProgramDescription<DescriptionOfCommandLineParameters,Program>& program_description, T... otherPrograms) {
	return insert_in_tuple(program_description,tuple_of_alternative_program_descriptions(otherPrograms...));
}

template<typename DescriptionOfCommandLineParameters, typename Program, typename... T>
auto tuple_of_alternative_program_descriptions(const string& command, const string& program_purpose,
		DescriptionOfCommandLineParameters parameterDescription, Program program, T... otherPrograms) {
	auto programDescription=make_program_description(command,program_purpose,parameterDescription,program);
	return insert_in_tuple(programDescription,tuple_of_alternative_program_descriptions(otherPrograms...));
}


template<typename TupleOfProgramDescriptionTypes>
class CommandLineProgramDescriptions {
	TupleOfProgramDescriptionTypes programDescriptions;
	string command_description() const {
		stringstream commands;
		commands<<"Allowed commands"<<endl;
		auto add_to_command_description = [&commands] (auto& desc) {
			commands<<desc.command()<<'\t'<<desc.purpose()<<endl;
		};
		iterate_over_tuple(add_to_command_description,programDescriptions);
		return commands.str();
	}
	bool run_matching_program(const string& command, int argc, const char** argv) const {
		bool ran=false;
		auto run_if_matches= [&command,argc,argv,&ran](auto& program_description) {
			if (program_description.match(command)) {
				assert(!ran);
				program_description.run(argc-1,argv+1);
				ran=true;
			}
		};
		iterate_over_tuple(run_if_matches,programDescriptions);
		return ran;
	}
public:
	CommandLineProgramDescriptions(TupleOfProgramDescriptionTypes programDescriptions)
		: programDescriptions{programDescriptions} {}
	//returns true if a program was run
	bool run(int argc, const char** argv) const {
		if (argc>=2 && run_matching_program(argv[1],argc,argv))
			return true;
		else {
			cout<<command_description();
			return false;
		}
	}
	bool run(int argc, char** argv) const {
		return run(argc,const_cast<const char**>(argv));
	}
};

template<typename... T>
auto alternative_program_descriptions(T... programDescriptions) {
	auto tuple=tuple_of_alternative_program_descriptions(programDescriptions...);
	return CommandLineProgramDescriptions<decltype(tuple)>(tuple);
}
}
