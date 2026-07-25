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

#include "io/log.hpp"
#include "misc/rtconfig.hpp"
#include "Ox/include/nuclei.hpp"
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <string>
#include <vector>
#include <unordered_map>

namespace Raylib {
	#include <raylib.h>
};

std::unordered_map<std::string, Raylib::Image> loaded_images;
std::unordered_map<std::string, Raylib::Texture2D> loaded_textures;
std::unordered_map<std::string, Raylib::RenderTexture2D> loaded_render_textures;

Raylib::Image load_image(const char *filename) {
	Raylib::Image img = Raylib::LoadImage(filename);
	if(Raylib::IsImageValid(img) == false)
		return Raylib::Image { nullptr, -1, -1, -1, 0 };
	
	std::string id = "";

	for(unsigned int shift = 16; shift > 0; shift -= 4)
		id += 'A' + ((img.width >> shift) & 0xf);

	id += ",";

	for(unsigned int shift = 16; shift > 0; shift -= 4)
		id += 'A' + ((img.height >> shift) & 0xf);

	id += ",";

	for(Ox::ulong shift = 56; shift > 0; shift -= 4)
		id += 'A' + (((Ox::ulong)img.data >> shift) & 0xf);

	loaded_images.insert_or_assign(id, img);
	return img;
};

Raylib::Texture2D load_texture_from_image(Raylib::Image img) {
	if(Raylib::IsImageValid(img) == false)
		return Raylib::Texture2D { 0, -1, -1, -1, 0 };

	Raylib::Texture2D tex = Raylib::LoadTextureFromImage(img);
	if(Raylib::IsTextureValid(tex) == false)
		return Raylib::Texture2D { 0, -1, -1, -1, 0 };
	
	std::string id = "";

	for(unsigned int shift = 16; shift > 0; shift -= 4)
		id += 'A' + ((tex.width >> shift) & 0xf);

	id += ",";

	for(unsigned int shift = 16; shift > 0; shift -= 4)
		id += 'A' + ((tex.height >> shift) & 0xf);

	id += ",";

	for(unsigned int shift = 56; shift > 0; shift -= 4)
		id += 'A' + ((tex.id >> shift) & 0xf);

	loaded_textures.insert_or_assign(id, tex);
	return tex;
};

Raylib::RenderTexture2D load_render_texture(int width, int height) {
	Raylib::RenderTexture2D tex = Raylib::LoadRenderTexture(width, height);
	if(Raylib::IsRenderTextureValid(tex) == false)
		return tex;
	
	std::string id = "";

	for(unsigned int shift = 16; shift > 0; shift -= 4)
		id += 'A' + ((tex.texture.width >> shift) & 0xf);

	id += ",";

	for(unsigned int shift = 16; shift > 0; shift -= 4)
		id += 'A' + ((tex.texture.height >> shift) & 0xf);

	id += ",";

	for(unsigned int shift = 56; shift > 0; shift -= 4)
		id += 'A' + ((tex.id >> shift) & 0xf);

	loaded_render_textures.insert_or_assign(id, tex);
	return tex;
};

void unload_image(Raylib::Image img) {
	if(Raylib::IsImageValid(img) == false)
		return;
	
	std::string id = "";

	for(unsigned int shift = 16; shift > 0; shift -= 4)
		id += 'A' + ((img.width >> shift) & 0xf);

	id += ",";

	for(unsigned int shift = 16; shift > 0; shift -= 4)
		id += 'A' + ((img.height >> shift) & 0xf);

	id += ",";

	for(Ox::ulong shift = 56; shift > 0; shift -= 4)
		id += 'A' + (((Ox::ulong)img.data >> shift) & 0xf);

	Raylib::UnloadImage(img);
	loaded_images.erase(id);
};

void unload_texture(Raylib::Texture2D tex) {
	if(Raylib::IsTextureValid(tex) == false)
		return;
	
	std::string id = "";

	for(unsigned int shift = 16; shift > 0; shift -= 4)
		id += 'A' + ((tex.width >> shift) & 0xf);

	id += ",";

	for(unsigned int shift = 16; shift > 0; shift -= 4)
		id += 'A' + ((tex.height >> shift) & 0xf);

	id += ",";

	for(unsigned int shift = 56; shift > 0; shift -= 4)
		id += 'A' + ((tex.id >> shift) & 0xf);

	Raylib::UnloadTexture(tex);
	loaded_textures.erase(id);
};

