#pragma once

#include "common/assert.hpp"

#define MIRROW_ASSERT(expr, msg) Assert(expr, msg)
#include "mirrow/drefl/drefl.hpp"
#include "mirrow/serd/dynamic/backends/tomlplusplus.hpp"
#include "mirrow/serd/static/backends/tomlplusplus.hpp"
#include "mirrow/srefl/reflect.hpp"
#include "mirrow/util/const_str.hpp"
#include "mirrow/util/function_traits.hpp"
#include "mirrow/util/misc.hpp"
#include "mirrow/util/type_list.hpp"
#include "mirrow/util/variable_traits.hpp"

#define GECS_ASSERT(expr, msg) Assert(expr, msg)
#define GECS_TYPE_INFO_TYPE const ::mirrow::drefl::type*
#define GECS_GET_TYPE_INFO(type) ::mirrow::drefl::typeinfo<type>()
#define GECS_ANY ::mirrow::drefl::any
#define GECS_MAKE_ANY_REF(x) ::mirrow::drefl::any_make_ref(x)
#include "gecs/gecs.hpp"

#include "common/singlton.hpp"

namespace nickel {

class ECS: public Singlton<ECS, false> {
public:
    gecs::world& World();
    const gecs::world& World() const;

private:
    gecs::world world_;
};

}