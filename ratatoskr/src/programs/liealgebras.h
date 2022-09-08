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
using namespace Wedge;
using namespace ratatoskr;

namespace ExtDerivative {
	struct Parameters {
		unique_ptr<LieGroup> G;
		ex form;
	};

	auto parameters_description=make_parameter_description
	(
		"lie-algebra","Lie algebra without parameters",lie_algebra(&Parameters::G),
		"form","a differential form on the Lie algebra",differential_form(&Parameters::form,&Parameters::G)
	);

	auto program = make_program_description(
		"ext-derivative", "Compute the exterior derivative of a form on a Lie algebra",
		parameters_description, [] (Parameters& parameters, ostream& os) {
			os<<parameters.G->d(parameters.form)<<endl;
		}
	);
}

namespace ClosedForms {
	struct Parameters {
		unique_ptr<AbstractLieGroup<false>> G;
		int p;
	};

	auto parameters_description=make_parameter_description
	(
		"lie-algebra","Lie algebra without parameters",lie_algebra(&Parameters::G),
		"p","a positive integer",&Parameters::p
	);

	auto program = make_program_description(
		"closed-forms", "Compute the space of closed p-forms",
		parameters_description, [] (Parameters& parameters, ostream& os) {
			os<<parameters.G->ClosedForms(parameters.p)<<endl;
		}
	);
}

namespace Subalgebra {
	struct Parameters {
		unique_ptr<LieGroupHasParameters<false>> G;
		unique_ptr<LieGroupHasParameters<false>> H;
	};

	auto parameters_description=make_parameter_description
	(
		"lie-algebra","Lie algebra without parameters",lie_algebra(&Parameters::G),
		"subalgebra","comma-separated list of generators of the subalgebra",lie_subalgebra(&Parameters::H,&Parameters::G)
	);

	auto program = make_program_description(
		"subalgebra-without-parameters", "Compute the structure constant of a Lie subalgebra",
		parameters_description, [] (Parameters& parameters, ostream& os) {
			parameters.H->canonical_print(os)<<endl;
		}
	);
}

namespace SubalgebraWithParameters {
	struct Parameters {
		unique_ptr<LieGroupHasParameters<true>> G;
		unique_ptr<LieGroupHasParameters<true>> H;
		GlobalSymbols symbols;
	};

	auto parameters_description=make_parameter_description
	(
		"lie-algebra","Lie algebra with parameters",lie_algebra(&Parameters::G,&Parameters::symbols),
		"subalgebra","comma-separated list of generators of the subalgebra",lie_subalgebra(&Parameters::H,&Parameters::G,&Parameters::symbols)
	);

	auto program = make_program_description(
		"subalgebra-with-parameters", "Compute the structure constant of a Lie subalgebra",
		parameters_description, [] (Parameters& parameters, ostream& os) {
			parameters.H->canonical_print(os)<<endl;
		}
	);
}

namespace Derivations {
	struct Parameters {
		unique_ptr<LieGroupHasParameters<false>> G;
	};
	auto parameters_description=make_parameter_description
	(
		"lie-algebra","Lie algebra without parameters",lie_algebra(&Parameters::G)
	);

	auto program = make_program_description(
		"derivations", "Compute the derivations of a Lie algebra",
		parameters_description, [] (Parameters& parameters, ostream& os) {
			GL gl(parameters.G->Dimension());
			os<<derivations(*parameters.G,gl)<<endl;			
		}
	);
}