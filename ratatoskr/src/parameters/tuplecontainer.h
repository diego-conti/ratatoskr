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
#ifndef TUPLE_CONTAINER_H
#define TUPLE_CONTAINER_H
#include <type_traits>
#include <tuple>
namespace ratatoskr {

template<typename F, typename Tuple, int n=0, enable_if_t<n<tuple_size_v<Tuple>,int> =0>
void iterate_over_tuple(const F& function, const Tuple& tuple) {
	function(std::get<n>(tuple));
	iterate_over_tuple<F,Tuple,n+1>(function,tuple);
}

template<typename F, typename Tuple, int n=0,enable_if_t<n>=tuple_size_v<Tuple>,int> =0>
void iterate_over_tuple(const F& function, const Tuple& tuple) {
}

template<typename T, typename Tuple>
auto insert_in_tuple(T&& t, Tuple&& tuple) {
	return tuple_cat(make_tuple(std::forward<T>(t)),std::forward<Tuple>(tuple));
}
}
#endif
