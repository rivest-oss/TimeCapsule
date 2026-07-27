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

#include "gui.hpp"
#include "asset.hpp"
#include <cstdio>

namespace timecapsule {
	namespace gui {
		Raylib::Font default_font;
		std::unordered_map<std::string, Raylib::Color> default_colors;
		
		int init(void) {
			default_font = Raylib::GetFontDefault();
			default_colors.clear();
			
			default_colors.insert_or_asign("text", Raylib::BLACK);
			
			return 0;
		};
		
		void deinit(void) {
			if(Raylib::IsFontValid(default_font))
				Raylib::UnloadFont(default_font);
			
			default_colors.clear();
		};
		
		Raylib::Font get_default_font(void) {
			return	Raylib::IsFontValid(default_font)
						? default_font
						: Raylib::GetFontDefault();
		};
		
		Raylib::Color get_default_color(std::string id) {
			if(default_colors.find(id) != default_colors.cend())
				return default_colors.at(id);
			
			return Raylib::MAGENTA;
		};
		
		void set_default_font(Raylib::Font font) {
			if(Raylib::IsFontValid(font) == false) {
				std::printf("[TimeCapsule/GUI] set_default_font(): Invalid 'font'.\n");
				return;
			}
			
			default_font = font;
		};
		
		void set_default_color(std::string id, Raylib::Color color) {
			if(id.size() < 1) {
				std::printf("[TimeCapsule/GUI] set_default_color(): Invalid 'id'.\n");
				return;
			}
			
			default_colors.insert_or_assign(id, color);
		};
		
		// Node
		
		Node::Node(void) {
			p_image.width = p_image.height = -1;
			reset_node();
		};
		
		Node::~Node(void) {
			reset_node();
		};
		
		void Node::reset_node(void) {
			p_colors.clear();
			
			if(Raylib::IsImageValid(p_image))
				unload_image(p_image);
			
			state = 0;
			type = NODE_TYPE_INVALID;
			p_text = "";
			p_font = Raylib::GetFontDefault();
			p_colors.clear();
			p_image.width = p_image.height = -1;
			p_size.x = p_size.y = 0.f;
		};
		
		int Node::get_type(void) {
			return type;
		};
		
		Raylib::Vector2 Node::get_size(void) {
			return p_size;
		};
		
		void Node::update(Raylib::Vector2 recommended_size) {
			if(type == NODE_TYPE_INVALID) {
				// don't do anything
			}
			
			else if(type == NODE_TYPE_TEXT) {
				p_size = Raylib::MeasureTextEx(
					p_font,
					p_text.c_str(),
					16.f,
					0.f
				);
			}
			
			else {
				// [TODO]
			}
		};
		
		void Node::draw(Raylib::Vector2 position) {
			// [TODO]
		};
		
		void Node::set_type_text(const char *text) {
			set_type_text(
				text,
				get_default_font(),
				get_default_color("text")
			);
		};
		
		void Node::set_type_text(
			const char *text,
			Raylib::Font text_font,
			Raylib::Color text_color
		) {
			state = 0;
			type = NODE_TYPE_TEXT;
			
			if(text != nullptr) {
				p_text = text;
			} else {
				std::printf("[TimeCapsule/GUI] Node::set_type_text(): Invalid 'text'.\n");
				p_text = "";
			}
			
			if(Raylib::IsFontValid(text_font)) {
				p_font = text_font;
			} else {
				std::printf("[TimeCapsule/GUI] Node::set_type_text(): Invalid 'font'.\n");
				p_font = get_default_font();
			}
			
			p_colors.insert_or_assign("text", text_color);
		};
	};
};
