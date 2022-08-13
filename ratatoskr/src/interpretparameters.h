#include "commandlineparameters.h"

/*
template<typename Tuple>
class InterpretationContext {
	Tuple base_parameters;
public:
	InterpretationContext(Tuple&& base_parameters) : base_parameters{std::forward<Tuple>(base_parameters)} {}

	ex form (const string& form) const {return ParseDifferentialForm(G->e(),form,lst);}
	matrix metric_from_musical_isomorphism(const string& flat) const ;
	ex ginacexpr(const string& rep) const {
		return ex{rep,symbols};
	}
};




auto tuple_of_context_parameters() {
	return make_tuple();
}

template<typename ParameterType, typename Parameters, typename... T>
auto tuple_of_context_parameters(ParameterType Parameters::*p,T... otherParameters) {
	return insert_in_tuple(p, tuple_of_context_parameters(otherParameters...));
}

template<typename... T>
auto make_interpretation_context(T... base_parameters) {
	auto tuple=tuple_of_context_parameters(base_parameters...);
	return InterpretationContext<decltype(base_parameters)>(base_parameters);
}


template<typename Input, typename Output, Output InterpretationContext::*convert(const Input& input)>
class Interpreter {
public:
	Output operator() (const InterpretationContext& context, const Input& input) const {
		return context::*convert(input);
	}
};
*/


template<typename Parameters, typename ParameterType>
class LieAlgebraParameterDescription final {
	string name_;
	string description_;
	unique_ptr<ParameterType> Parameters::*p_;
public:
	LieAlgebraParameterDescription(string name, string description, unique_ptr<ParameterType> Parameters::*p)
		: name_{name}, description_{description}, p_{p}
 {}
	string name() const {return name_;}
	string description() const {return description_;}
	void fill(Parameters& parameters, const po::variables_map& command_line_variable_map) const {
		if (!command_line_variable_map.count(name_)) throw MissingParameter(name_);
		auto& variable=command_line_variable_map[name_].as<string>();
		parameters.*p_=make_unique<AbstractLieGroup<false>>(variable.c_str());
	}
	void add_option_description(po::options_description& options) const {
		options.add_options()(name_.c_str(), po::value<string>(),description_.c_str());
	}
};

template<typename Parameters, typename ParameterType>
struct lie_algebra_tag {
	unique_ptr<ParameterType> Parameters::*p;
};

template<typename Parameters, typename ParameterType>
auto lie_algebra(unique_ptr<ParameterType> Parameters::*p) {
	return lie_algebra_tag<Parameters,ParameterType>{p};
}

template<typename Parameters, typename ParameterType, typename... T>
auto tuple_of_parameter_descriptions(const string& name, const string& description,
		lie_algebra_tag<Parameters,ParameterType> tag,	T... otherParameters) {
	LieAlgebraParameterDescription<Parameters,ParameterType> parameter_description(name,description,tag.p);
	return insert_in_tuple(parameter_description,tuple_of_parameter_descriptions(otherParameters...));
}

matrix metric_from_deflats(const LieGroup& G, const exvector& deflat) {
	matrix g(G.Dimension(),G.Dimension());
	for (int i=0;i<G.Dimension();++i)
	for (int j=0;j<G.Dimension();++j)
		g(i,j)=Hook(G.e()[i],deflat[j]);
	return g;
}

template<typename Parameters, typename ParameterType, typename GroupType>
class PseudoRiemannianMetricParameterDescription final {
	string name_;
	string description_;
	GroupType Parameters::*G;
	unique_ptr<ParameterType> Parameters::*p_;
public:
	PseudoRiemannianMetricParameterDescription(string name, string description,
			GroupType Parameters::*G, unique_ptr<ParameterType> Parameters::*p)
		: name_{name}, description_{description}, G{G}, p_{p}
 {}
	string name() const {return name_;}
	string description() const {return description_;}
	void fill(Parameters& parameters, const po::variables_map& command_line_variable_map) const {
		if (!command_line_variable_map.count(name_)) throw MissingParameter(name_);
		auto& variable=command_line_variable_map[name_].as<string>();
		auto& group=*(parameters.*G);
		auto deflat=ParseDifferentialForms(group.e(),variable.c_str());
		parameters.*p_=make_unique<PseudoRiemannianStructureByMatrix>(&group,group.e(),metric_from_deflats(group,deflat));
	}
	void add_option_description(po::options_description& options) const {
		options.add_options()(name_.c_str(), po::value<string>(),description_.c_str());
	}
};

template<typename Parameters, typename ParameterType, typename GroupType>
struct pseudo_riemannian_metric_tag {
	GroupType Parameters::*G;
	unique_ptr<ParameterType> Parameters::*p;
};

template<typename Parameters, typename ParameterType, typename GroupType>
auto pseudo_riemannian_metric(GroupType Parameters::*G, unique_ptr<ParameterType> Parameters::*p) {
	return pseudo_riemannian_metric_tag<Parameters,ParameterType,GroupType>{G,p};
}

template<typename Parameters, typename ParameterType, typename GroupType, typename... T>
auto tuple_of_parameter_descriptions(const string& name, const string& description,
		pseudo_riemannian_metric_tag<Parameters,ParameterType,GroupType> tag,	T... otherParameters) {
	PseudoRiemannianMetricParameterDescription<Parameters,ParameterType,GroupType> parameter_description(name,description,tag.G,tag.p);
	return insert_in_tuple(parameter_description,tuple_of_parameter_descriptions(otherParameters...));
}

