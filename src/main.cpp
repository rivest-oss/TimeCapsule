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
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <vector>
#include <unordered_map>
#include "misc/gui.hpp"
#include "math.hpp"
#include "engine/asset.hpp"
#include "engine/scene.hpp"

namespace Raylib {
	#include <raylib.h>
};

void update_and_draw(void);
void deinit(void);

Raylib::Camera3D player_camera;
Raylib::Camera3D debug_camera;

Raylib::Font debug_font;

const char *FONT_FILENAMES[] = {
	"/usr/share/fonts/truetype/ubuntu/Ubuntu-B.ttf",
	nullptr,
};

timecapsule::Scene world_scene;

const Raylib::Vector3 debug_model_ref_size {
	//1024.f,
	//100.f,
	//1024.f,
	//100.f,
	//10.f,
	//100.f,

	16.f,
	10.f,
	16.f,
};

int load_models(void) {
	Raylib::Model model_person = timecapsule::load_model("resources/models/not-for-distribution/person/scene.gltf");
	if(Raylib::IsModelValid(model_person) == false) return -1;

	timecapsule::model_t &model_person_at_scene = world_scene.add_model("person", model_person);
	model_person_at_scene.position.x = 0.f;

	return 0;
};

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

float clampf(float x, float min = -1.f, float max = 1.f) {
	if(x < min) return min;
	if(x > max) return max;
	return x;
};

float vdistance(float a, float b) {
	return std::sqrt(std::pow(b - a, 2.f));
};
float vdistance(Raylib::Vector2 a, Raylib::Vector2 b) {
	return std::sqrt(
		std::pow(b.x - a.x, 2.f)
		+ std::pow(b.y - a.y, 2.f)
	);
};
float vdistance(Raylib::Vector3 a, Raylib::Vector3 b) {
	return std::sqrt(
		std::pow(b.x - a.x, 2.f)
		+ std::pow(b.y - a.y, 2.f)
		+ std::pow(b.z - a.z, 2.f)
	);
};

Raylib::RenderTexture2D game_main_render_texture;
Raylib::RenderTexture2D game_debug_render_texture;

[[noreturn]] void init(int argc, const char **argv) {
	Ox::Error err;
	
	timecapsule::config_t conf = timecapsule::read_config_from_args(argc, argv);
	(void)conf;
	
	Raylib::SetTargetFPS(60);

	Raylib::SetConfigFlags(
		Raylib::FLAG_WINDOW_RESIZABLE
	);
	
	Raylib::InitWindow(640, 360, "TimeCapsule");
	while(Raylib::IsWindowReady() == false) {};
	
	Raylib::SetExitKey(Raylib::KEY_NULL);
	
	Raylib::InitAudioDevice();
	while(Raylib::IsAudioDeviceReady() == false) {};
	
	if(timecapsule::init_resources() < 0) {
		deinit();
		std::exit(1);
	}

	game_main_render_texture = timecapsule::load_render_texture(16, 16);
	if(Raylib::IsRenderTextureValid(game_main_render_texture) == false) {
		std::fprintf(stderr, "Couldn't load game main render texture.\n");
		deinit();
		std::exit(1);
	}
	
	game_debug_render_texture = timecapsule::load_render_texture(16, 16);
	if(Raylib::IsRenderTextureValid(game_debug_render_texture) == false) {
		std::fprintf(stderr, "Couldn't load game main debug render texture.\n");
		deinit();
		std::exit(1);
	}

	world_scene.deinit();
	
	player_camera.position = Raylib::Vector3 { 0.f, 2.f, 4.f };
	player_camera.target = Raylib::Vector3 { 0.f, 0.f, 0.f };
	player_camera.up = Raylib::Vector3 { 0.f, 1.f, 0.f };
	player_camera.fovy = 45.f;
	player_camera.projection = Raylib::CAMERA_PERSPECTIVE;

	debug_camera.position = Raylib::Vector3 { 0.f, 2.f, 4.f };
	debug_camera.target = Raylib::Vector3 { 0.f, 0.f, 0.f };
	debug_camera.up = Raylib::Vector3 { 0.f, 1.f, 0.f };
	debug_camera.fovy = 45.f;
	debug_camera.projection = Raylib::CAMERA_PERSPECTIVE;
	
	for(int i = 0; FONT_FILENAMES[i] != nullptr; i++) {
		debug_font = timecapsule::load_font(FONT_FILENAMES[i]);
		if(Raylib::IsFontValid(debug_font))
			break;
	};
	
	if(Raylib::IsFontValid(debug_font) == false)
		debug_font = Raylib::GetFontDefault();

	if(load_models() < 0) {
		deinit();
		std::exit(1);
	}

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
	
	world_scene.deinit();
	timecapsule::deinit_resources();
	
	Raylib::CloseAudioDevice();
	Raylib::CloseWindow();
};

