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
	/*
	namespace gui {
		Raylib::Font default_font;
		std::unordered_map<std::string, Raylib::Color> default_colors;
		
		int init(void) {
			default_font = Raylib::GetFontDefault();
			default_colors.clear();
			
			default_colors.insert_or_assign("text", Raylib::BLACK);
			
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
			p_images.clear();
			p_textures.clear();
			p_fonts.clear();
			p_nodes.clear();
			clear();
		};
		
		Node::~Node(void) {
			clear();
		};
		
		void Node::clear(void) {
			p_state = 0;
			p_type = "";
			
			p_size.x = p_size.y = 0.f;
			
			for(auto pair : p_images)
				if(Raylib::IsImageValid(pair.second))
					unload_image(pair.second);
			for(auto pair : p_textures)
				if(Raylib::IsTextureValid(pair.second))
					unload_texture(pair.second);
			for(auto pair : p_fonts)
				if(Raylib::IsFontValid(pair.second))
					unload_font(pair.second);
			for(auto pair : p_nodes)
				pair.second.clear();
			
			p_floats.clear();
			p_colors.clear();
			p_images.clear();
			p_textures.clear();
			p_fonts.clear();
			p_nodes.clear();
		};
		
		Raylib::Vector2 Node::get_size(void) {
			return p_size;
		};
		
		void Node::set_color(std::string id, Raylib::Color color) {
			if(id.size() < 1)
				return;
			
			p_colors.insert_or_assign(id, color);
		};
		
		Raylib::Vector2 Node::update(Raylib::Vector2 recommended_size) {
			// [TODO]
			return Raylib::Vector2 { 0.f, 0.f };
		};
		
		void Node::draw(Raylib::Vector2 position) {
			// [TODO]
		};
		
		int Node::set_type(std::string type) {
			clear();
			// [TODO]
			
			return -1;
		};
		
		_TIMECAPSULE_REGISTER_SYSTEM((timecapsule::gui::_system_t {
			.name = "text",
			
			.update = [](Raylib::Vector2 recommended_size) -> Raylib::Vector2 {
				return Raylib::Vector2 { 0.f, 0.f };
			},
			
			.draw = [](Raylib::Vector2 position) -> void {
				//
			},
		}));
	};
	*/
};
