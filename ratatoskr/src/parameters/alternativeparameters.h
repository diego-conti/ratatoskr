#include "dependentparameters.h"

namespace ratatoskr {
template<typename Parameters, typename TupleOfAlternatives>
class AlternativeParameterDescriptions {
	string description_;
	TupleOfAlternatives alternatives;;
public:
	AlternativeParameterDescriptions(const string& description,TupleOfAlternatives&& alternatives) :
		description_{description}, alternatives{std::forward<TupleOfAlternatives>(alternatives)}
	{}
	string name() const {return "[alternatives]";}
	string description() const {return description_;}
	void fill(Parameters& parameters, const po::variables_map& command_line_variable_map) const {
		auto count=fill_optional(parameters,command_line_variable_map);
		if (count>1) throw TooManyAlternatives(description_);
		else if (count==0) throw MissingParameter(description_);
	}
	int fill_optional(Parameters& parameters, const po::variables_map& command_line_variable_map) const {
		int count=0;
		auto fill_if_present=[&parameters,&command_line_variable_map,&count] (auto& desc) {
			count+=desc.fill_optional(parameters,command_line_variable_map);
		};
		iterate_over_tuple(fill_if_present,alternatives);
		return count;
	}
	void add_option_description(po::options_description& options) const {
		stringstream s;
		auto print_to_s=[&s] (auto& desc) {
			if (!s.str().empty()) s<<'|';
			s<<desc.name();
		};
		iterate_over_tuple(print_to_s,alternatives);
		options.add_options()(s.str().c_str(),description_.c_str());	//add a dummy option to tell the user that one of the alternatives must be indicated
		auto add_option=[&options] (auto& desc) {desc.add_option_description(options);};
		iterate_over_tuple(add_option,alternatives);
	}
};

template<typename T>
struct parameters_type {
	using type=void;
};

template<typename Parameters, typename TupleOfAlternatives>
struct parameters_type<AlternativeParameterDescriptions<Parameters,TupleOfAlternatives>> {
	using type= Parameters;
};

template<typename Parameters, typename ParameterType,typename Converter, typename RequiredParameters, typename BoostParameterType>
struct parameters_type<DependentParameterDescription<Parameters,ParameterType,Converter,RequiredParameters,BoostParameterType>> {
	using type=Parameters;
};

template<typename T, typename... U>
struct parameters_type<tuple<T,U...>> {
	using type= typename parameters_type<T>::type;
};

template<typename T>
using parameters_t=typename parameters_type<T>::type;

template<typename... T>
auto alternative(const string& description, T&&... parameter_descriptions) {
	auto alternatives=tuple_of_parameter_descriptions(std::forward<T>(parameter_descriptions)...);
	using Parameters=parameters_t<decltype(alternatives)>;
	return AlternativeParameterDescriptions<Parameters,decltype(alternatives)>(description,std::move(alternatives));
}

template<typename Parameters, typename TupleOfAlternatives, typename... T>
auto tuple_of_parameter_descriptions(AlternativeParameterDescriptions<Parameters, TupleOfAlternatives> alternatives, T&&... otherParameters) {
	return insert_in_tuple(alternatives,tuple_of_parameter_descriptions(std::forward<T>(otherParameters)...));
}
}
