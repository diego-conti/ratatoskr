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
//Example programs to illustrate ex parsing
using namespace ratatoskr;
namespace Convert {
struct Parameters {
	float amount;
	ex conversion;
};
auto parameters_description=make_parameter_description(
	"amount","The amount to convert",&Parameters::amount,
	"conversion-ratio","The conversion ratio",expression(&Parameters::conversion)
);
auto program = make_program_description(
	"convert", "Convert to a different unit of measurement by applying a coefficient",
	parameters_description, [] (Parameters& parameters, ostream& os) {
		os<<parameters.conversion*parameters.amount<<endl;
	}
);
}

namespace Derivative {
struct Parameters {
	ex function;
	Symbol<> variable;
};
auto parameters_description=make_parameter_description(
	"variable","a variable",new_symbol(&Parameters::variable),
	"function","a function of one variable",expression(&Parameters::function,&Parameters::variable)
);
auto program = make_program_description(
	"derivative", "Take the derivative of a function of one variable",
	parameters_description, [] (Parameters& parameters, ostream& os) {
		os<<parameters.function.diff(parameters.variable)<<endl;
	}
);
}

namespace PartialDerivative {
struct Parameters {
	GlobalSymbols symbols;
	ex function;
	ex variable;
};
auto parameters_description=make_parameter_description(
	"variable","a variable",expression(&Parameters::variable,&Parameters::symbols),
	"function","a function of one or more variables",expression(&Parameters::function,&Parameters::symbols)
);
auto program = make_program_description(
	"partial-derivative", "Take a partial derivative of a function of more variables",
	parameters_description, [] (Parameters& parameters, ostream& os) {
		os<<parameters.function.diff(ex_to<symbol>(parameters.variable))<<endl;
	}
);
}

namespace Invert {
	struct Parameters {
		GlobalSymbols symbols;
		matrix m;
	};
	auto parameters_description = make_parameter_description(
		alternative("the matrix to invert")
		("matrix", "a matrix, written by a space-separated list of lists of comma-separated values",matrix_by_elements(&Parameters::m,&Parameters::symbols))
		("diagonal-matrix", "a diagonal matrix, written by a comma-separated list of the diagonal entries",diagonal_matrix(&Parameters::m,&Parameters::symbols))
	);
	auto program = make_program_description(
		"invert", "compute the inverse of a matrix",
		parameters_description, [] (Parameters& parameters, ostream& os) {
			os<<parameters.m.inverse()<<endl;
		}
	);
}
