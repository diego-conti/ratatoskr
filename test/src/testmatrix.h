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

struct CommandLineParameters {
	GlobalSymbols symbols;
	matrix m;
};

auto description_diagonal=make_parameter_description
(
		"diagonal-matrix","a diagonal matrix, written by a comma-separated list of values",diagonal_matrix(&CommandLineParameters::m)
);

auto description_diagonal_parameters=make_parameter_description
(
		"diagonal-matrix","a diagonal matrix, written by a comma-separated list of values",diagonal_matrix(&CommandLineParameters::m,&CommandLineParameters::symbols)
);

auto description_by_elements=make_parameter_description
(
		"matrix","a matrix, written by a space-separated list of lists of comma-separated values",matrix_by_elements(&CommandLineParameters::m,&CommandLineParameters::symbols)
);

class MatrixTestSuite : public CxxTest::TestSuite
{
public:
	void testDiagonalMatrix() {
		const char* (argv[]) {"program invocation", "--diagonal-matrix=10,20,30"};
		int argc=std::size(argv);
		auto parameters=description_diagonal.parametersFromCommandLine(argc,argv);
        TS_ASSERT_EQUALS(parameters.m.rows(),3);
        TS_ASSERT_EQUALS(parameters.m.cols(),3);
        TS_ASSERT_EQUALS(parameters.m,(matrix{{10,0,0},{0,20,0},{0,0,30}}));		
	}
	void testDiagonalMatrixParameters() {
		const char* (argv[]) {"program invocation", "--diagonal-matrix=a,2*a,3*a"};
		int argc=std::size(argv);
		auto parameters=description_diagonal_parameters.parametersFromCommandLine(argc,argv);
        TS_ASSERT_EQUALS(parameters.m.rows(),3);
        TS_ASSERT_EQUALS(parameters.m.cols(),3);
        //ex a=parameters.symbols.by_name("a");
        //TS_ASSERT_EQUALS(parameters.m,(matrix{{a,0,0},{0,2*a,0},{0,0,3*a}}));		
	}
	void testMatrixByElements() {
		const char* (argv[]) {"program invocation", "--matrix=1,2,3", "4,5,6", "7,8,9"};
		int argc=std::size(argv);
		auto parameters=description_by_elements.parametersFromCommandLine(argc,argv);
        TS_ASSERT_EQUALS(parameters.m.rows(),3);
        TS_ASSERT_EQUALS(parameters.m.cols(),3);
        TS_ASSERT_EQUALS(parameters.m,(matrix{{1,2,3},{4,5,6},{7,8,9}}));		
	}
};
