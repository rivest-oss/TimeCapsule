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
			NODE_TYPE_BUTTON_TEXT,
		};
		
		int init(void);
		void deinit(void);
		
		void set_default_font(Raylib::Font font);
		void set_default_color(std::string id, Raylib::Color color);
		
		class Node {
			private:
				unsigned int p_state = 0;
				int p_type = NODE_TYPE_INVALID;
				
				std::string p_text;
				Raylib::Font p_font;
				Raylib::Image p_image;
				Raylib::Vector2 p_size;
				
				std::unordered_map<std::string, Raylib::Color> p_colors;
				std::unordered_map<std::string, float> p_floats;
				std::unordered_map<std::string, Node> p_nodes;
			
			public:
				Node(void);
				~Node(void);
				
				void reset_node(void);
				
				int get_type(void);
				Raylib::Vector2 get_size(void);
				void set_color(std::string id, Raylib::Color color);
				
				Raylib::Vector2 update(Raylib::Vector2 recommended_size);
				void draw(Raylib::Vector2 position);
				
				typedef struct text_t {
					std::string text = "";
					Raylib::Font font = Raylib::Font { 0 };
					Raylib::Color color = Raylib::BLANK;
					
					text_t &set_text(const char *s) { text = s; return *this; };
					text_t &set_text(std::string s) { text = s; return *this; };
					text_t &set_font(Raylib::Font f) { font = f; return *this; };
					text_t &set_color(Raylib::Color c) { color = c; return *this; };
				} text_t;
				
				void set_type_text(text_t options);
				
				typedef struct button_with_text_t {
					std::string text = "";
					Raylib::Font text_font = Raylib::Font { 0 };
					
					Raylib::Color text_color_default = Raylib::BLANK;
					Raylib::Color text_color_hover = Raylib::BLANK;
					// no focus? - rosz
					Raylib::Color text_color_active = Raylib::BLANK;
					Raylib::Color text_color_disabled = Raylib::BLANK;
					
					Raylib::Color background_color_default = Raylib::BLANK;
					Raylib::Color background_color_hover = Raylib::BLANK;
					Raylib::Color background_color_active = Raylib::BLANK;
					Raylib::Color background_color_disabled = Raylib::BLANK;
					
					Raylib::Color border_color = Raylib::BLANK;
					
					float border_roundness = 0.f;
					float border_thickness = 0.f;
					float button_margin = 0.0f;
					
					button_with_text_t &set_text(const char *s) { text = s; return *this; };
					button_with_text_t &set_text(std::string s) { text = s; return *this; };
					button_with_text_t &set_text_font(Raylib::Font f) { text_font = f; return *this; };
					
					button_with_text_t &set_text_color_default(Raylib::Color c) { text_color_default = c; return *this; };
					button_with_text_t &set_text_color_hover(Raylib::Color c) { text_color_hover = c; return *this; };
					button_with_text_t &set_text_color_active(Raylib::Color c) { text_color_active = c; return *this; };
					button_with_text_t &set_text_color_disabled(Raylib::Color c) { text_color_disabled = c; return *this; };
					
					button_with_text_t &set_background_color_default(Raylib::Color c) { background_color_default = c; return *this; };
					button_with_text_t &set_background_color_hover(Raylib::Color c) { background_color_hover = c; return *this; };
					button_with_text_t &set_background_color_active(Raylib::Color c) { background_color_active = c; return *this; };
					button_with_text_t &set_background_color_disabled(Raylib::Color c) { background_color_disabled = c; return *this; };
					
					button_with_text_t &set_border_roundness(float r) { border_roundness = r; return *this; };
					button_with_text_t &set_border_thickness(float t) { border_thickness = t; return *this; };
					button_with_text_t &set_text_border_margin(float m) { button_margin = m; return *this; };
				} button_with_text_t;
				
				void set_type_button_with_text(button_with_text_t options);
		};
	};
};
