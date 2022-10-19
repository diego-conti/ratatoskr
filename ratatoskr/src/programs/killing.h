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
using namespace ratatoskr;

namespace Killing {

	struct Parameters {
		unique_ptr<LieGroup> G;		
	};

	auto parameters_description=make_parameter_description (
		"lie-algebra","Lie algebra without parameters, in terms of orthonormal frame",lie_algebra(&Parameters::G)
	);

	VectorSpace<Spinor> killing_spinors(const LieGroup& G, const RiemannianStructure& g, const RiemannianConnection& omega, ex lambda) {
			VectorSpace<Spinor> spinors;
			for (int i=0;i<g.DimensionOfSpinorRepresentation();++i)
				spinors.AddGenerator(g.u(i));
			ex u=spinors.GenericElement();
			lst eqns;
			for (auto e_i : G.e()) 
				GetCoefficients<Spinor>(eqns,omega.Nabla<Spinor>(e_i, u)-lambda*g.CliffordDot(e_i,u));						
			return spinors.SubspaceFromEquations(eqns.begin(),eqns.end());
	}

	ex killing_constant(const LeviCivitaConnection<true>& omega) {
			auto ric=omega.RicciAsMatrix();
			int n=ric.cols();
			if (!(ric(0,0)*unit_matrix(n)-ric).evalm().is_zero_matrix())
				throw std::runtime_error("Einstein metric expected, but ric=" + to_canonical_string(ric));
			return sqrt(ric(0,0)/(4*(n-1)));
	}

	auto program = make_program_description(
		"killing", "Compute the space of killing spinors for a Riemannian metric on a Lie algebra",
		parameters_description, [] (Parameters& parameters, ostream& os) {
			parameters.G->canonical_print(os)<<endl;
			RiemannianStructure g(parameters.G.get(),parameters.G->e());
			LeviCivitaConnection<true> omega(parameters.G.get(),g);
			ex lambda=killing_constant(omega);
			os<<"Killing spinors for \\lambda="<<lambda<<endl;
			os<<killing_spinors(*parameters.G,g,omega,lambda).e();
			os<<"Killing spinors for \\lambda="<<-lambda<<endl;
			os<<killing_spinors(*parameters.G,g,omega,-lambda).e();
		}
	);

}
