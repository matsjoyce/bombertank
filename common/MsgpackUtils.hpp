#ifndef MSGPACK_UTILS_HPP
#define MSGPACK_UTILS_HPP

#include <vector>
#include <map>
#include <msgpack.hpp>

std::vector<std::map<msgpack::type::variant, msgpack::type::variant>> extractVectorOfMap(const msgpack::type::variant& obj);
double extractDouble(const msgpack::type::variant& obj);
int extractInt(const msgpack::type::variant& obj);
void debugPrintMsgpackMap(const std::map<msgpack::type::variant, msgpack::type::variant>& obj);

#endif // MSGPACK_UTILS_HPP
