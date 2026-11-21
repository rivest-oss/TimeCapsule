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

#include "../hl-debug.hpp"
#include "../gl.hpp"
#include "../math.hpp"
#include "../core.hpp"

namespace timecapsule {
	namespace highlevel_debug {
		unsigned int _debug_page_no = 0u;
		
		unsigned int _debug_page2_it_type = 0u;
		unsigned int _debug_page2_it_value = 0u;
		Raylib::Camera3D _debug_page2_3d_camera {
			Raylib::Vector3 { 0.f, 0.0f, 1.f },
			Raylib::Vector3 { 0.f, 0.f, 0.f },
			Raylib::Vector3 { 0.f, 1.f, 0.f },
			45.f,
			Raylib::CAMERA_PERSPECTIVE
		};
		
		void draw_debug_page1(perf_data_t perf_data, Raylib::Vector2 &pos);

		void draw_debug_page(perf_data_t perf_data) {
			if(Raylib::IsKeyPressed(Raylib::KEY_F3))
				_debug_page_no++;
			if(_debug_page_no > 2u)
				_debug_page_no = 0u;
			
			Raylib::Vector2 pos {
				16.f,
				16.f,
			};

			{
				int fps_no = Raylib::GetFPS();
				const char *fps_text = Raylib::TextFormat("%i FPS", fps_no);

				Raylib::Font font = Raylib::GetFontDefault();
				float font_size = 24.f;
				float text_spacing = 1.f;

				Raylib::Vector2 msr = Raylib::MeasureTextEx(
					font,
					fps_text,
					font_size,
					text_spacing
				);

				Raylib::DrawTextEx(
					font,
					fps_text,
					Raylib::Vector2 {
						std::round(pos.x),
						std::round(pos.y),
					},
					font_size,
					text_spacing,
					Raylib::WHITE
				);

				pos.y += msr.y + 1.f;
			}
			
			if(_debug_page_no == 2u) {
				const char *text = "";
				
				if(Raylib::IsKeyPressed(Raylib::KEY_UP))
					_debug_page2_it_type++;
				if(Raylib::IsKeyPressed(Raylib::KEY_DOWN))
					_debug_page2_it_type--;
				if(Raylib::IsKeyPressed(Raylib::KEY_LEFT))
					_debug_page2_it_value--;
				if(Raylib::IsKeyPressed(Raylib::KEY_RIGHT))
					_debug_page2_it_value++;
				
				if(_debug_page2_it_type > 3u)
					_debug_page2_it_type = 0u;
				
				if(_debug_page2_it_type == 0u) {
					Raylib::Texture2D tex;
					unsigned int len;
					std::string id;
					debug_copy_texture(tex, _debug_page2_it_value, len, id);
					
					if(Raylib::IsTextureValid(tex) == false)
						_debug_page2_it_value = 0u;
					
					debug_copy_texture(tex, _debug_page2_it_value, len, id);
					
					text = Raylib::TextFormat("texture (%i/%i) (%ix%i): %s", _debug_page2_it_value, len, tex.width, tex.height, id.c_str());
					
					if(Raylib::IsTextureValid(tex)) {
						Raylib::DrawTexturePro(
							tex,
							Raylib::Rectangle {
								0.f,
								0.f,
								(float)tex.width,
								-(float)tex.height,
							},
							Raylib::Rectangle {
								Raylib::GetScreenWidth() - (float)tex.width,
								Raylib::GetScreenHeight() - (float)tex.height,
								(float)tex.width,
								(float)tex.height,
							},
							Raylib::Vector2 {
								0.f,
								0.f,
							},
							0.f,
							Raylib::WHITE
						);
					}
				} else if(_debug_page2_it_type == 1u) {
					Raylib::RenderTexture2D tex;
					unsigned int len;
					std::string id;
					debug_copy_render_texture(tex, _debug_page2_it_value, len, id);
					
					if(Raylib::IsRenderTextureValid(tex) == false)
						_debug_page2_it_value = 0u;
					
					debug_copy_render_texture(tex, _debug_page2_it_value, len, id);
					
					text = Raylib::TextFormat("render tex. (%i/%i) (%ix%i): %s", _debug_page2_it_value, len, tex.texture.width, tex.texture.height, id.c_str());
					
					if(Raylib::IsRenderTextureValid(tex)) {
						Raylib::DrawTexturePro(
							tex.texture,
							Raylib::Rectangle {
								0.f,
								0.f,
								(float)tex.texture.width,
								-(float)tex.texture.height,
							},
							Raylib::Rectangle {
								Raylib::GetScreenWidth() - (float)tex.texture.width,
								Raylib::GetScreenHeight() - (float)tex.texture.height,
								(float)tex.texture.width,
								(float)tex.texture.height,
							},
							Raylib::Vector2 {
								0.f,
								0.f,
							},
							0.f,
							Raylib::WHITE
						);
					}
				} else if(_debug_page2_it_type == 2u) {
					Raylib::RenderTexture2D tex;
					unsigned int len;
					std::string id;
					debug_copy_render_texture_with_depth(tex, _debug_page2_it_value, len, id);
					
					if(Raylib::IsRenderTextureValid(tex) == false)
						_debug_page2_it_value = 0u;
					
					debug_copy_render_texture_with_depth(tex, _debug_page2_it_value, len, id);
					
					text = Raylib::TextFormat("render tex. w/ depth (%i/%i) (%ix%i): %s", _debug_page2_it_value, len, tex.texture.width, tex.texture.height, id.c_str());
					
					if(Raylib::IsRenderTextureValid(tex)) {
						Raylib::DrawTexturePro(
							tex.texture,
							Raylib::Rectangle {
								0.f,
								0.f,
								(float)tex.texture.width,
								-(float)tex.texture.height,
							},
							Raylib::Rectangle {
								Raylib::GetScreenWidth() - (float)tex.texture.width,
								Raylib::GetScreenHeight() - (float)tex.texture.height,
								(float)tex.texture.width,
								(float)tex.texture.height,
							},
							Raylib::Vector2 {
								0.f,
								0.f,
							},
							0.f,
							Raylib::WHITE
						);
					}
				} else if(_debug_page2_it_type == 3u) {
					Raylib::Model model;
					unsigned int len;
					std::string id;
					debug_copy_model(model, _debug_page2_it_value, len, id);
					
					if(Raylib::IsModelValid(model) == false)
						_debug_page2_it_value = 0u;
					
					debug_copy_model(model, _debug_page2_it_value, len, id);
					
					text = Raylib::TextFormat("model (%i/%i): %s", _debug_page2_it_value, len, id.c_str());
					
					if(Raylib::IsModelValid(model)) {
						Raylib::UpdateCamera(&_debug_page2_3d_camera, Raylib::CAMERA_ORBITAL);
						
						if(Raylib::IsKeyDown(Raylib::KEY_Q))
							_debug_page2_3d_camera.position.y -= get_unscaled_delta_time(),
							_debug_page2_3d_camera.target.y -= get_unscaled_delta_time();
						if(Raylib::IsKeyDown(Raylib::KEY_E))
							_debug_page2_3d_camera.position.y += get_unscaled_delta_time(),
							_debug_page2_3d_camera.target.y += get_unscaled_delta_time();
						
						Raylib::BeginMode3D(_debug_page2_3d_camera);
						
						Raylib::DrawModel(
							model,
							Raylib::Vector3 {
								0.f,
								0.f,
								0.f,
							},
							1.f,
							Raylib::WHITE
						);
						
						Raylib::EndMode3D();
					}
				}
				
				Raylib::DrawText(text, 0, Raylib::GetScreenHeight() - 18, 16, Raylib::BLACK);
				Raylib::DrawText(text, 2, Raylib::GetScreenHeight() - 18, 16, Raylib::BLACK);
				Raylib::DrawText(text, 0, Raylib::GetScreenHeight() - 16, 16, Raylib::BLACK);
				Raylib::DrawText(text, 2, Raylib::GetScreenHeight() - 16, 16, Raylib::BLACK);
				Raylib::DrawText(text, 1, Raylib::GetScreenHeight() - 17, 16, Raylib::WHITE);
			}
			
			if(_debug_page_no == 1u)
				draw_debug_page1(perf_data, pos);
		};
	};
};
