namespace ratatoskr {
template<typename Parameters, typename TupleOfAlternatives>
class AlternativeParameterDescriptions {
	string description_;
	TupleOfAlternatives alternatives;
public:
	AlternativeParameterDescriptions(const string& description,TupleOfAlternatives&& alternatives) :
		description_{description}, alternatives{std::forward<TupleOfAlternatives>(alternatives)}
	{}
	string name() const {return "[alternatives]";}
	string description() const {return description_;}
	int fill(Parameters& parameters, int argc, const char** argv) const {
		int count=0;
		auto fill_if_present=[&parameters,argc,argv,&count] (auto& desc) {
			count+=desc.fill(parameters,argc,argv);
		};
		iterate_over_tuple(fill_if_present,alternatives);
		if (count>1) throw TooManyAlternatives(description_);
		return count;
	}
	void add_option_description(po::options_description& options) const {
		stringstream s;
		auto print_to_s=[&s] (auto& alternative) {
			if (!s.str().empty()) s<<" or "<<endl;
			s<<alternative.human_readable_description();
		};
		iterate_over_tuple(print_to_s,alternatives);
		options.add_options()(s.str().c_str(),description_.c_str());	//add a dummy option to tell the user that one of the alternatives must be indicated
		auto add_option=[&options] (auto& alternative) {alternative.add_option_description(options);};
		iterate_over_tuple(add_option,alternatives);
	}
	string human_readable_description(int indent=0) const {
		stringstream s;
		s<<string(' ',indent)<<description()<<endl;
		auto add_description = [&s,indent] (auto& desc) {
				s<<string(' ',indent+1)<<"| "<<desc.human_readable_description()<<endl;
			};
		iterate_over_tuple(add_description,alternatives);
		return s.str();
	}
	template<typename... T>
	auto operator() (T&&... parameter_descriptions) const {
		auto new_alternative=make_sequence_of_parameter_descriptions<Parameters>(std::forward<T>(parameter_descriptions)...);
		auto tuple=tuple_cat(alternatives,make_tuple(new_alternative));
		return AlternativeParameterDescriptions<Parameters,decltype(tuple)>(description_,move(tuple));
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

template<typename Parameters, typename TupleOfAlternatives, typename... T>
auto tuple_of_parameter_descriptions(AlternativeParameterDescriptions<Parameters, TupleOfAlternatives> alternatives, T&&... otherParameters) {
	return insert_in_tuple(alternatives,tuple_of_parameter_descriptions(std::forward<T>(otherParameters)...));
}

template<typename Parameters>
auto alternative(const string& description) {
	return AlternativeParameterDescriptions<Parameters,tuple<>>(description,make_tuple());
}



}
