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
#ifndef RATATOSKR_LIE_ALGEBRAS_H
#define RATATOSKR_LIE_ALGEBRAS_H
namespace ratatoskr {
using namespace GiNaC;
using namespace Wedge;

template<typename Parameters, typename ParameterType>
auto lie_algebra(unique_ptr<ParameterType> Parameters::*p) {
	auto converter=[] (const string& parameter) {
		return make_unique<AbstractLieGroup<false>>(parameter);
	};
	return generic_converter(p,converter);
}
template<typename Parameters, typename ParameterType, typename SymbolsClass>
auto lie_algebra(unique_ptr<ParameterType> Parameters::*p, SymbolsClass Parameters::*symbols) {
	auto converter=[] (const string& parameter, const Symbols& symbols) {
		return make_unique<AbstractLieGroup<true>>(parameter,symbols.symbols());
	};
	return generic_converter(p,converter,symbols);
}


unique_ptr<AbstractLieSubgroup<true>>
make_subgroup(const LieGroupHasParameters<true>& G, const exvector& subalgebra) {
	return make_unique<AbstractLieSubgroup<true>>(G,subalgebra);
}
unique_ptr<AbstractLieSubgroup<false>>
make_subgroup(const LieGroupHasParameters<false>& G, const exvector& subalgebra) {
	return make_unique<AbstractLieSubgroup<false>>(G,subalgebra);
}

template<typename Parameters, typename LieSubgroupType, typename LieGroupType>
auto lie_subalgebra(unique_ptr<LieSubgroupType> Parameters::*p, unique_ptr<LieGroupType> Parameters::*G) {
	auto converter=[] (const string& parameter, const unique_ptr<LieGroupType>& G) {
		return make_subgroup(*G,ParseDifferentialForms(G->e(),parameter.c_str()));
	};
	return generic_converter(p,converter,G);
}

template<typename Parameters, typename LieSubgroupType, typename LieGroupType, typename SymbolsClass>
auto lie_subalgebra(unique_ptr<LieSubgroupType> Parameters::*p, unique_ptr<LieGroupType> Parameters::*G, SymbolsClass Parameters::*symbols) {
	auto converter=[] (const string& parameter, const unique_ptr<LieGroupType>& G, const Symbols& symbols) {
		return make_unique<AbstractLieSubgroup<true>>(*G,ParseDifferentialForms(G->e(),parameter.c_str(),symbols.symbols()));
	};
	return generic_converter(p,converter,G,symbols);
}

}
#endif
