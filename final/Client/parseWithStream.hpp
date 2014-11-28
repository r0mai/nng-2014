

#ifndef NNG_PARSEWITHSTREAM_HPP_
#define NNG_PARSEWITHSTREAM_HPP_

#include <string>
#include <iterator>
#include <istream>
#include <sstream>

template<class... Ts>
bool parseWithStream(const std::string& str, Ts&...);

template<class... Ts>
bool parseWithStream(std::istream& ss, Ts&...);

namespace parse_impl {

inline
bool parseWithStream(std::istream& ss) {
	//Check if the remaning stream only contains blanks
	return std::istream_iterator<std::string>(ss) == std::istream_iterator<std::string>();
}

template<class Head, class... Ts>
bool parseWithStream(std::istream& ss, Head& head, Ts&... tail) {
	return ss >> head && parseWithStream( ss, tail... );
}

} //namespace parse_impl

template<class... Ts>
bool parseWithStream(const std::string& str, Ts&... tail) {
	std::stringstream ss(str);
	return parse_impl::parseWithStream( ss, tail... );
}

template<class... Ts>
bool parseWithStream(std::istream& ss, Ts&... tail) {
	return parse_impl::parseWithStream( ss, tail... );
}

#endif /* NNG_PARSEWITHSTREAM_HPP_ */
