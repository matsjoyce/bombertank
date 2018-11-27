/*
 * <one line to give the program's name and a brief idea of what it does.>
 * Copyright (C) 2018  <copyright holder> <email>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef MSGPACK_UTILS_HPP
#define MSGPACK_UTILS_HPP

#include <msgpack.hpp>

using msgpackvar = std::map<std::string, msgpack::type::variant>;

inline int64_t extract_int(msgpack::type::variant var) {
    return var.is_uint64_t() ? var.as_uint64_t() : var.as_int64_t();
}

template<class T> unsigned int as_ui(T x) {
    return static_cast<unsigned int>(x);
}

std::ostream& operator<<(std::ostream& stream, msgpack::type::variant var);

#endif // MSGPACK_UTILS_HPP
