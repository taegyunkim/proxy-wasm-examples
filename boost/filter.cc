// NOLINT(namespace-envoy)
#include <string>
#include <tuple>
#include <type_traits>
#include <unordered_map>

#include "boost/callable_traits.hpp"
#include "proxy_wasm_intrinsics.h"

namespace ct = boost::callable_traits;

// This function template helps keep our example code neat
template <typename A, typename B> void assert_same() {
  static_assert(std::is_same<A, B>::value, "");
}

// foo is a function object
struct foo {
  void operator()(int, char, float) const {}
};

class HttpAuthRootContext : public RootContext {
public:
  explicit HttpAuthRootContext(uint32_t id, StringView root_id)
      : RootContext(id, root_id) {
    // Use args_t to retrieve a parameter list as a std::tuple:
    assert_same<ct::args_t<foo>, std::tuple<int, char, float>>();
  }
};

class HttpAuthContext : public Context {
public:
  explicit HttpAuthContext(uint32_t id, RootContext *root)
      : Context(id, root),
        root_(static_cast<HttpAuthRootContext *>(static_cast<void *>(root))) {}

private:
  HttpAuthRootContext *root_;
};
static RegisterContextFactory
    register_HttpAuthContext(CONTEXT_FACTORY(HttpAuthContext),
                             ROOT_FACTORY(HttpAuthRootContext), "http_auth");
