#include <type_traits>
#include <tuple>

template<typename F, typename Tuple, int n=0, enable_if_t<n<tuple_size_v<Tuple>,int> =0>
void iterate_over_tuple(F function, const Tuple& tuple) {
	function(std::get<n>(tuple));
	iterate_over_tuple<F,Tuple,n+1>(function,tuple);
}

template<typename F, typename Tuple, int n=0,enable_if_t<n>=tuple_size_v<Tuple>,int> =0>
void iterate_over_tuple(F function, const Tuple& tuple) {
}
