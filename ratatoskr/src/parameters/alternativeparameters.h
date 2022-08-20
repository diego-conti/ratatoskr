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
	string human_readable_description(int indent=0) const {
		stringstream s;
		s<<string(indent,' ')<<description()<<endl;
		auto line_sep=string(indent+1,' ')+'+'+string(description().size()-2,'-');
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
		using tuple_type=decltype(tuple);
		return AlternativeParameterDescriptions<underlying_parameters_t<tuple_type>,tuple_type>(description_,move(tuple));
	}
};

template<typename Parameters, typename TupleOfAlternatives, typename... T>
auto tuple_of_parameter_descriptions(AlternativeParameterDescriptions<Parameters, TupleOfAlternatives> alternatives, T&&... otherParameters) {
	return insert_in_tuple(alternatives,tuple_of_parameter_descriptions(std::forward<T>(otherParameters)...));
}

auto alternative(const string& description) {
	auto tuple=make_tuple();
	using tuple_type=decltype(tuple);
	using Parameters=underlying_parameters_t<tuple_type>;
	return AlternativeParameterDescriptions<Parameters,tuple_type>(description,move(tuple));
}



}
