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
#include <cxxtest/TestSuite.h>
#include "test.h"

#include "parameters/parameters.h"
#include "conversions/conversions.h"

using namespace GiNaC;
using namespace Wedge;
using namespace ratatoskr;


struct ExParameter {
	GlobalSymbols symbols;
	ex x,y;
};
auto description_ex_no_symbols = make_parameter_description(
		"x", "a ginac expression without symbols", expression(&ExParameter::x),
		"y", "a ginac expression without symbols", expression(&ExParameter::y)
);
auto description_ex_with_symbols = make_parameter_description (
		"symbol", "a predefined symbol", expression(&ExParameter::x,&ExParameter::symbols),
		"expression", "a ginac expression with symbol", expression(&ExParameter::y,&ExParameter::symbols)
);

struct DoubleGlobalSymbolsParameters {
	GlobalSymbols symbols;
	GlobalSymbols symbols2;
	ex x,y;
};
auto description_double=make_parameter_description (
		"x", "an ex depending on global symbols", expression(&DoubleGlobalSymbolsParameters::x,&DoubleGlobalSymbolsParameters::symbols),
		"y", "an ex depending on global symbols", expression(&DoubleGlobalSymbolsParameters::y,&DoubleGlobalSymbolsParameters::symbols2)
);

struct ManySymbolsParameters {
	ex symbols;
	ex x;
};
auto description_many_symbols = make_parameter_description(
	"symbols", "symbols to be used in expression", new_symbols(&ManySymbolsParameters::symbols),
	"x", "expression using symbols ", expression(&ManySymbolsParameters::x,&ManySymbolsParameters::symbols)
);

struct CommandLineParameters {
	GlobalSymbols symbols;
	unique_ptr<LieGroup> G;
};
auto description=make_parameter_description
(
		"lie-algebra","Lie algebra with parameters",lie_algebra(&CommandLineParameters::G,&CommandLineParameters::symbols)
);

class SymbolsTestSuite : public CxxTest::TestSuite
{
public:

	void testGlobalSymbols() {
		const char* (argv[]) {"program invocation", "--lie-algebra=0,0,[a]*12,[phi]*13"};
		int argc=std::size(argv);
		auto parameters=description.parametersFromCommandLine(argc,argv);
		stringstream s;
		parameters.G->canonical_print(s);
		TS_ASSERT_EQUALS(s.str(),"(0,0,a*(e1*e2),phi*(e1*e3))");
	}
	void testex() {
		const char* (argv[]) {"program invocation", "--x=sqrt(13)+2", "--y=4"};
		int argc=std::size(argv);
		auto parameters=description_ex_no_symbols.parametersFromCommandLine(argc,argv);
		TS_ASSERT_EQUALS(parameters.x,sqrt(ex(13))+2);
		TS_ASSERT_EQUALS(parameters.y,4);
	}
	void testexsymbols() {
		const char* (argv[]) {"program invocation", "--symbol=a", "--expression=sqrt(13)+a"};
		int argc=std::size(argv);
		auto parameters=description_ex_with_symbols.parametersFromCommandLine(argc,argv);
		TS_ASSERT_EQUALS(parameters.y,parameters.x+sqrt(ex(13)));
	}
	void testDoubleGlobalSystem() {
		const char* (argv[]) {"program invocation", "--x=a", "--y=a"};
		int argc=std::size(argv);
		auto parameters=description_double.parametersFromCommandLine(argc,argv);
		TS_ASSERT_EQUALS(parameters.x,parameters.y);

	}
	void testManySymbols(){
		const char* (argv[]) {"program invocation", "--x=a+b", "--symbols=z", "x",  "y", "a", "b"};
		int argc=std::size(argv);
		auto parameters=description_many_symbols.parametersFromCommandLine(argc,argv);
		TS_ASSERT_EQUALS(parameters.x,parameters.symbols.op(3)+parameters.symbols.op(4));

	}

};
