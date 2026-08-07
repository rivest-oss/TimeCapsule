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
#include <string>
#include <vector>
#include <unordered_map>
#include "misc/asset.hpp"
#include "misc/gui.hpp"
#include "math.hpp"

namespace Raylib {
	#include <raylib.h>
};

void update_and_draw(void);
void deinit(void);

Raylib::Camera3D player_camera;

Raylib::Font debug_font;

const char *FONT_FILENAMES[] = {
	"/usr/share/fonts/truetype/ubuntu/Ubuntu-B.ttf",
	nullptr,
};

const int debug_models_length = 11;

const char *DEBUG_MODELS_FILENAMES[debug_models_length + 1] = {
	"./resources/heightmap-lods-experiment/downscale-image-x1.png",
	"./resources/heightmap-lods-experiment/downscale-image-x2.png",
	"./resources/heightmap-lods-experiment/downscale-image-x4.png",
	"./resources/heightmap-lods-experiment/downscale-image-x8.png",
	"./resources/heightmap-lods-experiment/downscale-image-x16.png",
	"./resources/heightmap-lods-experiment/downscale-image-x32.png",
	"./resources/heightmap-lods-experiment/downscale-image-x64.png",
	"./resources/heightmap-lods-experiment/downscale-image-x128.png",
	"./resources/heightmap-lods-experiment/downscale-image-x256.png",
	"./resources/heightmap-lods-experiment/downscale-image-x512.png",
	"./resources/heightmap-lods-experiment/downscale-image-x1024.png",
	nullptr,
};

Raylib::Model debug_models[debug_models_length];

const Raylib::Vector3 debug_model_ref_size {
	//1024.f,
	//100.f,
	//1024.f,
	100.f,
	10.f,
	100.f,
};

unsigned int debug_model_iteration = 0;

enum : int {
	GAME_STATE_INVALID = 0,
	GAME_STATE_EXIT,
	GAME_STATE_RUNNING,
	GAME_STATE_PAUSED,
};

int game_state = GAME_STATE_INVALID;

void transition_game_state_to(int new_state) {
	if(new_state == GAME_STATE_EXIT) {
		game_state = GAME_STATE_EXIT;
		deinit();
		std::exit(0);
	}
	
	if(game_state == new_state)
		return;
	
	if(new_state == GAME_STATE_RUNNING) {
		Raylib::DisableCursor();
	}
	
	if(new_state == GAME_STATE_PAUSED) {
		Raylib::EnableCursor();
	}
	
	game_state = new_state;
};

[[noreturn]] void init(int argc, const char **argv) {
	Ox::Error err;
	
	timecapsule::config_t conf = timecapsule::read_config_from_args(argc, argv);
	(void)conf;
	
	Raylib::SetTargetFPS(60);

	Raylib::InitWindow(640, 360, "TimeCapsule");
	while(Raylib::IsWindowReady() == false) {};
	
	Raylib::SetExitKey(Raylib::KEY_NULL);
	
	Raylib::InitAudioDevice();
	while(Raylib::IsAudioDeviceReady() == false) {};
	
	if(timecapsule::init_resources() < 0) {
		deinit();
		std::exit(1);
	}
	
	player_camera.position = Raylib::Vector3 { 0.f, 2.f, 4.f };
	player_camera.target = Raylib::Vector3 { 0.f, 0.f, 0.f };
	player_camera.up = Raylib::Vector3 { 0.f, 1.f, 0.f };
	player_camera.fovy = 45.f;
	player_camera.projection = Raylib::CAMERA_PERSPECTIVE;
	
	for(int i = 0; FONT_FILENAMES[i] != nullptr; i++) {
		debug_font = timecapsule::load_font(FONT_FILENAMES[i]);
		if(Raylib::IsFontValid(debug_font))
			break;
	};
	
	if(Raylib::IsFontValid(debug_font) == false)
		debug_font = Raylib::GetFontDefault();

	for(int i = 0; DEBUG_MODELS_FILENAMES[i] != nullptr; i++) {
		Raylib::Image img = timecapsule::load_image(DEBUG_MODELS_FILENAMES[i]);
		if(Raylib::IsImageValid(img) == false) {
			std::fprintf(stderr, "Couldn't load heightmap image: %s\n", DEBUG_MODELS_FILENAMES[i]);
			deinit();
			std::exit(1);
		}

		Raylib::Texture2D diffuse_tex = timecapsule::load_texture_from_image(img);
		if(Raylib::IsTextureValid(diffuse_tex) == false) {
			std::fprintf(stderr, "Couldn't load heightmap diffuse texture: %s\n", DEBUG_MODELS_FILENAMES[i]);
			deinit();
			std::exit(1);
		}
		
		Raylib::Mesh mesh = Raylib::GenMeshHeightmap(img, debug_model_ref_size);
		debug_models[i] = timecapsule::load_model_from_mesh(mesh);

		timecapsule::unload_image(img);

		if(Raylib::IsModelValid(debug_models[i]) == false) {
			std::fprintf(stderr, "Couldn't load heightmap model: %s\n", DEBUG_MODELS_FILENAMES[i]);
			deinit();
			std::exit(1);
		}

		debug_models[i] = timecapsule::update_model_diffuse_texture(debug_models[i], diffuse_tex);
		if(Raylib::IsModelValid(debug_models[i]) == false) {
			std::fprintf(stderr, "Couldn't load heightmap diffuse textue to model: %s\n", DEBUG_MODELS_FILENAMES[i]);
			deinit();
			std::exit(1);
		}
	};
	
	transition_game_state_to(GAME_STATE_PAUSED);
	
	while(true) {
		if(Raylib::WindowShouldClose())
			transition_game_state_to(GAME_STATE_EXIT);
		
		if(Raylib::IsKeyPressed(Raylib::KEY_ESCAPE)) {
			if(game_state == GAME_STATE_RUNNING)
				transition_game_state_to(GAME_STATE_PAUSED);
			
			else if(game_state == GAME_STATE_PAUSED)
				transition_game_state_to(GAME_STATE_RUNNING);
		}
		
		update_and_draw();
	};
	
	transition_game_state_to(GAME_STATE_EXIT);
};

