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

namespace timecapsule {
	namespace highlevel_debug {
		const unsigned int entry_colorpal[64] { 0x000000,0x00FF00,0x0000FF,0xFF0000,0x01FFFE,0xFFA6FE,0xFFDB66,0x006401,0x010067,0x95003A,0x007DB5,0xFF00F6,0xFFEEE8,0x774D00,0x90FB92,0x0076FF,0xD5FF00,0xFF937E,0x6A826C,0xFF029D,0xFE8900,0x7A4782,0x7E2DD2,0x85A900,0xFF0056,0xA42400,0x00AE7E,0x683D3B,0xBDC6FF,0x263400,0xBDD393,0x00B917,0x9E008E,0x001544,0xC28C9F,0xFF74A3,0x01D0FF,0x004754,0xE56FFE,0x788231,0x0E4CA1,0x91D0CB,0xBE9970,0x968AE8,0xBB8800,0x43002C,0xDEFF74,0x00FFC6,0xFFE502,0x620E00,0x008F9C,0x98FF52,0x7544B1,0xB500FF,0x00FF78,0xFF6E41,0x005F39,0x6B6882,0x5FAD4E,0xA75740,0xA5FFD2,0xFFB167,0x009BFF,0xE85EBE };

		void draw_debug_page1(perf_data_t perf_data, Raylib::Vector2 &pos) {
				double total_perf_time = 0.0;
				for(int i = 0; i < perf_data.entries_len; i++)
					total_perf_time += perf_data.entries[i].time;
				
				Raylib::Font font = Raylib::GetFontDefault();
				
				float off_x = pos.x;
				float height = 20.f;
				float max_font_size = 24.f;
				float text_spacing = 1.f;
				
				pos.y += max_font_size;

				// draw performance bar
				for(int i = 0; i < perf_data.entries_len; i++) {
					_debug_perf_entry_t entry = perf_data.entries[i];
					
					Raylib::Color color = Raylib::GetColor((entry_colorpal[i] << 8) | 0xff);
					
					float width;
					float frame_budget = 1.0 / 60.0;
					
					if(total_perf_time > frame_budget) {
						if(total_perf_time <= (2.f * frame_budget)) {
							width = (float)Raylib::GetScreenWidth() * 0.25f;
							width *= entry.time / frame_budget;
						} else {
							width = (float)Raylib::GetScreenWidth() * 0.90f;
							width *= (entry.time / total_perf_time);
						}
					} else {
						width = (float)Raylib::GetScreenWidth() * 0.25f;
						width *= entry.time / frame_budget;
					}
					
					float font_size;
					for(font_size = max_font_size; font_size >= 8.f && font_size <= max_font_size; font_size -= 1.f) {
						Raylib::Vector2 msr = Raylib::MeasureTextEx(
							font,
							entry.title.c_str(),
							font_size,
							text_spacing
						);
						
						if(msr.x <= width)
							break;
					};
					
					if(font_size >= 8.f) {
						Raylib::Vector2 msr = Raylib::MeasureTextEx(
							font,
							entry.title.c_str(),
							font_size,
							text_spacing
						);
						
						Raylib::DrawTextEx(
							font,
							entry.title.c_str(),
							Raylib::Vector2 {
								off_x,
								pos.y - msr.y,
							},
							font_size,
							text_spacing,
							Raylib::WHITE
						);
					}
					
					Raylib::DrawRectangleRec(
						Raylib::Rectangle {
							off_x,
							pos.y,
							width,
							height,
						},
						color
					);
					
					off_x += width;
				};
				
				pos.y += height + 1.0;

				// draw triangle count
				{
					const char *triangle_count_text = Raylib::TextFormat(
						"Triangles: %i(P) / %i(D)",
						perf_data.player_render_triangle_count,
						perf_data.debug_render_triangle_count
					);

					Raylib::Font font = Raylib::GetFontDefault();
					float font_size = 24.f;
					float text_spacing = 1.f;

					Raylib::Vector2 msr = Raylib::MeasureTextEx(
						font,
						triangle_count_text,
						font_size,
						text_spacing
					);

					Raylib::DrawTextEx(
						font,
						triangle_count_text,
						pos,
						font_size,
						text_spacing,
						Raylib::WHITE
					);

					pos.y += msr.y + 1.f;
				}
		};
	};
};
