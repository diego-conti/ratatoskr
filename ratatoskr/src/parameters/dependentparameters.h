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
#ifndef DEPENDENT_PARAMETERS_H
#define DEPENDENT_PARAMETERS_H
#include "commandlineparameters.h"
#include "../output/twocolumnoutput.h"
#include <type_traits>
#include <vector>
namespace ratatoskr {

template<typename Parameters,typename T>
auto& dereference_if_pointer_to_member(T&& p, Parameters& parameters) {
	return std::forward<T>(p);
}
template<typename Parameters, typename T>
auto& dereference_if_pointer_to_member(T Parameters::*p, Parameters& parameters) {
	return parameters.*p;
}

template<typename Parameters>
class OptionAndValueDescription {
	string name_;
	string description_;
protected:
	virtual void do_fill(Parameters& parameters, const po::variables_map& command_line_variable_map) const=0;
	virtual string parameter_name() const=0;
	virtual void add_option_description(po::options_description& options) const =0;
	string name() const {return name_;}
	string description() const {return description_;}
public:
	using ParametersType = Parameters;
	OptionAndValueDescription(string name, string description)
		: name_{name}, description_{description}
		{}
	int fill(Parameters& parameters, int argc, const char** argv) const {
		po::variables_map vm;
		po::options_description options;
		add_option_description(options);
		po::store(po::command_line_parser(argc, argv).options(options).allow_unregistered().run(), vm);
		po::notify(vm);
		if (!vm.count(name_)) return 0;
		else {
			do_fill(parameters,vm);
			return 1;
		}
	}
	string human_readable_description(int indent=0) const {
		return two_column_output("--"s+parameter_name(), description(),indent);
	}
	virtual ~OptionAndValueDescription()=default;
};

template<typename T>
struct BoostType {
	static auto value() {return po::value<T>();}
};

template<typename T>
struct BoostType<std::vector<T>> {
	static auto value() {return po::value<std::vector<T>>()->multitoken();}
};


//BoostParameterType is one of the types recognized by boost:program_options for parameters
template<typename Parameters, typename ParameterType,typename Converter, typename RequiredParameters, typename BoostParameterType>
class DependentParameterDescription final : public OptionAndValueDescription<Parameters> {
	ParameterType Parameters::*p;
	Converter converter;
	RequiredParameters required_parameters;
protected:
	void do_fill(Parameters& parameters, const po::variables_map& command_line_variable_map) const override {
		auto& text=command_line_variable_map[this->name()].template as<BoostParameterType>();
		auto bind = [&parameters] (auto... pointers) {return tie(dereference_if_pointer_to_member<Parameters>(pointers,parameters)...);};
		auto bound_required_parameters=std::apply(bind, required_parameters);
		parameters.*p=std::apply(converter,insert_in_tuple(text,bound_required_parameters));
	}
	void add_option_description(po::options_description& options) const override {
		options.add_options()(this->name().c_str(), BoostType<remove_cv_t<remove_reference_t<BoostParameterType>>>::value(),this->description().c_str());
	}
	virtual string parameter_name() const override {return this->name()+" arg"s;}
public:
	DependentParameterDescription(string name, string description,
			ParameterType Parameters::*p, Converter& converter,const RequiredParameters& required_parameters)
		: OptionAndValueDescription<Parameters> {name, description}, p{p}, converter{converter}, required_parameters{required_parameters}
		{}
};

template<typename Parameters, typename ParameterType,typename Initializer, typename RequiredParameters>
class OptionDescription final : public OptionAndValueDescription<Parameters> {
	ParameterType Parameters::*p;
	Initializer initializer;
	RequiredParameters required_parameters;
protected:
	void do_fill(Parameters& parameters, const po::variables_map& command_line_variable_map) const override {
		auto bind = [&parameters] (auto... pointers) {return tie(dereference_if_pointer_to_member<Parameters>(pointers,parameters)...);};
		auto bound_required_parameters=std::apply(bind, required_parameters);
		parameters.*p=std::apply(initializer,bound_required_parameters);
	}
	void add_option_description(po::options_description& options) const override {
			options.add_options()(this->name().c_str(), this->description().c_str());
	}
	virtual string parameter_name() const override {return this->name();}
public:
	OptionDescription(string name, string description,
			ParameterType Parameters::*p, Initializer& initializer,const RequiredParameters& required_parameters)
		: OptionAndValueDescription<Parameters> {name, description}, p{p}, initializer{initializer}, required_parameters{required_parameters}
	{}
};

template<typename Parameters, typename ParameterType, typename Converter, typename RequiredParameters=tuple<>,typename BoostParameterType=string>
struct dependent_parameter_tag {
	ParameterType Parameters::*p;
	Converter converter;
	RequiredParameters required_parameters;	//tuple of pointers-to-member for class Parameters
};
template<typename Parameters, typename ParameterType, typename Initializer, typename RequiredParameters=tuple<>>
struct option_tag {
	ParameterType Parameters::*p;
	Initializer initializer;
	RequiredParameters required_parameters;	//tuple of pointers-to-member for class Parameters
};


auto tuple_of_parameter_descriptions() {
	return make_tuple();
}


template<typename Parameters, typename ParameterType, typename... T>
auto tuple_of_parameter_descriptions(const string& name, const string& description, ParameterType Parameters::*p,
		T... otherParameters) {
	auto identity = [] (const ParameterType& p) {return p;};
	DependentParameterDescription<Parameters,ParameterType,decltype(identity),tuple<>,ParameterType> parameter_descriptions(name,description,p,identity,make_tuple());
	return insert_in_tuple(parameter_descriptions,tuple_of_parameter_descriptions(otherParameters...));
}

template<typename Parameters, typename ParameterType, typename Converter, typename RequiredParameters, typename BoostParameterType, typename... T>
auto tuple_of_parameter_descriptions(const string& name, const string& description,
		dependent_parameter_tag<Parameters,ParameterType,Converter,RequiredParameters,BoostParameterType> tag,	T... otherParameters) {
	DependentParameterDescription<Parameters,ParameterType,Converter,RequiredParameters,BoostParameterType> parameter_description{name,description,tag.p,tag.converter,tag.required_parameters};
	return insert_in_tuple(parameter_description,tuple_of_parameter_descriptions(otherParameters...));
}

template<typename Parameters, typename ParameterType, typename Initializer, typename RequiredParameters, typename... T>
auto tuple_of_parameter_descriptions(const string& name, const string& description,
		option_tag<Parameters,ParameterType,Initializer,RequiredParameters> tag,	T... otherParameters) {
	OptionDescription<Parameters,ParameterType,Initializer,RequiredParameters> parameter_description{name,description,tag.p,tag.initializer,tag.required_parameters};
	return insert_in_tuple(parameter_description,tuple_of_parameter_descriptions(otherParameters...));
}

template<typename... T>
auto make_parameter_description(T... options) {
	auto tuple=tuple_of_parameter_descriptions(options...);
	return DescriptionOfCommandLineParameters<decltype(tuple)>(tuple);
}

template<typename... T>
auto make_sequence_of_parameter_descriptions(T... options) {
	auto tuple=tuple_of_parameter_descriptions(options...);
	using tuple_type=decltype(tuple);
	using Parameters=underlying_parameters_t<tuple_type>;
	return SequenceOfParameterDescriptions<tuple_type>(tuple);
}

template<typename BoostType=string,typename Parameters, typename ParameterType, typename Converter,  typename... RequiredParameters>
auto generic_converter(ParameterType Parameters::*p, Converter converter, RequiredParameters... required_parameters) {
	auto tuple=make_tuple(required_parameters...);
	return dependent_parameter_tag<Parameters,ParameterType,Converter,decltype(tuple),BoostType>{p,converter,move(tuple)};
}

template<typename Parameters, typename ParameterType, typename Converter, typename... RequiredParameters>
auto generic_option(ParameterType Parameters::*p, Converter converter, RequiredParameters... required_parameters) {
	auto tuple=make_tuple(required_parameters...);
	return option_tag<Parameters,ParameterType,Converter,decltype(tuple)>{p,converter,move(tuple)};
}
}
#endif
