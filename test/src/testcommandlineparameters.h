/*******************************************************************************
 *  Copyright (C) 2022 by Diego Conti diego.conti@unipi.it 
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
#include <cxxtest/TestSuite.h>
#include "test.h"

#include "parameters/parameters.h"

using namespace GiNaC;
using namespace Wedge;
using namespace ratatoskr;


struct CommandLineParameters {
	string string_parameter1;
	string string_parameter2;
	int int_parameter1;
	int int_parameter2;
	bool bool_parameter1;
	bool bool_parameter2;
};

auto description_strings=make_parameter_description
(
		"param1","first parameter",&CommandLineParameters::string_parameter1,
		"param2","second parameter",&CommandLineParameters::string_parameter2
);
auto description_integers=make_parameter_description
(
		"param1","first parameter",&CommandLineParameters::int_parameter1,
		"param2","second parameter",&CommandLineParameters::int_parameter2
);
auto description_booleans=make_parameter_description
(
		"param1","first parameter",&CommandLineParameters::bool_parameter1,
		"param2","second parameter",&CommandLineParameters::bool_parameter2
);

class CommandLineParametersTestSuite : public CxxTest::TestSuite
{
public:
	void testDescription() {
		string s=description_strings.human_readable_description();
		TS_ASSERT_DIFFERS(s.find("--param1 arg"),string::npos);
		TS_ASSERT_DIFFERS(s.find("first parameter"),string::npos);
		TS_ASSERT_DIFFERS(s.find("--param2 arg"),string::npos);
		TS_ASSERT_DIFFERS(s.find("second parameter"),string::npos);

		TS_ASSERT(s.find("--param1 arg")<s.find("first parameter"));
		TS_ASSERT(s.find("--param2 arg")<s.find("second parameter"));
		TS_ASSERT(s.find("--param1 arg")<s.find("--param2 arg"));

		s=description_integers.human_readable_description();
		TS_ASSERT_DIFFERS(s.find("--param1 arg"),string::npos);
		TS_ASSERT_DIFFERS(s.find("first parameter"),string::npos);
		TS_ASSERT_DIFFERS(s.find("--param2 arg"),string::npos);
		TS_ASSERT_DIFFERS(s.find("second parameter"),string::npos);

		TS_ASSERT(s.find("--param1 arg")<s.find("first parameter"));
		TS_ASSERT(s.find("--param2 arg")<s.find("second parameter"));
		TS_ASSERT(s.find("--param1 arg")<s.find("--param2 arg"));

		s=description_booleans.human_readable_description();
		TS_ASSERT_DIFFERS(s.find("--param1 arg"),string::npos);
		TS_ASSERT_DIFFERS(s.find("first parameter"),string::npos);
		TS_ASSERT_DIFFERS(s.find("--param2 arg"),string::npos);
		TS_ASSERT_DIFFERS(s.find("second parameter"),string::npos);

		TS_ASSERT(s.find("--param1 arg")<s.find("first parameter"));
		TS_ASSERT(s.find("--param2 arg")<s.find("second parameter"));
		TS_ASSERT(s.find("--param1 arg")<s.find("--param2 arg"));
}

	void testParseStringsWithEquals() {
		const char* (argv[]) {"program invocation", "--param1=number1", "--param2=number2"};
		int argc=std::size(argv);
		auto parameters=description_strings.parametersFromCommandLine(argc,argv);
		TS_ASSERT_EQUALS(parameters.string_parameter1,"number1");
		TS_ASSERT_EQUALS(parameters.string_parameter2,"number2");
	}
	void testParseStringsWithSpaces() {
		const char* (argv[]) {"program invocation", "--param2", "number2", "--param1", "number1"};
		int argc=std::size(argv);
		auto parameters=description_strings.parametersFromCommandLine(argc,argv);
		TS_ASSERT_EQUALS(parameters.string_parameter1,"number1");
		TS_ASSERT_EQUALS(parameters.string_parameter2,"number2");
	}
	void testParseIntegers() {
		const char* (argv[]) {"program invocation", "--param1=1", "--param2=-2"};
		int argc=std::size(argv);
		auto parameters=description_integers.parametersFromCommandLine(argc,argv);
		TS_ASSERT_EQUALS(parameters.int_parameter1,1);
		TS_ASSERT_EQUALS(parameters.int_parameter2,-2);

	}

	void testParseBooleans() {
		const char* (argv[]) {"program invocation", "--param1=true", "--param2=false"};
		int argc=std::size(argv);
		auto parameters=description_booleans.parametersFromCommandLine(argc,argv);
		TS_ASSERT_EQUALS(parameters.bool_parameter1,true);
		TS_ASSERT_EQUALS(parameters.bool_parameter2,false);
	}
	void testThrows() {
		const char* (argv[]) {"program invocation", "--param1=true"};
		int argc=std::size(argv);
		TS_ASSERT_THROWS(description_booleans.parametersFromCommandLine(argc,argv),MissingParameter);

	}
};