void deinit(void) {
	Raylib::EnableCursor();
	
	timecapsule::deinit_resources();
	
	Raylib::CloseAudioDevice();
	Raylib::CloseWindow();
};

void update(double dt) {
	(void)dt;

	const float speed_walking = 1.4f;
	const float speed_running = 4.4f;
	
	if(game_state != GAME_STATE_RUNNING)
		return;

	Raylib::UpdateCameraPro(
		&player_camera,
		Raylib::Vector3 {
			Raylib::IsKeyDown(Raylib::KEY_W) * speed_running * (float)dt
			- Raylib::IsKeyDown(Raylib::KEY_S) * speed_running * (float)dt,
			Raylib::IsKeyDown(Raylib::KEY_D) * speed_running * (float)dt
			- Raylib::IsKeyDown(Raylib::KEY_A) * speed_running * (float)dt,
			Raylib::GetMouseWheelMove() * 4.f * (float)dt,
		},
		Raylib::Vector3 {
			Raylib::GetMouseDelta().x * M_PI2f * (float)dt,
			Raylib::GetMouseDelta().y * M_PI2f * (float)dt,
			0.f,
		},
		0.f
	);

	if(Raylib::IsKeyPressed(Raylib::KEY_M))
		debug_model_iteration++;

	if(debug_model_iteration >= debug_models_length)
		debug_model_iteration = 0;
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

void debug_draw_rotation(Raylib::Font font, float angle, Raylib::Vector2 screen_pos, float screen_radius) {
	Raylib::DrawCircleLines(screen_pos.x + screen_radius, screen_pos.y + screen_radius, screen_radius, Raylib::WHITE);

	Raylib::DrawLine(
		screen_pos.x + screen_radius,
		screen_pos.y + screen_radius,
		screen_pos.x + screen_radius + std::cos(angle) * screen_radius,
		screen_pos.y + screen_radius + -1.f * std::sin(angle) * screen_radius,
		Raylib::WHITE
	);

	Raylib::DrawCircle(screen_pos.x + screen_radius, screen_pos.y + screen_radius, std::min(4.f, std::max(1.f, screen_radius / 10.f)), Raylib::ColorAlpha(Raylib::WHITE, 0.5f));
	
	int deg = int(angle * (180.f / M_PIf));
	if(deg < 0) deg = 360 - (deg * -1);
	
	Raylib::DrawTextEx(
		Raylib::IsFontValid(font) ? font : Raylib::GetFontDefault(),
		Raylib::TextFormat(
			"%i°",
			deg
		),
		Raylib::Vector2 {
			(float)screen_pos.x + screen_radius * 1.9f,
			(float)screen_pos.y,
		},
		std::min(24.f, std::max(10.f, screen_radius / 2.3f)),
		0.f,
		Raylib::WHITE
	);
};

void debug_draw_rotation_3d(Raylib::Vector3 angle, Raylib::Vector2 screen_pos, float screen_radius, Raylib::Font font = { 0 }) {
	debug_draw_rotation(
		font,
		angle.x,
		Raylib::Vector2 {
			screen_pos.x,
			screen_pos.y,
		},
		screen_radius
	);

	debug_draw_rotation(
		font,
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
		300 - 10,
		200 - 10,
		Raylib::ColorAlpha(Raylib::DARKGRAY, 0.5f)
	);

	int font_height = 16;
	int x = base_x + 10, y = base_y + 10;

	debug_draw_text(
		Raylib::TextFormat(
			"Time: %f",
			Raylib::GetTime()
		),
		x,
		y,
		font_height,
		Raylib::WHITE
	);

	y += font_height + 4;

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
		font_height * 2.f,
		debug_font
	);
};

void draw(double dt) {
	Raylib::BeginDrawing();
	Raylib::ClearBackground(Raylib::BLACK);
	
	(void)dt;
	
	int screen_width = Raylib::GetScreenWidth();
	int screen_height = Raylib::GetScreenHeight();
	
	if(true) {
		debug_draw_camera_info(screen_width - 300, 10);
		
		if(game_state == GAME_STATE_PAUSED) {
			for(int y = -1; y < 2; y++)
				for(int x = -1; x < 2; x++)
					if(x != 0 && y != 0)
					Raylib::DrawText("Game paused", x + 16, y + 16, 32, Raylib::BLACK);
			
			Raylib::DrawText("Game paused", 16, 16, 32, Raylib::WHITE);
		}
	}

	Raylib::BeginMode3D(player_camera);

	Raylib::DrawSphereWires(
		Raylib::Vector3 {
			0.f,
			0.f,
			0.f,
		},
		1.f,
		5,
		10,
		Raylib::GREEN
	);

	Raylib::DrawModel(
		debug_models[debug_model_iteration],
		Raylib::Vector3 {
			-(debug_model_ref_size.x / 2.f),
			-(debug_model_ref_size.y / 2.f),
			-(debug_model_ref_size.z / 2.f),
		},
		1.f,
		Raylib::WHITE
	);

	Raylib::EndMode3D();
	
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
