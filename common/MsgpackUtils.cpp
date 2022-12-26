#include "MsgpackUtils.hpp"
#include <QDebug>

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

double extractDouble(const msgpack::type::variant& obj) {
    if (obj.is_double()) return obj.as_double();
    if (obj.is_int64_t()) return obj.as_int64_t();
    if (obj.is_uint64_t()) return obj.as_uint64_t();
    qFatal("Msgpack object does not contain double, int or uint");
}

int extractInt(const msgpack::type::variant& obj) {
    if (obj.is_int64_t()) return obj.as_int64_t();
    if (obj.is_uint64_t()) return obj.as_uint64_t();
    qFatal("Msgpack object does not contain int or uint");
}

void debugPrintMsgpackMap(const std::map<msgpack::type::variant, msgpack::type::variant>& obj) {
    for (auto& p : obj) {
        int s = 0;
        qDebug() << p.first.as_string().c_str() << abi::__cxa_demangle(p.second.type().name(), 0, 0, &s);
    }
}
