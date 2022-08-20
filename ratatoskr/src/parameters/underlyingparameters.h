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
template<typename Parameters, typename TupleOfParameterDescriptions>
class SequenceOfParameterDescriptions;

template<typename Parameters,typename ParameterType,typename Converter, typename RequiredParameters, typename BoostParameterType>
class DependentParameterDescription;

template<typename Parameters,typename ParameterType,typename Initializer, typename RequiredParameters>
class OptionDescription;

template<typename Parameters, typename TupleOfAlternatives>
class AlternativeParameterDescriptions;

//specializations
template<typename Parameters, typename TupleOfAlternatives>
struct UnderlyingParameters<AlternativeParameterDescriptions<Parameters,TupleOfAlternatives>> {
	using type=Parameters;
};

template<typename Parameters, typename TupleOfAlternatives>
struct UnderlyingParameters<SequenceOfParameterDescriptions<Parameters,TupleOfAlternatives>> {
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