bool is_camera_third_person = false;

float temp_t = 0.f;

void update(double dt) {
	(void)dt;

	const float speed_walking = 1.4f;
	const float speed_running = 4.4f;

	float forward_movement_speed = speed_running;
	float upward_movement_speed = 4.f;

	if(Raylib::IsKeyDown(Raylib::KEY_LEFT_SHIFT)) {
		forward_movement_speed *= 2.f;
		upward_movement_speed *= 4.f;
	}

	if(Raylib::IsKeyDown(Raylib::KEY_LEFT_CONTROL)) {
		forward_movement_speed *= (1.f / 2.f);
	}
	
	if(game_state != GAME_STATE_RUNNING)
		return;

	if(Raylib::IsKeyPressed(Raylib::KEY_P))
		is_camera_third_person = is_camera_third_person == false;

	if(is_camera_third_person == false) {
		Raylib::UpdateCameraPro(
			&player_camera,
			Raylib::Vector3 {
				Raylib::IsKeyDown(Raylib::KEY_W) * forward_movement_speed * (float)dt
				- Raylib::IsKeyDown(Raylib::KEY_S) * forward_movement_speed * (float)dt,
				Raylib::IsKeyDown(Raylib::KEY_D) * forward_movement_speed * (float)dt
				- Raylib::IsKeyDown(Raylib::KEY_A) * forward_movement_speed * (float)dt,
				Raylib::GetMouseWheelMove() * upward_movement_speed * (float)dt,
			},
			Raylib::Vector3 {
				Raylib::GetMouseDelta().x * M_PI2f * (float)dt,
				Raylib::GetMouseDelta().y * M_PI2f * (float)dt,
				0.f,
			},
			0.f
		);
	} else {
		Raylib::UpdateCamera(&player_camera, Raylib::CAMERA_THIRD_PERSON);
	}

	temp_t += dt;

	timecapsule::model_t &model_person_at_scene = world_scene.get_model("person");
	model_person_at_scene.position.x = std::sin(temp_t);

	if(temp_t >= 6.0)
		model_person_at_scene.rotation.z = std::sin(temp_t - 6.0);
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

void debug_draw_camera_info(int base_x, int base_y, int screen_width, int screen_height) {
	Raylib::DrawRectangle(
		base_x,
		base_y,
		300,
		214,
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

	debug_draw_text(
		Raylib::TextFormat(
			"FPS: %i",
			Raylib::GetFPS()
		),
		x,
		y,
		font_height,
		Raylib::WHITE
	);

	y += font_height + 4;

	debug_draw_text(
		Raylib::TextFormat(
			"Screen: %c %ix%i",
			Raylib::IsWindowFullscreen() ? 'F' : 'W',
			screen_width,
			screen_height
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

float min(float a, float b) {
	return a < b ? a : b;
};
float max(float a, float b) {
	return a > b ? a : b;
};

Raylib::Rectangle model_screen_rec, occlusor_screen_rec;

// [TODO] This sucks, improve it.
bool is_model_completely_occluded(Raylib::BoundingBox model, Raylib::BoundingBox occlusor) {
	Raylib::Vector2 model_screen_points[8] = {
		Raylib::GetWorldToScreen(Raylib::Vector3 { model.min.x, model.min.y, model.min.z }, player_camera),
		Raylib::GetWorldToScreen(Raylib::Vector3 { model.min.x, model.min.y, model.max.z }, player_camera),
		Raylib::GetWorldToScreen(Raylib::Vector3 { model.min.x, model.max.y, model.min.z }, player_camera),
		Raylib::GetWorldToScreen(Raylib::Vector3 { model.min.x, model.max.y, model.max.z }, player_camera),
		Raylib::GetWorldToScreen(Raylib::Vector3 { model.max.x, model.min.y, model.min.z }, player_camera),
		Raylib::GetWorldToScreen(Raylib::Vector3 { model.max.x, model.min.y, model.max.z }, player_camera),
		Raylib::GetWorldToScreen(Raylib::Vector3 { model.max.x, model.max.y, model.min.z }, player_camera),
		Raylib::GetWorldToScreen(Raylib::Vector3 { model.max.x, model.max.y, model.max.z }, player_camera),
	};

	Raylib::Vector2 occlusor_screen_points[8] = {
		Raylib::GetWorldToScreen(Raylib::Vector3 { occlusor.min.x, occlusor.min.y, occlusor.min.z }, player_camera),
		Raylib::GetWorldToScreen(Raylib::Vector3 { occlusor.min.x, occlusor.min.y, occlusor.max.z }, player_camera),
		Raylib::GetWorldToScreen(Raylib::Vector3 { occlusor.min.x, occlusor.max.y, occlusor.min.z }, player_camera),
		Raylib::GetWorldToScreen(Raylib::Vector3 { occlusor.min.x, occlusor.max.y, occlusor.max.z }, player_camera),
		Raylib::GetWorldToScreen(Raylib::Vector3 { occlusor.max.x, occlusor.min.y, occlusor.min.z }, player_camera),
		Raylib::GetWorldToScreen(Raylib::Vector3 { occlusor.max.x, occlusor.min.y, occlusor.max.z }, player_camera),
		Raylib::GetWorldToScreen(Raylib::Vector3 { occlusor.max.x, occlusor.max.y, occlusor.min.z }, player_camera),
		Raylib::GetWorldToScreen(Raylib::Vector3 { occlusor.max.x, occlusor.max.y, occlusor.max.z }, player_camera),
	};

	model_screen_rec.x = +INFINITY;
	model_screen_rec.y = +INFINITY;
	model_screen_rec.width = -INFINITY;
	model_screen_rec.height = -INFINITY;

	for(int i = 0; i < 8; i++) {
		if(model_screen_points[i].x < model_screen_rec.x)
			model_screen_rec.x = model_screen_points[i].x;
		if(model_screen_points[i].y < model_screen_rec.y)
			model_screen_rec.y = model_screen_points[i].y;
		if(model_screen_points[i].x > model_screen_rec.width)
			model_screen_rec.width = model_screen_points[i].x;
		if(model_screen_points[i].y > model_screen_rec.height)
			model_screen_rec.height = model_screen_points[i].y;
	}

	model_screen_rec.width = std::abs(model_screen_rec.width - model_screen_rec.x);
	model_screen_rec.height = std::abs(model_screen_rec.height - model_screen_rec.y);

	occlusor_screen_rec.x = +INFINITY;
	occlusor_screen_rec.y = +INFINITY;
	occlusor_screen_rec.width = -INFINITY;
	occlusor_screen_rec.height = -INFINITY;

	for(int i = 0; i < 8; i++) {
		if(occlusor_screen_points[i].x < occlusor_screen_rec.x)
			occlusor_screen_rec.x = occlusor_screen_points[i].x;
		if(occlusor_screen_points[i].y < occlusor_screen_rec.y)
			occlusor_screen_rec.y = occlusor_screen_points[i].y;
		if(occlusor_screen_points[i].x > occlusor_screen_rec.width)
			occlusor_screen_rec.width = occlusor_screen_points[i].x;
		if(occlusor_screen_points[i].y > occlusor_screen_rec.height)
			occlusor_screen_rec.height = occlusor_screen_points[i].y;
	}

	occlusor_screen_rec.width = std::abs(occlusor_screen_rec.width - occlusor_screen_rec.x);
	occlusor_screen_rec.height = std::abs(occlusor_screen_rec.height - occlusor_screen_rec.y);

	if(Raylib::CheckCollisionRecs(model_screen_rec, occlusor_screen_rec) == false)
		return false;

	Raylib::Rectangle collision_rec = Raylib::GetCollisionRec(model_screen_rec, occlusor_screen_rec);

	int area_model_screen = model_screen_rec.width * model_screen_rec.height;
	int area_collision_screen = collision_rec.width * collision_rec.height;

	return area_collision_screen >= area_model_screen;

	return false;
};

void draw(double dt) {
	(void)dt;
	
	int screen_width = Raylib::GetScreenWidth();
	int screen_height = Raylib::GetScreenHeight();

	if(screen_width < 2 || screen_height < 2) {
		std::printf("[DEBUG] Screen is %ix%i, rendering aborted.\n", screen_width, screen_height);
		Raylib::BeginDrawing();
		Raylib::ClearBackground((int(Raylib::GetTime()) & 0x1) ? Raylib::Color { 0xff, 0x00, 0x00, 0xff } : Raylib::BLACK);
		Raylib::EndDrawing();
		return;
	}

	int game_main_screen_width = screen_width / 2;
	int game_main_screen_height = screen_height;
	int game_main_debug_screen_width = screen_width / 2;
	int game_main_debug_screen_height = screen_height;

	if(
		game_main_screen_width != game_main_render_texture.texture.width
		|| game_main_screen_height != game_main_render_texture.texture.height
	) {
		timecapsule::unload_render_texture(game_main_render_texture);
		
		game_main_render_texture = timecapsule::load_render_texture(game_main_screen_width, game_main_screen_height);
		if(Raylib::IsRenderTextureValid(game_main_render_texture) == false) {
			std::fprintf(stderr, "Couldn't load game main render texture.\n");
			deinit();
			std::exit(1);
		}
	}

	if(
		game_main_debug_screen_width != game_debug_render_texture.texture.width
		|| game_main_debug_screen_height != game_debug_render_texture.texture.height
	) {
		timecapsule::unload_render_texture(game_debug_render_texture);
		
		game_debug_render_texture = timecapsule::load_render_texture(game_main_debug_screen_width, game_main_debug_screen_height);
		if(Raylib::IsRenderTextureValid(game_debug_render_texture) == false) {
			std::fprintf(stderr, "Couldn't load game main render texture.\n");
			deinit();
			std::exit(1);
		}
	}

	world_scene.render(player_camera, game_main_render_texture);
	world_scene.render(debug_camera, game_debug_render_texture);

	/*
	if(false) {
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
	}

	Raylib::Color colors[9] = {
		Raylib::ColorLerp(Raylib::GREEN, Raylib::YELLOW, 0.f),
		Raylib::ColorLerp(Raylib::GREEN, Raylib::YELLOW, 1.f*(1.f/3.f)),
		Raylib::ColorLerp(Raylib::GREEN, Raylib::YELLOW, 2.f*(1.f/3.f)),
		Raylib::ColorLerp(Raylib::GREEN, Raylib::YELLOW,3.f*(1.f/3.f)),
		Raylib::ColorLerp(Raylib::YELLOW, Raylib::RED, 1.f*(1.f/3.f)),
		Raylib::ColorLerp(Raylib::YELLOW, Raylib::RED, 2.f*(1.f/3.f)),
		Raylib::ColorLerp(Raylib::YELLOW, Raylib::RED,3.f*(1.f/3.f)),
		Raylib::RED,
		Raylib::PURPLE,
	};

	Raylib::DrawModel(
		model_house,
		Raylib::Vector3 {
			0.f,
			0.f,
			0.f,
		},
		1.f,
		Raylib::WHITE
	);

	Raylib::DrawModel(
		model_person,
		Raylib::Vector3 {
			-2.f,
			0.f,
			5.f,
		},
		2.2f,
		Raylib::WHITE
	);

	for(int x = 0; x < 8; x++) {
		for(int z = 0; z < 8; z++) {
			Raylib::DrawModel(
				model_person,
				Raylib::Vector3 {
					-2.f + (x + 2) * 2.f,
					0.f,
					5.f + (z + 2) * 2.f,
				},
				2.2f,
				Raylib::WHITE
			);
		};
	};

	Raylib::BoundingBox model_bb = Raylib::GetModelBoundingBox(model_person);
	model_bb.min.x += 0.f;
	model_bb.min.y += 0.f;
	model_bb.min.z += 0.f;
	model_bb.max.x += 0.f;
	model_bb.max.y += 0.f;
	model_bb.max.z += 0.f;

	Raylib::BoundingBox occlusor_bb {
		Raylib::Vector3 {
			4.f,
			0.f,
			0.f,
		},
		Raylib::Vector3 {
			4.f,
			4.f,
			4.f,
		},
	};

	bool is_occluded = is_model_completely_occluded(model_bb, occlusor_bb);

	if(false) {
		if(is_occluded)
			Raylib::DrawBoundingBox(model_bb, Raylib::RED);
		else
			Raylib::DrawBoundingBox(model_bb, Raylib::GREEN);

		Raylib::DrawBoundingBox(occlusor_bb, Raylib::GRAY);
	}

	if(is_camera_third_person) {
		Raylib::DrawCapsule(
			Raylib::Vector3 {
				player_camera.target.x,
				player_camera.target.y - 1.50f,
				player_camera.target.z,
			},
			Raylib::Vector3 {
				player_camera.target.x,
				player_camera.target.y,
				player_camera.target.z,
			},
			0.50f,
			16,
			16,
			Raylib::DARKBLUE
		);
	}
	
	Raylib::EndMode3D();

	if(false) {
		Raylib::DrawRectanglePro(
			occlusor_screen_rec,
			Raylib::Vector2 { 0.f, 0.f },
			0.f,
			Raylib::ColorAlpha(Raylib::PURPLE, 0.1f)
		);
		
		Raylib::DrawRectanglePro(
			model_screen_rec,
			Raylib::Vector2 { 0.f, 0.f },
			0.f,
			Raylib::ColorAlpha(Raylib::YELLOW, 0.1f)
		);
	}

	if(true) {
		debug_draw_camera_info(screen_width - 300, 10, screen_width, screen_height);
		
		if(game_state == GAME_STATE_PAUSED) {
			for(int y = -1; y < 2; y++)
				for(int x = -1; x < 2; x++)
					if(x != 0 && y != 0)
					Raylib::DrawText("Game paused", x + 16, y + 16, 32, Raylib::BLACK);
			
			Raylib::DrawText("Game paused", 16, 16, 32, Raylib::WHITE);
		}
	}
	*/

	Raylib::ClearBackground(Raylib::BLACK);

	/*
	Raylib::DrawTexturePro(
		game_main_render_texture.texture,
		Raylib::Rectangle {
			0.f,
			0.f,
			(float)game_main_render_texture.texture.width,
			(float)game_main_render_texture.texture.height,
		},
		Raylib::Rectangle {
			0.f,
			0.f,
			(float)game_main_render_texture.texture.width,
			(float)game_main_render_texture.texture.height,
		},
		Raylib::Vector2 {
			0.f,
			0.f,
		},
		0.f,
		Raylib::WHITE
	);
	
	Raylib::DrawTexturePro(
		game_debug_render_texture.texture,
		Raylib::Rectangle {
			0.f,
			0.f,
			(float)game_debug_render_texture.texture.width,
			(float)game_debug_render_texture.texture.height,
		},
		Raylib::Rectangle {
			(float)game_main_render_texture.texture.width,
			(float)game_main_render_texture.texture.height,
			(float)game_debug_render_texture.texture.width,
			(float)game_debug_render_texture.texture.height,
		},
		Raylib::Vector2 {
			0.f,
			0.f,
		},
		0.f,
		Raylib::WHITE
	);
	*/

	Raylib::DrawTexturePro(
		game_main_render_texture.texture,
		Raylib::Rectangle {
			0.f,
			0.f,
			(float)game_main_render_texture.texture.width,
			-(float)game_main_render_texture.texture.height,
		},
		Raylib::Rectangle {
			0.f,
			0.f,
			(float)game_main_render_texture.texture.width,
			(float)game_main_render_texture.texture.height,
		},
		Raylib::Vector2 {
			0.f,
			0.f,
		},
		0.f,
		Raylib::WHITE
	);

	Raylib::DrawTexturePro(
		game_debug_render_texture.texture,
		Raylib::Rectangle {
			0.f,
			0.f,
			(float)game_debug_render_texture.texture.width,
			-(float)game_debug_render_texture.texture.height,
		},
		Raylib::Rectangle {
			(float)game_main_render_texture.texture.width,
			0.f,
			(float)game_debug_render_texture.texture.width,
			(float)game_debug_render_texture.texture.height,
		},
		Raylib::Vector2 {
			0.f,
			0.f,
		},
		0.f,
		Raylib::WHITE
	);

	Raylib::EndDrawing();
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
