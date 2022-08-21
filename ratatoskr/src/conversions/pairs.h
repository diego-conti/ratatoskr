namespace ratatoskr {

template<typename Parameters, typename First, typename Second>
auto comma_separated_pair(pair<First,Second> Parameters::*p) {
	auto converter=[] (const string& values) {
		try {
			return pair_from_csv<First,Second>(values);
		}
		catch (PairParseError& e) {
			throw ConversionError(e.what());
		}
	};
	return generic_converter(p,converter);
}
}