void unload_render_texture(Raylib::RenderTexture2D tex) {
	if(Raylib::IsRenderTextureValid(tex) == false)
		return;
	
	std::string id = "";

	for(unsigned int shift = 16; shift > 0; shift -= 4)
		id += 'A' + ((tex.texture.width >> shift) & 0xf);

	id += ",";

	for(unsigned int shift = 16; shift > 0; shift -= 4)
		id += 'A' + ((tex.texture.height >> shift) & 0xf);

	id += ",";

	for(unsigned int shift = 56; shift > 0; shift -= 4)
		id += 'A' + ((tex.id >> shift) & 0xf);

	Raylib::UnloadRenderTexture(tex);
	loaded_render_textures.erase(id);
};

void update_and_draw(void);
void deinit(void);

Raylib::Camera3D player_camera;
Raylib::Font debug_font;

[[noreturn]] void init(int argc, const char **argv) {
	Ox::Error err;
	
	timecapsule::config_t conf = timecapsule::read_config_from_args(argc, argv);
	(void)conf;
	
	Raylib::SetTargetFPS(60);
	Raylib::SetExitKey(Raylib::KEY_NULL);

	Raylib::InitWindow(640, 360, "TimeCapsule");
	while(Raylib::IsWindowReady() == false) {};
	
	Raylib::InitAudioDevice();
	while(Raylib::IsAudioDeviceReady() == false) {};
	
	player_camera.position = Raylib::Vector3 { 0.f, 0.f, 4.f };
	player_camera.target = Raylib::Vector3 { 0.f, 0.f, 0.f };
	player_camera.up = Raylib::Vector3 { 0.f, 1.f, 0.f };
	player_camera.fovy = 60.f;
	player_camera.projection = Raylib::CAMERA_PERSPECTIVE;

	debug_font = Raylib::LoadFont("/usr/share/fonts/truetype/ubuntu/Ubuntu-B.ttf");
	if(Raylib::IsFontValid(debug_font) == false)
		debug_font = Raylib::GetFontDefault();

	Raylib::DisableCursor();
	
	while(Raylib::WindowShouldClose() == false) {
		update_and_draw();
	};

	deinit();
	std::exit(0);
};

void deinit(void) {
	Raylib::EnableCursor();

	for(auto pair : loaded_images)
		if(Raylib::IsImageValid(pair.second))
			Raylib::UnloadImage(pair.second);
	for(auto pair : loaded_textures)
		if(Raylib::IsTextureValid(pair.second))
			Raylib::UnloadTexture(pair.second);
	for(auto pair : loaded_render_textures)
		if(Raylib::IsRenderTextureValid(pair.second))
			Raylib::UnloadRenderTexture(pair.second);
	
	Raylib::CloseAudioDevice();
	Raylib::CloseWindow();
};

void update(double dt) {
	(void)dt;
	
	Raylib::UpdateCameraPro(
		&player_camera,
		Raylib::Vector3 {
			Raylib::IsKeyDown(Raylib::KEY_W) * 4.0f * (float)dt
			- Raylib::IsKeyDown(Raylib::KEY_S) * 4.0f * (float)dt,
			Raylib::IsKeyDown(Raylib::KEY_D) * 4.0f * (float)dt
			- Raylib::IsKeyDown(Raylib::KEY_A) * 4.0f * (float)dt,
			Raylib::GetMouseWheelMove() * 4.f * (float)dt,
		},
		Raylib::Vector3 {
			Raylib::GetMouseDelta().x * 4.0f * (float)dt,
			Raylib::GetMouseDelta().y * 4.0f * (float)dt,
			0.f,
		},
		0.f
	);
};

void debug_draw_text(const char *text, int x, int y, int font_height, Raylib::Color text_color, Raylib::Color contourn_color = Raylib::BLANK) {
	for(int sy = -1; sy < 2; sy++)
		for(int sx = -1; sx < 2; sx++)
			if(sy != 0 || sx != 0)
				Raylib::DrawTextEx(
					debug_font,
					text,
					Raylib::Vector2 {
						(float)x + sx,
						(float)y + sy,
					},
					(float)font_height,
					0.1f,
					contourn_color
				);
	
	Raylib::DrawTextEx(
		debug_font,
		text,
		Raylib::Vector2 {
			(float)x,
			(float)y,
		},
		(float)font_height,
		0.1f,
		text_color
	);
};

