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
namespace ratatoskr {
template<typename TupleOfAlternatives,typename Parameters>
class AlternativeParameterDescriptions {
	string description_;
	TupleOfAlternatives alternatives;
public:
	AlternativeParameterDescriptions(const string& description,TupleOfAlternatives&& alternatives) :
		description_{description}, alternatives{std::forward<TupleOfAlternatives>(alternatives)}
	{}
	int fill(Parameters& parameters, int argc, const char** argv) const {
		int count=0;
		auto fill_if_present=[&parameters,argc,argv,&count] (auto& desc) {
			count+=desc.fill(parameters,argc,argv);
		};
		iterate_over_tuple(fill_if_present,alternatives);
		if (count>1) throw TooManyAlternatives(description_);
		return count;
	}
	string human_readable_description(int indent=0) const {
		stringstream s;
		s<<two_column_output("[one of the following]", description_,indent);
		auto line_sep=string(indent+1,' ')+'+'+string(description_.size()-2,'-');
		s<<line_sep<<endl;
		auto add_description = [&s,indent,line_sep] (auto& desc) {
				s<<desc.human_readable_description(indent+1);
				s<<line_sep<<endl;
			};
		iterate_over_tuple(add_description,alternatives);
		return s.str();
	}
	template<typename... T>
	auto operator() (T&&... parameter_descriptions) const {
		auto new_alternative=make_sequence_of_parameter_descriptions(std::forward<T>(parameter_descriptions)...);
		auto tuple=tuple_cat(alternatives,make_tuple(new_alternative));
		return AlternativeParameterDescriptions<decltype(tuple)>(description_,std::move(tuple));
	}
};

template<typename TupleOfAlternatives, typename Parameters, typename... T>
auto tuple_of_parameter_descriptions(AlternativeParameterDescriptions<TupleOfAlternatives,Parameters> alternatives, T&&... otherParameters) {
	return insert_in_tuple(alternatives,tuple_of_parameter_descriptions(std::forward<T>(otherParameters)...));
}

auto alternative(const string& description) {
	return AlternativeParameterDescriptions<tuple<>>(description,make_tuple());
}



}
