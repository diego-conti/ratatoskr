#ifndef ASUNIQUE_H
#define ASUNIQUE_H

namespace ratatoskr {

//function to convert an rvalue to a unique_ptr. 
template<typename T, typename U=remove_reference_t<T>>
unique_ptr<U> as_unique(T&& object) {
	static_assert(is_rvalue_reference_v<T&&>, "as_unique should be called on rvalues returned by constructor-like functions");
	return make_unique<U>(std::move(object));
}
}
#endif