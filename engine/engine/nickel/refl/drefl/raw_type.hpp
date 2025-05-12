#pragma once

#include "nickel/refl/util/misc.hpp"
#include "nickel/refl/util/variable_traits.hpp"

namespace nickel::refl {

template <typename T>
using property_raw_t = remove_cvref_t<remove_all_pointers_t<
    remove_cvref_t<typename variable_traits<T>::type>>>;

template <typename T>
using property_no_qualifier = remove_cvref_t<typename variable_traits<T>::type>;

template <typename T>
using raw_type_t =
    remove_cvref_t<remove_all_pointers_t<remove_cvref_t<T>>>;

}