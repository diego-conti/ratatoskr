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
#ifndef RATATOSKR_MATRIX_H
#define RATATOSKR_MATRIX_H
namespace ratatoskr {
using namespace GiNaC;
using namespace Wedge;

inline matrix matrix_from_string_matrix(const vector<vector<string>>&m, const Symbols& symbols) {
    if (m.empty()) return matrix{};
    matrix result(m.size(),m[0].size());
    for (int i=0;i<result.rows();++i) {
        if (m[i].size()!=result.cols()) throw UnevenMatrix(result.rows(),result.cols());        
        for (int j=0;j<result.cols();++j) {        
            try  {
                result(i,j)=symbols.ex_from_string(m[i][j]);
            }    
            catch (...) {
                throw ParseError(m[i][j]);
            }
        }
    }
    return result;
}

inline matrix matrix_from_rows(const vector<string>& s, const Symbols& symbols) {
    vector<vector<string>> elements;
    std::transform(s.begin(),s.end(),std::back_inserter(elements),[] (auto& string) {return splice(string);});
    return matrix_from_string_matrix(elements,symbols.symbols());
}
inline matrix diagonal_matrix_from_strings(const vector<string>& s, const Symbols& symbols) {
	unsigned int order=static_cast<int>(s.size());
	matrix r{order,order};
	for (int i=0;i<order;++i)
		r(i,i)=symbols.ex_from_string(s[i]);
	return r;
}
inline matrix diagonal_matrix_from_string(const string& s, const Symbols& symbols) {
	return diagonal_matrix_from_strings(splice(s),symbols);
}

template<typename Parameters, typename ParameterType, typename Symbols>
auto matrix_by_elements(ParameterType Parameters::*p,Symbols Parameters::*symbols) {
	return generic_converter<vector<string>>(p,matrix_from_rows,symbols);
}
template<typename Parameters, typename ParameterType, typename Symbols>
auto matrix_by_elements(ParameterType Parameters::*p) {
    auto converter = [] (const string& parameter) {return matrix_from_rows(parameter,Symbols{});};
	return generic_converter<vector<string>>(p,converter);
}

template<typename Parameters, typename ParameterType, typename Symbols>
auto diagonal_matrix(ParameterType Parameters::*p,Symbols Parameters::*symbols) {
	return generic_converter(p,diagonal_matrix_from_string,symbols);
}
template<typename Parameters, typename ParameterType>
auto diagonal_matrix(ParameterType Parameters::*p) {
    auto converter = [] (const string& parameter) {return diagonal_matrix_from_string(parameter,Symbols{});};
	return generic_converter(p,converter);
}

}

#endif