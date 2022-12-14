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

struct UndefinedParametersType {};

template<typename T>
constexpr bool is_undefined_v=is_same_v<T,UndefinedParametersType>;

template<typename T>
struct UnderlyingParameters {
	using type=UndefinedParametersType;
};

template<typename T>
using underlying_parameters_t=typename UnderlyingParameters<T>::type;


//forward references
template<typename TupleOfParameterDescriptions,typename Parameters=underlying_parameters_t<TupleOfParameterDescriptions>>
class SequenceOfParameterDescriptions;

template<typename Parameters,typename ParameterType,typename Converter, typename RequiredParameters, typename BoostParameterType>
class DependentParameterDescription;

template<typename Parameters,typename ParameterType,typename Initializer, typename RequiredParameters>
class OptionDescription;

template<typename TupleOfAlternatives,typename Parameters=underlying_parameters_t<TupleOfAlternatives>>
class AlternativeParameterDescriptions;

template<typename TupleOfParameterDescriptions,typename Parameters=underlying_parameters_t<TupleOfParameterDescriptions>>
class DescriptionOfCommandLineParameters;

//specializations
template<typename TupleOfAlternatives,typename Parameters>
struct UnderlyingParameters<SequenceOfParameterDescriptions<TupleOfAlternatives,Parameters>> {
	using type=Parameters;
};

template<typename TupleOfAlternatives,typename Parameters>
struct UnderlyingParameters<AlternativeParameterDescriptions<TupleOfAlternatives,Parameters>> {
	using type=Parameters;
};

template<typename TupleOfAlternatives,typename Parameters>
struct UnderlyingParameters<DescriptionOfCommandLineParameters<TupleOfAlternatives,Parameters>> {
	using type=Parameters;
};

template<typename Parameters, typename ParameterType,typename Converter, typename RequiredParameters, typename BoostParameterType>
struct UnderlyingParameters<
	DependentParameterDescription<Parameters,ParameterType, Converter,  RequiredParameters,  BoostParameterType>
> {
	using type=Parameters;
};

template<typename Parameters, typename ParameterType,typename Initializer, typename RequiredParameters>
struct UnderlyingParameters<
	OptionDescription<Parameters,ParameterType, Initializer,  RequiredParameters>
> {
	using type=Parameters;
};

template<>
struct UnderlyingParameters<tuple<>> {
	using type=UndefinedParametersType;
};

template<typename T, typename...U >
class UnderlyingParameters<tuple<T,U...>> {
	using ParametersOfFirst=underlying_parameters_t<T>;
	using ParametersOfRest=underlying_parameters_t<tuple<U...>>;
	static_assert(is_same_v<ParametersOfFirst,ParametersOfRest>|| is_undefined_v<ParametersOfRest>,"tuple of elements with different underlying Parameters");
public:
	using type=ParametersOfFirst;
};
}
