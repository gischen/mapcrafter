/*
 * Copyright 2012-2014 Moritz Hilscher
 *
 * This file is part of Mapcrafter.
 *
 * Mapcrafter is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Mapcrafter is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Mapcrafter.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "terminal.h"

#include "../util.h"

#include <cstdio>

#if defined(OS_WINDOWS)
#  include <io.h>
#  include <windows.h>
#endif
#ifdef HAVE_UNISTD_H
#  include <unistd.h>
#endif

namespace mapcrafter {
namespace util {

bool isOutTTY() {
#ifdef OS_WIN
	return _isatty(_fileno(stdout));
#else
	return isatty(fileno(stdout));
#endif
}

setcolor::setcolor(int type, int color)
	: type(type), color(color) {
}

std::ostream& setcolor::operator()(std::ostream& out) const {
	if (!isAvailable())
		return out;
	if (color == 0) {
		reset(out);
		return out;
	}
#ifdef OS_WINDOWS
	int color_flags = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
	switch (color) {
	case black:
		color_flags = 0;
		break;
	case white:
		color_flags = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
		break;
	case red:
		color_flags = FOREGROUND_RED;
		break;
	case green:
		color_flags = FOREGROUND_GREEN;
		break;
	case blue:
		color_flags = FOREGROUND_BLUE;
		break;
	case yellow:
		color_flags = FOREGROUND_RED | FOREGROUND_GREEN;
		break;
	}
	HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hStdout, color_flags);
	return out;
#else
	return out << "\033[1;" << type + color << "m";
#endif
}

std::ostream& setcolor::reset(std::ostream& out) {
	if (!isAvailable())
		return out;
#ifdef OS_WINDOWS
	HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hStdout, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
	return out;
#else
	return out << "\033[1;0m";
#endif
}

bool setcolor::isAvailable() {
	if (!available_initialized) {
		available_initialized = true;
		available = isOutTTY();
	}
	return available;
}

bool setcolor::available_initialized = false;
bool setcolor::available = false;

std::ostream& operator<<(std::ostream& out, const setcolor& color) {
	return color(out);
}

} /* namespace util */
} /* namespace mapcrafter */