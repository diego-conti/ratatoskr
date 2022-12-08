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
	return generic_converter(p,converter,G);
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
