#include "MsgpackUtils.hpp"

std::vector<std::map<msgpack::type::variant, msgpack::type::variant>> extractVectorOfMap(const msgpack::type::variant& obj) {
    std::vector<std::map<msgpack::type::variant, msgpack::type::variant>> res;
    auto vec = obj.as_vector();
    std::transform(vec.begin(), vec.end(), std::back_inserter(res), [](auto& o) {
        std::map<msgpack::type::variant, msgpack::type::variant> m;
        for (auto& p : o.as_multimap()) {
            m.insert(p);
        }
        return m;
    });
    return res;
}
