#ifndef TUPLE_CONTAINER_H
#define TUPLE_CONTAINER_H
#include <type_traits>
#include <tuple>
namespace ratatoskr {

template<typename F, typename Tuple, int n=0, enable_if_t<n<tuple_size_v<Tuple>,int> =0>
void iterate_over_tuple(const F& function, const Tuple& tuple) {
	function(std::get<n>(tuple));
	iterate_over_tuple<F,Tuple,n+1>(function,tuple);
}

template<typename F, typename Tuple, int n=0,enable_if_t<n>=tuple_size_v<Tuple>,int> =0>
void iterate_over_tuple(const F& function, const Tuple& tuple) {
}

template<typename T, typename Tuple>
auto insert_in_tuple(T&& t, Tuple&& tuple) {
	return tuple_cat(make_tuple(forward<T>(t)),forward<Tuple>(tuple));
}
}
#endif
