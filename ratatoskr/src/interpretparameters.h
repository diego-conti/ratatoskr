#include "commandlineparameters.h"
#include <type_traits>


template<typename Parameters, typename ParameterType,typename Converter, typename RequiredParameters=tuple<>>
class DependentParameterDescription final {
	string name_;
	string description_;
	unique_ptr<ParameterType> Parameters::*p_;
	Converter converter;
	RequiredParameters required_parameters;
public:
	template <typename U=RequiredParameters>
	DependentParameterDescription(typename std::enable_if_t<true,string> name, string description,
			unique_ptr<ParameterType> Parameters::*p, Converter& converter)
		: name_{name}, description_{description}, p_{p}, converter{converter}
		{}

	DependentParameterDescription(string name, string description,
			unique_ptr<ParameterType> Parameters::*p, Converter& converter,const RequiredParameters& required_parameters)
		: name_{name}, description_{description}, p_{p}, converter{converter}, required_parameters{required_parameters}
		{}
	string name() const {return name_;}
	string description() const {return description_;}
	void fill(Parameters& parameters, const po::variables_map& command_line_variable_map) const {
		if (!command_line_variable_map.count(name_)) throw MissingParameter(name_);
		auto& text=command_line_variable_map[name_].as<string>();
		auto bind = [&parameters] (auto... pointers) {return tie(parameters.*pointers...);};
		auto bound_required_parameters=std::apply(bind, required_parameters);
		parameters.*p_=std::apply(converter,insert_in_tuple(text,bound_required_parameters));
	}
	void add_option_description(po::options_description& options) const {
		options.add_options()(name_.c_str(), po::value<string>(),description_.c_str());
	}
};

template<typename Parameters, typename ParameterType, typename Converter, typename RequiredParameters=tuple<>>
struct dependent_parameter_tag {
	unique_ptr<ParameterType> Parameters::*p;
	Converter converter;
	RequiredParameters required_parameters;	//tuple of pointers-to-member for class Parameters
};


template<typename Parameters, typename ParameterType, typename Converter, typename RequiredParameters, typename... T>
auto tuple_of_parameter_descriptions(const string& name, const string& description,
		dependent_parameter_tag<Parameters,ParameterType,Converter,RequiredParameters> tag,	T... otherParameters) {
	DependentParameterDescription<Parameters,ParameterType,Converter,RequiredParameters> parameter_description{name,description,tag.p,tag.converter,tag.required_parameters};
	return insert_in_tuple(parameter_description,tuple_of_parameter_descriptions(otherParameters...));
}


template<typename Parameters, typename ParameterType>
auto lie_algebra(unique_ptr<ParameterType> Parameters::*p) {
	auto converter=[] (const string& parameter) {
		return make_unique<AbstractLieGroup<false>>(parameter.c_str());
	};
	return dependent_parameter_tag<Parameters,ParameterType,decltype(converter)>{p,move(converter)};
}

matrix metric_from_deflats(const LieGroup& G, const exvector& deflat) {
	matrix g(G.Dimension(),G.Dimension());
	for (int i=0;i<G.Dimension();++i)
	for (int j=0;j<G.Dimension();++j)
		g(i,j)=Hook(G.e()[i],deflat[j]);
	return g;
}

template<typename Parameters, typename ParameterType, typename... RequiredParameters>
auto pseudo_riemannian_metric(unique_ptr<ParameterType> Parameters::*p,RequiredParameters... required_parameters) {
	auto tuple=make_tuple(required_parameters...);
	auto converter=[] (const string& parameter, unique_ptr<LieGroup>& G) {
		auto deflat=ParseDifferentialForms(G->e(),parameter.c_str());
		return make_unique<PseudoRiemannianStructureByMatrix>(G.get(),G->e(),metric_from_deflats(*G,deflat).inverse());
	};
	return dependent_parameter_tag<Parameters,ParameterType,decltype(converter),decltype(tuple)>{p,move(converter),move(tuple)};
}

