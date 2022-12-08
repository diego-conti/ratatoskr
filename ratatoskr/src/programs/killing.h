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

namespace Killing {

	struct Parameters {
		unique_ptr<LieGroup> G;
		unique_ptr<PseudoRiemannianStructureByOrthonormalFrame> g;
		vector<int> timelike_indices;
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

	VectorSpace<Spinor> killing_spinors(const LieGroup& G, const PseudoRiemannianStructureByOrthonormalFrame& g, const PseudoLeviCivitaConnection& omega, ex lambda) {
			VectorSpace<Spinor> spinors;
			for (int i=0;i<g.DimensionOfSpinorRepresentation();++i)
				spinors.AddGenerator(g.u(i));
			ex u=spinors.GenericElement();
			lst eqns;
			for (auto e_i : G.e()) {
				GetCoefficients<Spinor>(eqns,omega.Nabla<Spinor>(e_i, u)-lambda*g.CliffordDot(e_i,u));						
			}
			return spinors.SubspaceFromEquations(eqns.begin(),eqns.end());
	}

	ex killing_constant(const PseudoLeviCivitaConnection& omega, vector<int> timelike_indices) {
			auto ric=omega.RicciAsMatrix();
			int n=ric.cols();
			auto g=ex_to<matrix>(unit_matrix(n));
			for (int i: timelike_indices) g(i-1,i-1)=-1;
			if (!(ric(0,0)*g*g(0,0)-ric).evalm().is_zero_matrix())
				throw std::runtime_error("Einstein metric expected, but ric=" + to_canonical_string(ric));
			return sqrt(g(0,0)*	ric(0,0)/(4*(n-1)));
	}

	auto program = make_program_description(
		"killing", "Compute the space of invariant Killing spinors for a Riemannian metric on a Lie algebra",
		parameters_description, [] (Parameters& parameters, ostream& os) {
			parameters.G->canonical_print(os)<<endl;
			os<<"timelike indices "<<parameters.g->ScalarProduct().TimelikeIndices()<<endl;
			PseudoLeviCivitaConnection omega(parameters.G.get(),*parameters.g);			
			for (auto e: parameters.G->e())			
			for (auto u: exvector {parameters.g->u(0),parameters.g->u(1)})
				cout<<e<<"."<<u<<"="<<parameters.g->CliffordDot(e,u)<<endl;


			ex lambda=killing_constant(omega,parameters.g->ScalarProduct().TimelikeIndices());
			os<<"Killing spinors for \\lambda="<<lambda<<endl;
			os<<killing_spinors(*parameters.G,*parameters.g,omega,lambda).e();
			os<<"Killing spinors for \\lambda="<<-lambda<<endl;
			os<<killing_spinors(*parameters.G,*parameters.g,omega,-lambda).e();
		}
	);

}
