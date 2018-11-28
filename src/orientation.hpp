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

#ifndef ORIENTATION_HPP
#define ORIENTATION_HPP

#include <cmath>

namespace Orientation {
    enum Orientation : unsigned int {N, E, S, W};

    constexpr Orientation opposite(Orientation ori) {
        switch (ori) {
            case N: return S;
            case E: return W;
            case S: return N;
            case W: return E;
        }
        return N;
    }

    constexpr Orientation left(Orientation ori) {
        switch (ori) {
            case N: return W;
            case E: return N;
            case S: return E;
            case W: return S;
        }
        return N;
    }

    constexpr Orientation right(Orientation ori) {
        switch (ori) {
            case N: return E;
            case E: return S;
            case S: return W;
            case W: return N;
        }
        return N;
    }

    constexpr int dx(Orientation ori) {
        switch (ori) {
            case N: return 0;
            case E: return 1;
            case S: return 0;
            case W: return -1;
        }
        return 0;
    }

    constexpr int dy(Orientation ori) {
        switch (ori) {
            case N: return -1;
            case E: return 0;
            case S: return 1;
            case W: return 0;
        }
        return 0;
    }

    constexpr double angle(Orientation ori) {
        switch (ori) {
            case N: return 0;
            case E: return 90;
            case S: return 180;
            case W: return 270;
        }
        return 0;
    }

    constexpr double angle_diff(Orientation ori, double ang) {
        auto d = std::remainder(std::abs(angle(ori) - ang), 360);
        auto d2 = d < 0 ? d + 360 : d;
        return std::min(d2, 360 - d2);
    }
}

#endif // ORIENTATION_HPP