void debug_draw_rotation(float angle, Raylib::Vector2 screen_pos, float screen_radius) {
	Raylib::DrawCircleLines(screen_pos.x + screen_radius, screen_pos.y + screen_radius, screen_radius, Raylib::WHITE);

	Raylib::DrawLine(
		screen_pos.x + screen_radius,
		screen_pos.y + screen_radius,
		screen_pos.x + screen_radius + std::cos(angle) * screen_radius,
		screen_pos.y + screen_radius + -1.f * std::sin(angle) * screen_radius,
		Raylib::WHITE
	);

	Raylib::DrawCircle(screen_pos.x + screen_radius, screen_pos.y + screen_radius, std::min(4.f, std::max(1.f, screen_radius / 10.f)), Raylib::ColorAlpha(Raylib::WHITE, 0.5f));
};

void debug_draw_rotation_3d(Raylib::Vector3 angle, Raylib::Vector2 screen_pos, float screen_radius) {
	debug_draw_rotation(
		angle.x,
		Raylib::Vector2 {
			screen_pos.x,
			screen_pos.y,
		},
		screen_radius
	);

	debug_draw_rotation(
		angle.y,
		Raylib::Vector2 {
			screen_pos.x + screen_radius * 2.5f,
			screen_pos.y,
		},
		screen_radius
	);
};

void debug_draw_camera_info(int base_x, int base_y) {
	Raylib::DrawRectangle(
		base_x,
		base_y,
		300-10,
		200-10,
		Raylib::ColorAlpha(Raylib::DARKGRAY, 0.5f)
	);

	int font_height = 16;
	int x = base_x + 10, y = base_y + 10;

	debug_draw_text("Camera:", x, y, font_height + 4, Raylib::WHITE);
	y += 8 + font_height;

	debug_draw_text(
		Raylib::TextFormat(
			"Position: %.4f, %.4f, %.4f (XYZ)",
			player_camera.position.x,
			player_camera.position.y,
			player_camera.position.z
		),
		x,
		y,
		font_height,
		Raylib::WHITE
	);

	y += font_height + 4;

	debug_draw_text(
		Raylib::TextFormat(
			"Target: %.4f, %.4f, %.4f (XYZ)",
			player_camera.target.x,
			player_camera.target.y,
			player_camera.target.z
		),
		x,
		y,
		font_height,
		Raylib::WHITE
	);

	y += font_height + 4;

	Raylib::Vector3 camera_diff {
		player_camera.target.x - player_camera.position.x,
		player_camera.target.y - player_camera.position.y,
		player_camera.target.z - player_camera.position.z,
	};

	float camera_length = std::sqrt(
		camera_diff.x * camera_diff.x
		+ camera_diff.y * camera_diff.y
		+ camera_diff.z * camera_diff.z
	);

	float camera_yaw = std::atan2(camera_diff.x, camera_diff.z);
	float camera_pitch = std::asin(camera_diff.y / camera_length);

	debug_draw_rotation_3d(
		Raylib::Vector3 {
			camera_yaw,
			camera_pitch,
			0.f,
		},
		Raylib::Vector2 {
			(float)x,
			(float)y,
		},
		font_height * 2.f
	);
};

void draw(double dt) {
	Raylib::BeginDrawing();
	Raylib::ClearBackground(Raylib::BLACK);
	
	// [TODO]
	(void)dt;

	int screen_width = Raylib::GetScreenWidth();
	int screen_height = Raylib::GetScreenHeight();

	for(int y = 0; y < screen_height; y += 16) {
		for(int x = 0; x < screen_width; x += 16) {
			Raylib::Ray ray = Raylib::GetScreenToWorldRayEx(
				Raylib::Vector2 {
					x + 8.f,
					y + 8.f,
				},
				player_camera,
				screen_width,
				screen_height
			);

			Raylib::Color color = Raylib::WHITE;
			float w = ray.direction.x;
			float h = ray.direction.x;

			if(w < 0.f && h >= 0.f) { color = Raylib::RED, w *= -1.f; }
			else if(w >= 0.f && h < 0.f) { color = Raylib::GREEN, h *= -1.f; }
			else if(w < 0.f && h < 0.f) { color = Raylib::BLUE, w *= -1.f, h *= -1.f; }

			w = std::max(1.f, w * 15.f);
			h = std::max(1.f, h * 15.f);

			Raylib::DrawRectangle(
				x,
				y,
				(int)w,
				(int)h,
				color
			);
		};
	};

	if(true) {
		debug_draw_camera_info(Raylib::GetScreenWidth() - 300, 10);
	}
	
	Raylib::EndDrawing();
	Raylib::SwapScreenBuffer();
};

void update_and_draw(void) {
	double dt = Raylib::GetFrameTime();
	update(dt);
	draw(dt);
};

int main(int argc, const char **argv) {
	init(argc, argv);
	return 0;
};

