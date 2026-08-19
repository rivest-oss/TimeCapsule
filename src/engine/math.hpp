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

#pragma once
#include <cmath>
#include "gl.hpp"

namespace Raylib {
	#include <raymath.h>
};

#undef M_PI
#undef M_PI2
#undef M_PIf
#undef M_PI2f

#define M_PI  3.14159265358979323846264338327950288419716939937510
#define M_PI2 6.28318530717958647692528676655900576839433879875020

#define M_PIf  3.14159265358979323846264338327950288419716939937510f
#define M_PI2f 6.28318530717958647692528676655900576839433879875020f

float clampf(float x, float min = -1.f, float max = 1.f);

float vdistance(float a, float b);
float vdistance(Raylib::Vector2 a, Raylib::Vector2 b);
float vdistance(Raylib::Vector3 a, Raylib::Vector3 b);

float min(float a, float b);
float max(float a, float b);
