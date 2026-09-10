/*
** TimeCapsule: A videogame engine.
** Copyright (C) 2026 Rivest Osz
** 
** This file is part of "TimeCapsule".
** 
** "TimeCapsule" is free software: you can redistribute it and/or modify it under the terms of
** the GNU General Public License as published by the Free Software Foundation, either
** version 3 of the License, or (at your option) any later version.
** 
** "TimeCapsule" is distributed in the hope that it will be useful, but WITHOUT ANY
** WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
** A PARTICULAR PURPOSE. See the GNU General Public License for more details.
** 
** You should have received a copy of the GNU General Public License along with
** "TimeCapsule". If not, see <https://www.gnu.org/licenses/>. 
**/

#include "math.hpp"

float clampf(float x, float min, float max) {
	if(x < min) return min;
	if(x > max) return max;
	return x;
};

float vdistance(float a, float b) {
	return std::sqrt(std::pow(b - a, 2.f));
};
float vdistance(Raylib::Vector2 a, Raylib::Vector2 b) {
	return std::sqrt(
		std::pow(b.x - a.x, 2.f)
		+ std::pow(b.y - a.y, 2.f)
	);
};
float vdistance(Raylib::Vector3 a, Raylib::Vector3 b) {
	return std::sqrt(
		std::pow(b.x - a.x, 2.f)
		+ std::pow(b.y - a.y, 2.f)
		+ std::pow(b.z - a.z, 2.f)
	);
};

float min(float a, float b) {
	return a < b ? a : b;
};
float max(float a, float b) {
	return a > b ? a : b;
};

float lerp(float a, float b, float t) {
	return a + (b - a) * t;
};
timecapsule::vector2_t lerp(timecapsule::vector2_t a, timecapsule::vector2_t b, float t) {
	return timecapsule::vector2_t {
		lerp(a.x, b.x, t),
		lerp(a.y, b.y, t),
	};
};
timecapsule::vector3_t lerp(timecapsule::vector3_t a, timecapsule::vector3_t b, float t) {
	return timecapsule::vector3_t {
		lerp(a.x, b.x, t),
		lerp(a.y, b.y, t),
		lerp(a.z, b.z, t),
	};
};

float dot(timecapsule::vector2_t a, timecapsule::vector2_t b) {
	return a.x * b.x + a.y * b.y;
};
float dot(timecapsule::vector3_t a, timecapsule::vector3_t b) {
	return a.x * b.x + a.y * b.y + a.z * b.z;
};

timecapsule::vector2_t subtract_vec(timecapsule::vector2_t a, timecapsule::vector2_t b) {
	timecapsule::vector2_t r;
	r.x = a.x - b.x;
	r.y = a.y - b.y;
	return r;
};
timecapsule::vector3_t subtract_vec(timecapsule::vector3_t a, timecapsule::vector3_t b) {
	timecapsule::vector3_t r;
	r.x = a.x - b.x;
	r.y = a.y - b.y;
	r.z = a.z - b.z;
	return r;
};
