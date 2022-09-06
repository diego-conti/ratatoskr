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
#ifndef PAIR_FROM_H
#define PAIR_FROM_H
#include "splice.h"

namespace ratatoskr {
/** @brief returns a pair object from a string. The string should be formatted as a sequence of two strings convertible to U and V, interleaved by a single character */

class PairParseError : public std::runtime_error {
public:
	PairParseError(const string& value, char expected) : std::runtime_error{
		value+" is not a pair of values separated by `"+expected+"'"
	} {}
};

template<typename U>
U from_string(const string& s) {
	U result;
	std::stringstream str{s};
	str>>result;
	return result;
}

template<typename U, typename V>
std::pair<U,V> pair_from_string(const std::string& s, char sep) {
	vector<string> elements=splice(s,sep);
	if (elements.size()!=2) throw PairParseError(s,sep);
	return make_pair(from_string<U>(elements[0]), from_string<V>(elements[1]));
}

template<typename U, typename V>
std::pair<U,V> pair_from_csv(const std::string& s) {
	return pair_from_string<U,V>(s,',');
}

}
#endif
