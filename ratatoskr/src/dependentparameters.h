#ifndef DEPENDENT_PARAMETERS_H
#define DEPENDENT_PARAMETERS_H
#include "commandlineparameters.h"
#include <type_traits>


template<typename Parameters, typename ParameterType,typename Converter, typename RequiredParameters>
class DependentParameterDescription final {
	string name_;
	string description_;
	ParameterType Parameters::*p_;
	Converter converter;
	RequiredParameters required_parameters;
	void do_fill(Parameters& parameters, const po::variables_map& command_line_variable_map) const {
		auto& text=command_line_variable_map[name_].as<string>();
		auto bind = [&parameters] (auto... pointers) {return tie(parameters.*pointers...);};
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
	void add_option_description(po::options_description& options) const {
		options.add_options()(name_.c_str(), po::value<string>(),description_.c_str());
	}
};

template<typename Parameters, typename ParameterType, typename Converter, typename RequiredParameters=tuple<>>
struct dependent_parameter_tag {
	ParameterType Parameters::*p;
	Converter converter;
	RequiredParameters required_parameters;	//tuple of pointers-to-member for class Parameters
};


template<typename Parameters, typename ParameterType, typename Converter, typename RequiredParameters, typename... T>
auto tuple_of_parameter_descriptions(const string& name, const string& description,
		dependent_parameter_tag<Parameters,ParameterType,Converter,RequiredParameters> tag,	T... otherParameters) {
	DependentParameterDescription<Parameters,ParameterType,Converter,RequiredParameters> parameter_description{name,description,tag.p,tag.converter,tag.required_parameters};
	return insert_in_tuple(parameter_description,tuple_of_parameter_descriptions(otherParameters...));
}

template<typename Parameters, typename ParameterType, typename Converter, typename... RequiredParameters>
auto generic_converter(ParameterType Parameters::*p, Converter&& converter, RequiredParameters... required_parameters) {
	auto tuple=make_tuple(required_parameters...);
	return dependent_parameter_tag<Parameters,ParameterType,Converter,decltype(tuple)>{p,std::forward<Converter>(converter),move(tuple)};
}

#endif
