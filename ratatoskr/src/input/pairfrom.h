#ifndef PAIR_FROM_H
#define PAIR_FROM_H
namespace ratatoskr {
/** @brief returns a pair object from a string. The string should be formatted as a sequence of two strings convertible to U and V, interleaved by a single character */

class PairParseError : public std::runtime_error {
public:
	PairParseError(const string& value, char expected) : std::runtime_error{
		value+" is not a pair of values separated by `"+expected+"'"
	} {}
};

template<typename U>
U from_string(const string& s) {
	U result;
	std::stringstream str{s};
	str>>result;
	return result;
}

template<typename U, typename V>
std::pair<U,V> pair_from_string(const std::string& s, char sep) {
	vector<string> elements=splice(s,sep);
	if (elements.size()!=2) throw PairParseError(s,sep);
	return make_pair(from_string<U>(elements[0]), from_string<V>(elements[1]));
}

template<typename U, typename V>
std::pair<U,V> pair_from_csv(const std::string& s) {
	return pair_from_string<U,V>(s,',');
}

}
#endif
