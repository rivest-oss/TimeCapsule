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
	namespace gui {
		enum : int {
			NODE_TYPE_INVALID = 0,
			NODE_TYPE_TEXT,
		};
		
		int init(void);
		void deinit(void);
		
		void set_default_font(Raylib::Font font);
		void set_default_color(std::string id, Raylib::Color color);
		
		class Node {
			private:
				unsigned int state = 0;
				int type = NODE_TYPE_INVALID;
				
				std::string p_text;
				Raylib::Font p_font;
				
				std::unordered_map<std::string, Raylib::Color> p_colors;
			
			public:
				int get_type(void);
				Raylib::Vector2 get_size(void);
				
				void update(Raylib::Vector2 recommended_size);
				void draw(Raylib::Vector2 position);
				
				// basic
				void set_type_text(const char *text);
				// extended
				void set_type_text(
					const char *text,
					Raylib::Font font,
					Raylib::Color text_color
				);
		};
	};
};
