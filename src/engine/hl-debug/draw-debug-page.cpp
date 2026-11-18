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
		void draw_debug_page1(perf_data_t perf_data, Raylib::Vector2 &pos);

		void draw_debug_page(unsigned int page, perf_data_t perf_data) {
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

			if(page == 1u)
				draw_debug_page1(perf_data, pos);
		};
	};
};
