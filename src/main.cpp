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
#include "noise.hpp"

namespace Raylib {
	#include <raylib.h>
};

#undef M_PI
#undef M_PI2
#undef M_PIf
#undef M_PI2f

#define M_PI  3.14159265358979323846264338327950288419716939937510
#define M_PI2 6.28318530717958647692528676655900576839433879875020

#define M_PIf  3.14159265358979323846264338327950288419716939937510f
#define M_PI2f 6.28318530717958647692528676655900576839433879875020f

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

Raylib::Texture2D load_texture(const char *filename) {
	Raylib::Texture2D tex = Raylib::LoadTexture(filename);
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
Raylib::Image debug_img;

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
	player_camera.fovy = 45.f;
	player_camera.projection = Raylib::CAMERA_PERSPECTIVE;

	debug_font = Raylib::LoadFont("/usr/share/fonts/truetype/ubuntu/Ubuntu-B.ttf");
	if(Raylib::IsFontValid(debug_font) == false)
		debug_font = Raylib::GetFontDefault();

	debug_img = load_image("./image.png");

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

Raylib::RayCollision cast_ray(Raylib::Ray ray, const float max_dist) {
	Raylib::RayCollision ray_collision {
		false,
		0.f,
		Raylib::Vector3 { 0.f, 0.f, 0.f, },
		Raylib::Vector3 { 0.f, 0.f, 0.f, },
	};

	Raylib::Vector3 pos {
		ray.position.x,
		ray.position.y,
		ray.position.z,
	};

	float dist = 0.f;

	while(dist < max_dist) {
		float interact_y = 0.0;

		if(pos.y <= interact_y) {
			Raylib::Vector3 diff {
				pos.x - ray.position.x,
				pos.y - ray.position.y,
				pos.z - ray.position.z,
			};

			ray_collision.hit = true;
			ray_collision.distance = dist;
			ray_collision.point = pos;
			
			break;
		}

		float safe_dist = std::abs(pos.y);
		safe_dist = std::max(safe_dist, 0.01f);

		pos.x += std::cos(ray.direction.x) * safe_dist;
		pos.y += std::sin(ray.direction.y) * safe_dist;
		pos.z += std::cos(ray.direction.z) * safe_dist;

		dist += safe_dist;
	};

	return ray_collision;
};

void draw(double dt) {
	Raylib::BeginDrawing();
	Raylib::ClearBackground(Raylib::BLACK);
	
	(void)dt;

	int screen_width = Raylib::GetScreenWidth();
	int screen_height = Raylib::GetScreenHeight();

	for(int y = 0; y < screen_height; y++) {
		for(int x = 0; x < screen_width; x++) {
			Raylib::Ray ray = Raylib::GetScreenToWorldRayEx(
				Raylib::Vector2 {
					(float)x,
					(float)y,
				},
				player_camera,
				screen_width,
				screen_height
			);

			{
				float len = 1.f / std::sqrt(
					ray.direction.x * ray.direction.x
					+ ray.direction.y * ray.direction.y
					+ ray.direction.z * ray.direction.z
				);

				ray.direction.x /= len;
				ray.direction.y /= len;
				ray.direction.z /= len;
			}

			const float max_dist = 10.f;

			Raylib::RayCollision ray_collision = cast_ray(ray, max_dist);
			Raylib::Color color;

			if(ray_collision.hit) {
				if(ray_collision.distance < 0.0001f) {
					color = Raylib::WHITE;
				} else {
					float factor = ray_collision.distance / max_dist;
					//float factor = ray_collision.point.y - 2.0;

					if(factor > 0.92f) factor = 0.92f;
					if(factor < 0.01f) factor = 0.01f;
					factor *= -1.f;

					color = Raylib::ColorBrightness(Raylib::WHITE, factor);
				}
			} else {
				color = Raylib::BLACK;
			}

			int img_x = ray_collision.point.x * 100.f;
			while(img_x < 0) img_x += debug_img.width;
			while(img_x >= debug_img.width) img_x -= debug_img.width;

			int img_y = ray_collision.point.z * 100.f;
			while(img_y < 0) img_y += debug_img.height;
			while(img_y >= debug_img.height) img_y -= debug_img.height;

			Raylib::Color pixel_color = Raylib::GetImageColor(
				debug_img,
				img_x,
				img_y
			);

			Raylib::DrawPixel(x, y, pixel_color);
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

