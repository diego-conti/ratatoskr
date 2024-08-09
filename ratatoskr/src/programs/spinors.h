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
using namespace ratatoskr;

namespace Nabla {

	struct Parameters {
		unique_ptr<LieGroup> G;
		unique_ptr<PseudoRiemannianStructureByOrthonormalFrame> g;
		vector<int> timelike_indices;
		lst symbols;
	};

	auto parameters_description=make_parameter_description (
		"lie-algebra","Lie algebra without parameters",lie_algebra(&Parameters::G),
		alternative("metric")(
			"timelike","Sequence of timelike indices in frame",&Parameters::timelike_indices,
			"indefinite-on-frame","Orthonormal frame for the metric",metric_by_on_frame(&Parameters::g,&Parameters::G,&Parameters::timelike_indices)
		)
		(
			"on-frame","Orthonormal frame for the metric",metric_by_on_frame(&Parameters::g,&Parameters::G,&Parameters::timelike_indices)
		)
	);
	auto program = make_program_description(
		"nabla", "Compute covariant derivatives of spinors on a pseudo-Riemannian metric on a Lie algebra",
		parameters_description, [] (Parameters& parameters, ostream& os) {
			parameters.G->canonical_print(os)<<endl;
			PseudoLeviCivitaConnection omega(parameters.G.get(),*parameters.g);
			os<<"Connection form="<<omega.AsMatrix()<<endl;            
            for (auto X : parameters.G->e())
            for (int i=0;i<parameters.g->DimensionOfSpinorRepresentation();++i) {
                auto u=parameters.g->u(i);
                auto nabla_X_u=omega.Nabla<Spinor>(X,u);
                os<<"\\nabla_{"<<X<<"}"<<u<<"="<<nabla_X_u<<endl;
            }
		}
	);

}

namespace NablaSpinor {

	struct Parameters {
		unique_ptr<LieGroup> G;
		unique_ptr<PseudoRiemannianStructureByOrthonormalFrame> g;
		ex psi;
		vector<int> timelike_indices;
		lst symbols;
		GlobalSymbols global_symbols;
	};

	auto parameters_description=make_parameter_description (
		"lie-algebra","Lie algebra without parameters",lie_algebra(&Parameters::G),
		alternative("metric")(
			"timelike","Sequence of timelike indices in frame",&Parameters::timelike_indices,
			"indefinite-on-frame","Orthonormal frame for the metric",metric_by_on_frame(&Parameters::g,&Parameters::G,&Parameters::timelike_indices)
		)
		(
			"on-frame","Orthonormal frame for the metric",metric_by_on_frame(&Parameters::g,&Parameters::G,&Parameters::timelike_indices)
		),
		"spinor","coefficients of the spinor relative to basis",spinor(&Parameters::psi,&Parameters::g,&Parameters::global_symbols)
	);
	auto program = make_program_description(
		"nabla-spinor", "Compute covariant derivatives of a spinor on a pseudo-Riemannian metric on a Lie algebra",
		parameters_description, [] (Parameters& parameters, ostream& os) {
			parameters.G->canonical_print(os)<<endl;
			PseudoLeviCivitaConnection omega(parameters.G.get(),*parameters.g);
			os<<"Connection form="<<omega.AsMatrix()<<endl;            
            for (auto X : parameters.G->e()) {            
                auto nabla_X_psi=NormalForm<Spinor>(omega.Nabla<Spinor>(X,parameters.psi));
                os<<"\\nabla_{"<<X<<"}"<<parameters.psi<<"="<<nabla_X_psi<<endl;
            }
		}
	);

}
namespace Clifford {

	struct Parameters {
		unique_ptr<LieGroup> G;
		unique_ptr<PseudoRiemannianStructureByOrthonormalFrame> g;
		vector<int> timelike_indices;
		lst symbols;
	};

	auto parameters_description=make_parameter_description (
		"lie-algebra","Lie algebra without parameters",lie_algebra(&Parameters::G),
		alternative("metric")(
			"timelike","Sequence of timelike indices in frame",&Parameters::timelike_indices,
			"indefinite-on-frame","Orthonormal frame for the metric",metric_by_on_coframe(&Parameters::g,&Parameters::G,&Parameters::timelike_indices)
		)
		(
			"on-frame","Orthonormal frame for the metric",metric_by_on_coframe(&Parameters::g,&Parameters::G,&Parameters::timelike_indices)
		)
	);
	auto program = make_program_description(
		"clifford", "Compute cliford product on a pseudo-Riemannian metric on a Lie algebra",
		parameters_description, [] (Parameters& parameters, ostream& os) {
			parameters.G->canonical_print(os)<<endl;
            for (auto X : parameters.G->e())
            for (int i=0;i<parameters.g->DimensionOfSpinorRepresentation();++i) {
                auto u=parameters.g->u(i);                
                os<<X<<"\\cdot "<<u<<"="<<parameters.g->CliffordDot(X,u)<<endl;
            }
		}
	);

}
