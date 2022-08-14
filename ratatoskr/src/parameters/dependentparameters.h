#ifndef DEPENDENT_PARAMETERS_H
#define DEPENDENT_PARAMETERS_H
#include "commandlineparameters.h"
#include <type_traits>
namespace ratatoskr {

template<typename Parameters,typename T>
auto& dereference_if_pointer_to_member(T&& p, Parameters& parameters) {
	return std::forward<T>(p);
}
template<typename Parameters, typename T>
auto& dereference_if_pointer_to_member(T Parameters::*p, Parameters& parameters) {
	return parameters.*p;
}

template<typename Parameters, typename ParameterType,typename Converter, typename RequiredParameters, typename BoostParameterType>
class DependentParameterDescription final {
	string name_;
	string description_;
	ParameterType Parameters::*p_;
	Converter converter;
	RequiredParameters required_parameters;
	template<class U=BoostParameterType, enable_if_t<is_void_v<U>,bool> =false>
	void do_fill(Parameters& parameters, const po::variables_map& command_line_variable_map) const {
		auto bind = [&parameters] (auto... pointers) {return tie(dereference_if_pointer_to_member<Parameters>(pointers,parameters)...);};
		auto bound_required_parameters=std::apply(bind, required_parameters);
		parameters.*p_=std::apply(converter,bound_required_parameters);
	}
	template<class U=BoostParameterType, enable_if_t<!is_void_v<U>,bool> =false>
	void do_fill(Parameters& parameters, const po::variables_map& command_line_variable_map) const {
		auto& text=command_line_variable_map[name_].as<BoostParameterType>();
		auto bind = [&parameters] (auto... pointers) {return tie(dereference_if_pointer_to_member<Parameters>(pointers,parameters)...);};
		auto bound_required_parameters=std::apply(bind, required_parameters);
		parameters.*p_=std::apply(converter,insert_in_tuple(text,bound_required_parameters));
	}
public:
	DependentParameterDescription(string name, string description,
			ParameterType Parameters::*p, Converter& converter,const RequiredParameters& required_parameters)
		: name_{name}, description_{description}, p_{p}, converter{converter}, required_parameters{required_parameters}
		{}
	string name() const {return name_;}
	string description() const {return description_;}
	void fill(Parameters& parameters, const po::variables_map& command_line_variable_map) const {
		if (!fill_optional(parameters,command_line_variable_map)) throw MissingParameter(name_);
	}
	int fill_optional(Parameters& parameters, const po::variables_map& command_line_variable_map) const {
		if (!command_line_variable_map.count(name_)) return 0;
		else {
			do_fill(parameters,command_line_variable_map);
			return 1;
		}
	}
	template<class U=BoostParameterType, enable_if_t<is_void_v<U>,bool> =false>
	void add_option_description(po::options_description& options) const {
			options.add_options()(name_.c_str(), description_.c_str());
	}
	template<class U=BoostParameterType, enable_if_t<!is_void_v<U>,bool> =false>
	void add_option_description(po::options_description& options) const {
		options.add_options()(name_.c_str(), po::value<BoostParameterType>(),description_.c_str());
	}
};

template<typename Parameters, typename ParameterType, typename Converter, typename RequiredParameters=tuple<>,typename BoostParameterType=string>
struct dependent_parameter_tag {
	ParameterType Parameters::*p;
	Converter converter;
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

template<typename Parameters, typename... T>
auto make_parameter_description(T... options) {
	auto tuple=tuple_of_parameter_descriptions<Parameters>(options...);
	return DescriptionOfCommandLineParameters<Parameters,decltype(tuple)>(tuple);
}

template<typename Parameters, typename ParameterType, typename Converter, typename... RequiredParameters>
auto generic_converter(ParameterType Parameters::*p, Converter&& converter, RequiredParameters... required_parameters) {
	auto tuple=make_tuple(required_parameters...);
	return dependent_parameter_tag<Parameters,ParameterType,Converter,decltype(tuple),string>{p,std::forward<Converter>(converter),move(tuple)};
}

template<typename Parameters, typename ParameterType, typename Converter, typename... RequiredParameters>
auto generic_option(ParameterType Parameters::*p, Converter&& converter, RequiredParameters... required_parameters) {
	auto tuple=make_tuple(required_parameters...);
	return dependent_parameter_tag<Parameters,ParameterType,Converter,decltype(tuple),void>{p,std::forward<Converter>(converter),move(tuple)};
}
}
#endif
