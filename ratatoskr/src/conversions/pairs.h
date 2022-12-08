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
namespace ratatoskr {

template<typename Parameters, typename First, typename Second>
auto comma_separated_pair(pair<First,Second> Parameters::*p) {
	auto converter=[] (const string& values) {
		try {
			return pair_from_csv<First,Second>(values);
		}
		catch (PairParseError& e) {
			throw ConversionError(e.what());
		}
	};
	return generic_converter(p,converter);
}

template<typename Parameters, typename First, typename Second>
auto vector_of_comma_separated_pairs(vector<pair<First,Second>> Parameters::*p) {
	auto converter=[] (const vector<string>& values) {
		vector<pair<First,Second>> result;
		try {
			transform(values.begin(),values.end(),back_inserter(result), pair_from_csv<First,Second>);
			return result;
		}
		catch (PairParseError& e) {
			throw ConversionError(e.what());
		}
	};
	return generic_converter<vector<string>>(p,converter);
}
}
