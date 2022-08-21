/*******************************************************************************
 *  Copyright (C) 2022 by Diego Conti 
 *  This file is part of Ratatoskr.                          
 *  Ratatoskr is free software; you can redistribute it and/or modify         
 *  it under the terms of the GNU General Public License as published by  
 *  the Free Software Foundation; either version 3 of the License, or     
 *  (at your option) any later version.                                   
 *                                                                          
 *  Ratatoskr is distributed in the hope that it will be useful,              
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of        
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         
 *  GNU General Public License for more details.                          
 *                                                                           
 *  You should have received a copy of the GNU General Public License     
 *  along with Wedge; if not, write to the                                
 *   Free Software Foundation, Inc.,                                       
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             
 *  
 *******************************************************************************/
#include "output/twocolumnoutput.h"

namespace ratatoskr {

inline po::options_description output_options() {
	po::options_description options;
	options.add_options()("latex","latex output");
	options.add_options()("silent","no output");
	return options;
}

inline ostream& output_stream(int argc, const char** argv) {
	po::variables_map vm;
	po::store(po::command_line_parser(argc, argv).options(output_options()).allow_unregistered().run(), vm);
	po::notify(vm);
	static stringstream dev_null;
	if (vm.count("silent")) return dev_null;
	if (vm.count("latex")) cout<<latex;
	return cout;
}

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
	void run(int argc, const char** argv) const {
		try {
			auto parameters=parameterDescription.parametersFromCommandLine(argc,argv);
			program(parameters,output_stream(argc,argv));
		}
		catch (const CommandLineError& error) {
			cerr<<command_<<": "<<program_purpose_<<endl;
			cerr<<error.what()<<endl;
			cerr<<parameterDescription.human_readable_description();
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
	if (command.find(' ')!=string::npos) throw DefinitionError("Command name "+command+ " should not contain whitespace");
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
		commands<<"Allowed commands:"<<endl;
		auto add_to_command_description = [&commands] (auto& desc) {
			commands<<two_column_output(desc.command(),desc.purpose(),1);
		};
		iterate_over_tuple(add_to_command_description,programDescriptions);
		commands<<endl;
		commands<<"Global options:"<<endl;
		commands<<output_options();
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
		else if (argc==2) {	//this is to ensure that invoking the program with the only option of "--silent" results in no output, for consistency
			output_stream(argc,argv)<<command_description();
			return false;
		}
		else {
			cerr<<command_description();
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
