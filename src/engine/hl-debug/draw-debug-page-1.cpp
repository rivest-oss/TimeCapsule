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
		const unsigned int p1_entry_colorpal[64] { 0xCCCCCC,0x00FF00,0x0000FF,0xFF0000,0x01FFFE,0xFFA6FE,0xFFDB66,0x006401,0x010067,0x95003A,0x007DB5,0xFF00F6,0xFFEEE8,0x774D00,0x90FB92,0x0076FF,0xD5FF00,0xFF937E,0x6A826C,0xFF029D,0xFE8900,0x7A4782,0x7E2DD2,0x85A900,0xFF0056,0xA42400,0x00AE7E,0x683D3B,0xBDC6FF,0x263400,0xBDD393,0x00B917,0x9E008E,0x001544,0xC28C9F,0xFF74A3,0x01D0FF,0x004754,0xE56FFE,0x788231,0x0E4CA1,0x91D0CB,0xBE9970,0x968AE8,0xBB8800,0x43002C,0xDEFF74,0x00FFC6,0xFFE502,0x620E00,0x008F9C,0x98FF52,0x7544B1,0xB500FF,0x00FF78,0xFF6E41,0x005F39,0x6B6882,0x5FAD4E,0xA75740,0xA5FFD2,0xFFB167,0x009BFF,0xE85EBE };
		float p1_frame_time_history[320];
		float p1_comparator = 0.f;

		void draw_debug_page1(perf_data_t perf_data, Raylib::Vector2 &pos) {
			const float frame_time_budget = 1.f / 60.f;

			const float threshold_yellow = frame_time_budget * 0.90f;
			const float threshold_red = frame_time_budget;

			float frame_total_time = 0.f;

			for(std::pair<std::string, highlevel_debug::_debug_perf_entry_t> e : perf_data.entries)
				frame_total_time += e.second.time;

			float highest_cost = frame_total_time;
			for(int i = 1; i < (320 - 1); i++)
				if(p1_frame_time_history[i] > highest_cost)
					highest_cost = p1_frame_time_history[i];
			
			if(highest_cost < p1_comparator)
				p1_comparator -= p1_comparator * 0.10 * get_unscaled_delta_time();
			if(highest_cost > p1_comparator)
				p1_comparator = highest_cost;
			
			if(p1_comparator < frame_time_budget)
				p1_comparator = frame_time_budget;
			
			std::printf("p1_comparator: %f\n", p1_comparator * 1000.f);

			// draw cost of frames
			if(Raylib::GetScreenWidth() >= 400) {
				float off_x = 16.f;

				const float max_height = 32.f;
				pos.y += max_height;

				Raylib::Color color = Raylib::GREEN;

				for(int i = 1; i < 320; i++)
					p1_frame_time_history[i - 1] = p1_frame_time_history[i];

				p1_frame_time_history[320 - 1] = frame_total_time;

				for(int i = 0; i < 320; i++, off_x += 1.f) {
					float dt = p1_frame_time_history[i];

					float height = (dt / frame_time_budget) * 0.5f * max_height;
					if(height > max_height)
						height = max_height;

					if(dt >= threshold_red)
						color = Raylib::RED;
					else if(dt >= threshold_yellow)
						color = Raylib::YELLOW;
					else
						color = Raylib::GREEN;
					
					Raylib::DrawRectangleRec(
						Raylib::Rectangle {
							off_x,
							pos.y - height,
							1.f,
							height,
						},
						color
					);
				};

				pos.y += 1.f;
			}

			// draw graph of cost in frame
			{
				const float max_height = 32.f;
				const float max_width = Raylib::GetScreenWidth() * 0.60f;

				Raylib::Font font = Raylib::GetFontDefault();
				float text_spacing = 1.f;
				const float min_font_size = 4.f;
				const float max_font_size = 16.f;

				float x = pos.x;
				pos.y += max_font_size + 1.f;

				for(std::pair<std::string, highlevel_debug::_debug_perf_entry_t> e : perf_data.entries) {
					_debug_perf_entry_t entry = e.second;
					Raylib::Color entry_color = Raylib::GetColor((p1_entry_colorpal[entry.color_index] << 8u) | 0xffu);
					const char *entry_title = e.first.c_str();

					float width = max_width * (entry.time / frame_total_time);

					Raylib::DrawRectangleRec(
						Raylib::Rectangle {
							x,
							pos.y,
							width,
							max_height,
						},
						entry_color
					);

					float font_size;

					for(font_size = max_font_size; font_size >= min_font_size && font_size <= max_font_size; font_size -= 1.f) {
						Raylib::Vector2 msr = Raylib::MeasureTextEx(
							font,
							entry_title,
							font_size,
							text_spacing
						);
						
						if(msr.x <= width)
							break;
					};

					if(font_size >= min_font_size) {
						Raylib::Vector2 msr = Raylib::MeasureTextEx(
							font,
							entry_title,
							font_size,
							text_spacing
						);

						Raylib::DrawTextEx(
							font,
							entry_title,
							Raylib::Vector2 {
								x,
								pos.y - msr.y,
							},
							font_size,
							text_spacing,
							Raylib::WHITE
						);
					}

					//const char *entry_weight_text = Raylib::TextFormat("%i%%", (int)((entry.time / frame_total_time) * 100.f));
					const char *entry_weight_text = Raylib::TextFormat("%.02fms", entry.time * 1000.f);

					for(font_size = max_font_size; font_size >= min_font_size && font_size <= max_font_size; font_size -= 1.f) {
						Raylib::Vector2 msr = Raylib::MeasureTextEx(
							font,
							entry_weight_text,
							font_size,
							text_spacing
						);
						
						if(msr.x <= (width - 2.f))
							break;
					};

					if(font_size >= min_font_size) {
						Raylib::Vector2 msr = Raylib::MeasureTextEx(
							font,
							entry_weight_text,
							font_size,
							text_spacing
						);

						Raylib::DrawTextEx(
							font,
							entry_weight_text,
							Raylib::Vector2 {
								x + 1.f,
								pos.y + (max_height - msr.y) * 0.5f,
							},
							font_size,
							text_spacing,
							Raylib::Color {
								(unsigned char)(entry_color.r ^ 0xff),
								(unsigned char)(entry_color.g ^ 0xff),
								(unsigned char)(entry_color.b ^ 0xff),
								entry_color.a
							}
						);
					}

					x += width;
				};

				pos.y += max_height + 16.f;
			}

			// draw triangle count
			{
				const char *triangle_count_text = Raylib::TextFormat(
					"Triangles: %i(P) / %i(D)",
					perf_data.player_render_triangle_count,
					perf_data.debug_render_triangle_count
				);

				Raylib::Font font = Raylib::GetFontDefault();
				float font_size = 16.f;
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
