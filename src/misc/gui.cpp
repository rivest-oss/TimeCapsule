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
		
		/*
		Node::Node(void) {
			p_image.width = p_image.height = -1;
			reset_node();
		};
		
		Node::~Node(void) {
			reset_node();
		};
		
		void Node::reset_node(void) {
			p_colors.clear();
			p_floats.clear();
			p_nodes.clear();
			
			if(Raylib::IsImageValid(p_image))
				unload_image(p_image);
			
			p_state = 0;
			p_type = NODE_TYPE_INVALID;
			p_text = "";
			p_font = Raylib::GetFontDefault();
			p_colors.clear();
			p_image.width = p_image.height = -1;
			p_size.x = p_size.y = 0.f;
		};
		
		int Node::get_type(void) {
			return p_type;
		};
		
		Raylib::Vector2 Node::get_size(void) {
			return p_size;
		};
		
		void Node::set_color(std::string id, Raylib::Color color) {
			p_colors.insert_or_assign(id, color);
		};
		
		Raylib::Vector2 Node::update(Raylib::Vector2 recommended_size) {
			if(p_type == NODE_TYPE_INVALID) {
				// don't do anything
				return Raylib::Vector2 { 0.f, 0.f };
			}
			
			else if(p_type == NODE_TYPE_TEXT) {
				const float min_font_size = 8.f;
				Raylib::Vector2 msr;
				
				float font_size;
				
				if(p_floats.find("font size") != p_floats.cend()) {
					font_size = p_floats.at("font size");
					
					msr = Raylib::MeasureTextEx(
						p_font,
						p_text.c_str(),
						font_size,
						0.f
					);
					
					if(msr.x <= recommended_size.x && msr.y <= recommended_size.y) {
						p_size = msr;
						return p_size;
					}
				}
				
				font_size = recommended_size.y;
				if(font_size < min_font_size)
					font_size = min_font_size;
				
				while(font_size > min_font_size) {
					msr = Raylib::MeasureTextEx(
						p_font,
						p_text.c_str(),
						font_size,
						0.f
					);
					
					if(msr.x <= recommended_size.x && msr.y <= recommended_size.y)
						break;
					
					font_size -= 0.5f;
				};
				
				if(font_size < min_font_size)
					font_size = min_font_size;
				
				p_floats.insert_or_assign("font size", font_size);
				
				msr = Raylib::MeasureTextEx(
					p_font,
					p_text.c_str(),
					font_size,
					0.f
				);
				
				p_size = msr;
				return p_size;
			}
			
			else if(p_type == NODE_TYPE_BUTTON_TEXT) {
				p_nodes.clear();
				
				Node text_node;
				
				text_node.set_type_text(
					text_t()
						.set_text(p_text)
						.set_font(p_font)
						.set_color(p_colors.at("text color default"))
				);
				
				Raylib::Vector2 size = text_node.update(Raylib::Vector2 {
					recommended_size.x - (p_floats.at("border thickness") + p_floats.at("button margin")),
					recommended_size.y - (p_floats.at("border thickness") + p_floats.at("button margin")),
				});
				
				p_floats.insert_or_assign("text size x", size.x);
				p_floats.insert_or_assign("text size y", size.y);
				
				p_nodes.insert_or_assign("text", text_node);
				
				size.x += (p_floats.at("border thickness") + p_floats.at("button margin")) * 2.f;
				size.y += (p_floats.at("border thickness") + p_floats.at("button margin")) * 2.f;
				
				return size;
			}
			
			else {
				// [TODO] handle error
				return Raylib::Vector2 { 0.f, 0.f };
			}
		};
		
		enum : int {
			_BUTTON_TEXT_NODE_STATE_DEFAULT = 0,
			_BUTTON_TEXT_NODE_STATE_HOVER,
			_BUTTON_TEXT_NODE_STATE_ACTIVE,
			_BUTTON_TEXT_NODE_STATE_DISABLED,
		};
		
		void Node::draw(Raylib::Vector2 position) {
			if(p_type == NODE_TYPE_INVALID) {
				// don't do anything
			}
			
			else if(p_type == NODE_TYPE_TEXT) {
				float font_size = 8.f;
				if(p_floats.find("font size") != p_floats.cend())
					font_size = p_floats.at("font size");
				
				Raylib::DrawTextEx(
					p_font,
					p_text.c_str(),
					position,
					font_size,
					0.f,
					p_colors.at("text")
				);
			}
			
			else if(p_type == NODE_TYPE_BUTTON_TEXT) {
				if(p_nodes.find("text") == p_nodes.cend())
					return;
				
				Raylib::Vector2 mouse_pos = Raylib::GetMousePosition();
				
				if(
					Raylib::CheckCollisionPointRec(
						mouse_pos,
						Raylib::Rectangle {
							position.x,
							position.y,
							p_floats.at("text size x") + (p_floats.at("border thickness") + p_floats.at("button margin")) * 2.f,
							p_floats.at("text size y") + (p_floats.at("border thickness") + p_floats.at("button margin")) * 2.f,
						}
					)
				) {
					switch(p_state) {
						case _BUTTON_TEXT_NODE_STATE_DEFAULT: {
							p_state = _BUTTON_TEXT_NODE_STATE_HOVER;
							break;
						};
						
						case _BUTTON_TEXT_NODE_STATE_HOVER: {
							if(Raylib::IsMouseButtonPressed(Raylib::MOUSE_BUTTON_LEFT))
								p_state = _BUTTON_TEXT_NODE_STATE_ACTIVE;
							
							break;
						};
						
						case _BUTTON_TEXT_NODE_STATE_ACTIVE: {
							if(Raylib::IsMouseButtonUp(Raylib::MOUSE_BUTTON_LEFT)) {
								// [TODO] handle button press
								p_state = _BUTTON_TEXT_NODE_STATE_DEFAULT;
							}
							
							break;
						};
						
						case _BUTTON_TEXT_NODE_STATE_DISABLED:
						default:
							break;
					};
				} else {
					p_state = _BUTTON_TEXT_NODE_STATE_DEFAULT;
				}
				
				std::string color_id_suffix = "default";
				
				if(p_state == _BUTTON_TEXT_NODE_STATE_HOVER) color_id_suffix = "hover";
				if(p_state == _BUTTON_TEXT_NODE_STATE_ACTIVE) color_id_suffix = "active";
				if(p_state == _BUTTON_TEXT_NODE_STATE_DISABLED) color_id_suffix = "disabled";
				
				std::string id_bg_color = "background color " + color_id_suffix;
				std::string id_it_color = "text color " + color_id_suffix;
				
				Raylib::DrawRectangleRounded(
					Raylib::Rectangle {
						position.x,
						position.y,
						p_floats.at("text size x") + (p_floats.at("border thickness") + p_floats.at("button margin")) * 2.f,
						p_floats.at("text size y") + (p_floats.at("border thickness") + p_floats.at("button margin")) * 2.f,
					},
					p_floats.at("border roundness"),
					4,
					p_colors.at(id_bg_color)
				);
				
				Raylib::DrawRectangleRoundedLinesEx(
					Raylib::Rectangle {
						position.x,
						position.y,
						p_floats.at("text size x") + (p_floats.at("border thickness") + p_floats.at("button margin")) * 2.f,
						p_floats.at("text size y") + (p_floats.at("border thickness") + p_floats.at("button margin")) * 2.f,
					},
					p_floats.at("border roundness"),
					4,
					p_floats.at("border thickness"),
					p_colors.at("border color")
				);
				
				Node &text_node = p_nodes.at("text");
				text_node.set_color("text", p_colors.at(id_it_color));
				text_node.draw(Raylib::Vector2 {
					position.x + p_floats.at("border thickness") + p_floats.at("button margin"),
					position.y + p_floats.at("border thickness") + p_floats.at("button margin"),
				});
			}
			
			else {
				// [TODO] handle case
			}
		};
		
		void Node::set_type_text(text_t options) {
			p_state = 0;
			p_type = NODE_TYPE_TEXT;
			
			p_text = options.text;
			
			if(Raylib::IsFontValid(options.font)) {
				p_font = options.font;
			} else {
				std::printf("[TimeCapsule/GUI] WARNING: Node::set_type_text(): Invalid 'font'.\n");
				p_font = get_default_font();
			}
			
			Raylib::Color text_color;
			
			if(
				options.color.r == Raylib::BLANK.r
				&& options.color.g == Raylib::BLANK.g
				&& options.color.b == Raylib::BLANK.b
				&& options.color.a == Raylib::BLANK.a
			) {
				text_color = get_default_color("text");
			} else {
				text_color = options.color;
			}
			
			p_colors.insert_or_assign("text", text_color);
		};
		
		void Node::set_type_button_with_text(button_with_text_t options) {
			p_state = 0;
			p_type = NODE_TYPE_BUTTON_TEXT;
			
			p_text = options.text;
			
			if(Raylib::IsFontValid(options.text_font)) {
				p_font = options.text_font;
			} else {
				std::printf("[TimeCapsule/GUI] WARNING: Node::set_type_text(): Invalid 'font'.\n");
				p_font = get_default_font();
			}
			
			Raylib::Color color;
			
			#define HANDLE_COLOR(option,internal_id,global_id)	\
				if(												\
					option.r == Raylib::BLANK.r					\
					&& option.g == Raylib::BLANK.g				\
					&& option.b == Raylib::BLANK.b				\
					&& option.a == Raylib::BLANK.a				\
				) {												\
					color = get_default_color(global_id);		\
				} else {										\
					color = option;								\
				}												\
																\
				p_colors.insert_or_assign(internal_id, color);
			
			HANDLE_COLOR(options.text_color_default, "text color default", "button text color default");
			HANDLE_COLOR(options.text_color_hover, "text color hover", "button text color hover");
			HANDLE_COLOR(options.text_color_active, "text color active", "button text color active");
			HANDLE_COLOR(options.text_color_disabled, "text color disabled", "button text color disabled");
			HANDLE_COLOR(options.background_color_default, "background color default", "button background color default");
			HANDLE_COLOR(options.background_color_hover, "background color hover", "button background color hover");
			HANDLE_COLOR(options.background_color_active, "background color active", "button background color active");
			HANDLE_COLOR(options.background_color_disabled, "background color disabled", "button background color disabled");
			HANDLE_COLOR(options.border_color, "border color", "button border color");
			
			// [TODO] handle & correct roundness(max1.0), thickness
			
			p_floats.insert_or_assign("border roundness", options.border_roundness);
			p_floats.insert_or_assign("border thickness", options.border_thickness);
			p_floats.insert_or_assign("button margin", options.button_margin);
		};
		*/
		
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
		};
		
		void Node::draw(Raylib::Vector2 position) {
			// [TODO]
		};
		
		int Node::set_type(std::string type) {
			clear();
			// [TODO]
		};
	};
};
