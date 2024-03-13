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
#ifndef RATATOSKR_EXPRESSIONS_H
#define RATATOSKR_EXPRESSIONS_H
namespace ratatoskr {
using namespace GiNaC;
using namespace Wedge;

template<typename Parameters,  typename GroupType>
auto differential_form(ex Parameters::*p,unique_ptr<GroupType> Parameters::*G) {
	auto converter=[] (const string& parameter, unique_ptr<LieGroup>& G) {
		return ParseDifferentialForm(G->e(),parameter.c_str());
	};
	return generic_converter(p,converter,G);
}

template<typename Parameters,  typename GroupType>
auto differential_form(ex Parameters::*p,unique_ptr<GroupType> Parameters::*G,GlobalSymbols Parameters::*symbols) {
	auto converter=[] (const string& parameter, unique_ptr<LieGroup>& G, const GlobalSymbols& global_symbols) {
		return ParseDifferentialForm(G->e(),parameter.c_str(),global_symbols.symbols());
	};
	return generic_converter(p,converter,G,symbols);
}

inline exvector parse_expressions(const string& comma_separated_ex, const lst& symbols=lst{}) {
	int n=0;
	int last_comma;
	exvector result;
	do {
		last_comma=comma_separated_ex.find(',',n);			
		auto substring=comma_separated_ex.substr(n,last_comma==string::npos? string::npos : last_comma-n);
		result.emplace_back(substring,symbols);		
		n=last_comma+1;
	} while (last_comma!=string::npos);
	return result;
}

template<typename Parameters,  typename GroupType>
auto spinor(ex Parameters::*p,unique_ptr<GroupType> Parameters::*G,unique_ptr<PseudoRiemannianStructureByOrthonormalFrame> Parameters::*g) {
	auto converter=[] (const string& parameter, unique_ptr<LieGroup>& G,  unique_ptr<PseudoRiemannianStructureByOrthonormalFrame>& g) {
		auto coefficients=parse_expressions(parameter);
		ex result;
		for (int i=0;i<coefficients.size();++i) result+=coefficients[i]*g->u(i);
		return result;		
	};
	return generic_converter(p,converter,G,g);
}

template<typename Parameters,  typename GroupType, typename Symbols>
auto spinor(ex Parameters::*p,unique_ptr<GroupType> Parameters::*G,unique_ptr<PseudoRiemannianStructureByOrthonormalFrame> Parameters::*g,Symbols Parameters::*symbols) {
	auto converter=[] (const string& parameter, unique_ptr<LieGroup>& G,  unique_ptr<PseudoRiemannianStructureByOrthonormalFrame>& g,const Symbols& symbols) {
		auto coefficients=parse_expressions(parameter,symbols.symbols());
		ex result;
		for (int i=0;i<coefficients.size();++i) result+=coefficients[i]*g->u(i);
		return result;		
	};
	return generic_converter(p,converter,G,g,symbols);
}

template<typename Parameters>
auto expression(ex Parameters::*p) {
	auto converter=[] (const string& parameter) {
		return ex{parameter,lst{}};
	};
	return generic_converter(p,converter);
}

template<typename Parameters, typename SymbolsClass>
auto expression(ex Parameters::*p, SymbolsClass Parameters::*symbols) {
	auto converter=[] (const string& parameter, const Symbols& symbols) {
		return symbols.ex_from_string(parameter);
	};
	return generic_converter(p,converter,symbols);
}

}
#endif
