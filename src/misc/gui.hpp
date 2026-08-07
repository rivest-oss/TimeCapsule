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
#include <string>
#include <unordered_map>

namespace Raylib {
	#include <raylib.h>
};

namespace timecapsule {
	/*
	namespace gui {
		typedef struct _system_t {
			const char *name;
			Raylib::Vector2 (*update)(Raylib::Vector2 recommended_size);
			void (*draw)(Raylib::Vector2 position);
		} _system_t;
		
		static constexpr int MAX_SYSTEMS = 100'000'000;
		static _system_t _systems[MAX_SYSTEMS];
		static const char *_systems_files[MAX_SYSTEMS];
		static int _systems_lines[MAX_SYSTEMS];
		static int _systems_count;
		
		static void _register_system(_system_t sys, const char *file, int line) {
			_systems[_systems_count] = sys;
			_systems_files[_systems_count] = file;
			_systems_lines[_systems_count] = line;
			_systems_count++;
		};
		
		#define CONCAT_IMPL(x,y) x##y
		#define MACRO_CONCAT(x,y) CONCAT_IMPL(x, y)
		#define _TIMECAPSULE_REGISTER_SYSTEM(sys) \
			static int MACRO_CONCAT(_reg_, __COUNTER__) = (timecapsule::gui::_register_system(sys, __FILE__, __LINE__), 0)
		
		int init(void);
		void deinit(void);
		
		void set_default_font(Raylib::Font font);
		void set_default_color(std::string id, Raylib::Color color);
		
		class Node {
			private:
				unsigned int p_state = 0;
				std::string p_type;
				
				Raylib::Vector2 p_size;
				
				std::unordered_map<std::string, float> p_floats;
				std::unordered_map<std::string, Raylib::Color> p_colors;
				std::unordered_map<std::string, Raylib::Image> p_images;
				std::unordered_map<std::string, Raylib::Texture2D> p_textures;
				std::unordered_map<std::string, Raylib::Font> p_fonts;
				std::unordered_map<std::string, Node> p_nodes;
			
			public:
				Node(void);
				~Node(void);
				
				void clear(void);
				
				Raylib::Vector2 get_size(void);
				void set_color(std::string id, Raylib::Color color);
				
				Raylib::Vector2 update(Raylib::Vector2 recommended_size);
				void draw(Raylib::Vector2 position);
				
				int set_type(std::string type);
		};
	};
	*/
};
