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
	GLuint _preallocated_query_id[64];
	int _preallocated_query_index = 0;

	void Raylib::rlExtrasInit(void) {
		typedef void (* glGenQueries_t)(GLsizei n, GLuint *ids);
		glGenQueries_t f = (glGenQueries_t)Raylib::rlGetProcAddress("glGenQueries");

		f(sizeof(_preallocated_query_id) / sizeof(unsigned int), _preallocated_query_id);

		_preallocated_query_index = 0;
	};

	void Raylib::rlExtrasDeinit(void) {
		typedef void (* glDeleteQueries_t)(GLsizei n, GLuint *ids);
		glDeleteQueries_t f = (glDeleteQueries_t)Raylib::rlGetProcAddress("glDeleteQueries");

		f(sizeof(_preallocated_query_id) / sizeof(unsigned int), _preallocated_query_id);
	};

	unsigned int Raylib::rlBeginQuery(void) {
		unsigned int id = _preallocated_query_id[_preallocated_query_index++];

		typedef void (* glQueryCounter_t)(GLuint id, GLenum target);
		glQueryCounter_t f = (glQueryCounter_t)Raylib::rlGetProcAddress("glQueryCounter");

		f(id, GL_TIMESTAMP);

		return id;
	};

	bool Raylib::rlIsQueryResultReady(unsigned int id) {
		typedef void (* glGetQueryObjectiv_t)(GLuint id, GLenum pname, GLint *params);
		glGetQueryObjectiv_t f = (glGetQueryObjectiv_t)Raylib::rlGetProcAddress("glGetQueryObjectiv");

		GLint r;
		f(id, GL_QUERY_RESULT_AVAILABLE, &r);

		return r != 0;
	};

	unsigned long Raylib::rlGetQueryResultI64(unsigned int id) {
		typedef void (* glGetQueryObjectui64v_t)(GLuint id, GLenum pname, GLuint64 *params);
		glGetQueryObjectui64v_t f = (glGetQueryObjectui64v_t)Raylib::rlGetProcAddress("glGetQueryObjectui64v");

		GLuint64 r;
		f(id, GL_QUERY_RESULT, &r);

		return r;
	};

	double Raylib::rlGetQueryResultF64(unsigned int id) {
		unsigned long r = rlGetQueryResultI64(id);
		if(r == 0lu) return 0.0;

		return ((double)r) / 1000000000.0;
	};

	void Raylib::rlClearQueries(void) {
		_preallocated_query_index = 0;
	};
#elif defined(GRAPHICS_API_OPENGL_ES2) || defined(GRAPHICS_API_OPENGL_ES3)
	#error "Unsupported graphics API"
#elif defined(GRAPHICS_API_OPENGL_SOFTWARE)
#else
	#error "Unsupported graphics API"
#endif
