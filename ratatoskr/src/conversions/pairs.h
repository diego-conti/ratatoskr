namespace ratatoskr {
template<typename Parameters, typename First, typename Second>
auto comma_separated_pair(pair<First,Second> Parameters::*p) {
	auto converter=[] (const string& values) {
		pair<First,Second> result;
		stringstream s{values};
		s>>result.first;
		char c;
		s>>c;
		if (c!=',') throw ConversionError(values+ ": expected , found "+c);
		s>>result.second;
		return result;
	};
	return generic_converter(p,converter);
}

//specialization for pair<string,?>
template<typename Parameters, typename Second>
auto comma_separated_pair(pair<string,Second> Parameters::*p) {
	auto converter=[] (const string& values) {
		pair<string,Second> result;
		int i=values.find(',');
		if (i==string::npos) throw ConversionError(values+ " does not contain ,");
		result.first=values.substr(0,i);
		stringstream s{values.substr(i+1)};
		s>>result.second;
		return result;
	};
	return generic_converter(p,converter);
}
}
