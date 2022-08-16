#ifndef PAIR_FROM_H
#define PAIR_FROM_H

/** @brief returns a pair object from a string. The string should be formatted as a sequence of two strings convertible to U and V, interleaved by a single character */
template<typename U, typename V>
std::pair<U,V> pair_from_string(const std::string& s) {
	std::pair<U,V> result;
	std::stringstream str{s};
	str>>result.first;
	str.ignore();
	str>>result.second;
	return result;
}
#endif
