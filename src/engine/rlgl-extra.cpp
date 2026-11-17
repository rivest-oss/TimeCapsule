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

#include "rlgl-extra.hpp"
#include <GL/gl.h>

#if defined(GRAPHICS_API_OPENGL_21) || defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_43)
	double active_query_t;

	void Raylib::rlBeginQuery(void) {
		typedef void (* glFinish_t)(void);
		glFinish_t f = (glFinish_t)Raylib::rlGetProcAddress("glFinish");

		f();
		active_query_t = Raylib::GetTime();
	};

	void Raylib::rlEndQuery(void) {
		typedef void (* glFinish_t)(void);
		glFinish_t f = (glFinish_t)Raylib::rlGetProcAddress("glFinish");

		f();
		active_query_t = Raylib::GetTime() - active_query_t;
	};

	bool Raylib::rlIsQueryResultReady(void) {
		return true;
	};

	double Raylib::rlGetQueryResult(void) {
		return active_query_t;
	};

#elif defined(GRAPHICS_API_OPENGL_ES2)
#elif defined(GRAPHICS_API_OPENGL_SOFTWARE)
#else
	#error "Unsupported graphics API"
#endif
