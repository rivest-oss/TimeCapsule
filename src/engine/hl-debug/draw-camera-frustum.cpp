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
		void draw_camera_frustrum_in_3d(camera_t camera, Raylib::Vector3 camera_sphere_radius_ref, Raylib::Texture2D &game_play_texture) {
			Raylib::Color bright_yellow = Raylib::YELLOW;
			Raylib::Color dark_yellow = Raylib::Color { 0xff, 0x99, 0x00, 0xff };
			
			// center
			Raylib::Ray ray_center = Raylib::GetScreenToWorldRayEx(
				Raylib::Vector2 {
					float(game_play_texture.width) / 2.f,
					float(game_play_texture.height) / 2.f,
				},
				camera.current,
				game_play_texture.width,
				game_play_texture.height
			);
			
			// top left
			Raylib::Ray ray_tl = Raylib::GetScreenToWorldRayEx(
				Raylib::Vector2 {
					0.f,
					0.f,
				},
				camera.current,
				game_play_texture.width,
				game_play_texture.height
			);
			// top right
			Raylib::Ray ray_tr = Raylib::GetScreenToWorldRayEx(
				Raylib::Vector2 {
					(float)game_play_texture.width - 1,
					0.f,
				},
				camera.current,
				game_play_texture.width,
				game_play_texture.height
			);
			// bottom left
			Raylib::Ray ray_bl = Raylib::GetScreenToWorldRayEx(
				Raylib::Vector2 {
					0.f,
					(float)game_play_texture.height - 1,
				},
				camera.current,
				game_play_texture.width,
				game_play_texture.height
			);
			// bottom right
			Raylib::Ray ray_br = Raylib::GetScreenToWorldRayEx(
				Raylib::Vector2 {
					(float)game_play_texture.width - 1,
					(float)game_play_texture.height - 1,
				},
				camera.current,
				game_play_texture.width,
				game_play_texture.height
			);
			
			float dist_near = float(Raylib::rlGetCullDistanceNear());
			//float dist_far = float(Raylib::rlGetCullDistanceFar());
			float dist_far = dist_near + 0.1f;
			
			// drawing
			
			// (center & corners, 0,0 to dist_far)
			{
				// draw center
				Raylib::DrawLine3D(
					Raylib::Vector3 {
						camera.current.position.x,
						camera.current.position.y,
						camera.current.position.z,
					},
					Raylib::Vector3 {
						camera.current.position.x + ray_center.direction.x * dist_far,
						camera.current.position.y + ray_center.direction.y * dist_far,
						camera.current.position.z + ray_center.direction.z * dist_far,
					},
					dark_yellow
				);
				// draw top left
				Raylib::DrawLine3D(
					Raylib::Vector3 {
						camera.current.position.x,
						camera.current.position.y,
						camera.current.position.z,
					},
					Raylib::Vector3 {
						camera.current.position.x + ray_tl.direction.x * dist_far,
						camera.current.position.y + ray_tl.direction.y * dist_far,
						camera.current.position.z + ray_tl.direction.z * dist_far,
					},
					dark_yellow
				);
				// draw top right
				Raylib::DrawLine3D(
					Raylib::Vector3 {
						camera.current.position.x,
						camera.current.position.y,
						camera.current.position.z,
					},
					Raylib::Vector3 {
						camera.current.position.x + ray_tr.direction.x * dist_far,
						camera.current.position.y + ray_tr.direction.y * dist_far,
						camera.current.position.z + ray_tr.direction.z * dist_far,
					},
					dark_yellow
				);
				// draw bottom left
				Raylib::DrawLine3D(
					Raylib::Vector3 {
						camera.current.position.x,
						camera.current.position.y,
						camera.current.position.z,
					},
					Raylib::Vector3 {
						camera.current.position.x + ray_bl.direction.x * dist_far,
						camera.current.position.y + ray_bl.direction.y * dist_far,
						camera.current.position.z + ray_bl.direction.z * dist_far,
					},
					dark_yellow
				);
				// draw bottom right
				Raylib::DrawLine3D(
					Raylib::Vector3 {
						camera.current.position.x,
						camera.current.position.y,
						camera.current.position.z,
					},
					Raylib::Vector3 {
						camera.current.position.x + ray_br.direction.x * dist_far,
						camera.current.position.y + ray_br.direction.y * dist_far,
						camera.current.position.z + ray_br.direction.z * dist_far,
					},
					dark_yellow
				);
			}
			
			// (corners, dist_near)
			{
				// TL->TR
				Raylib::DrawLine3D(
					Raylib::Vector3 {
						camera.current.position.x + ray_tl.direction.x * dist_near,
						camera.current.position.y + ray_tl.direction.y * dist_near,
						camera.current.position.z + ray_tl.direction.z * dist_near,
					},
					Raylib::Vector3 {
						camera.current.position.x + ray_tr.direction.x * dist_near,
						camera.current.position.y + ray_tr.direction.y * dist_near,
						camera.current.position.z + ray_tr.direction.z * dist_near,
					},
					bright_yellow
				);
				// BL->BR
				Raylib::DrawLine3D(
					Raylib::Vector3 {
						camera.current.position.x + ray_bl.direction.x * dist_near,
						camera.current.position.y + ray_bl.direction.y * dist_near,
						camera.current.position.z + ray_bl.direction.z * dist_near,
					},
					Raylib::Vector3 {
						camera.current.position.x + ray_br.direction.x * dist_near,
						camera.current.position.y + ray_br.direction.y * dist_near,
						camera.current.position.z + ray_br.direction.z * dist_near,
					},
					bright_yellow
				);
				// TL->BL
				Raylib::DrawLine3D(
					Raylib::Vector3 {
						camera.current.position.x + ray_tl.direction.x * dist_near,
						camera.current.position.y + ray_tl.direction.y * dist_near,
						camera.current.position.z + ray_tl.direction.z * dist_near,
					},
					Raylib::Vector3 {
						camera.current.position.x + ray_bl.direction.x * dist_near,
						camera.current.position.y + ray_bl.direction.y * dist_near,
						camera.current.position.z + ray_bl.direction.z * dist_near,
					},
					bright_yellow
				);
				// TR->BR
				Raylib::DrawLine3D(
					Raylib::Vector3 {
						camera.current.position.x + ray_tr.direction.x * dist_near,
						camera.current.position.y + ray_tr.direction.y * dist_near,
						camera.current.position.z + ray_tr.direction.z * dist_near,
					},
					Raylib::Vector3 {
						camera.current.position.x + ray_br.direction.x * dist_near,
						camera.current.position.y + ray_br.direction.y * dist_near,
						camera.current.position.z + ray_br.direction.z * dist_near,
					},
					bright_yellow
				);
			}
			
			// (corners, dist_far)
			{
				// TL->TR
				Raylib::DrawLine3D(
					Raylib::Vector3 {
						camera.current.position.x + ray_tl.direction.x * dist_far,
						camera.current.position.y + ray_tl.direction.y * dist_far,
						camera.current.position.z + ray_tl.direction.z * dist_far,
					},
					Raylib::Vector3 {
						camera.current.position.x + ray_tr.direction.x * dist_far,
						camera.current.position.y + ray_tr.direction.y * dist_far,
						camera.current.position.z + ray_tr.direction.z * dist_far,
					},
					bright_yellow
				);
				// BL->BR
				Raylib::DrawLine3D(
					Raylib::Vector3 {
						camera.current.position.x + ray_bl.direction.x * dist_far,
						camera.current.position.y + ray_bl.direction.y * dist_far,
						camera.current.position.z + ray_bl.direction.z * dist_far,
					},
					Raylib::Vector3 {
						camera.current.position.x + ray_br.direction.x * dist_far,
						camera.current.position.y + ray_br.direction.y * dist_far,
						camera.current.position.z + ray_br.direction.z * dist_far,
					},
					bright_yellow
				);
				// TL->BL
				Raylib::DrawLine3D(
					Raylib::Vector3 {
						camera.current.position.x + ray_tl.direction.x * dist_far,
						camera.current.position.y + ray_tl.direction.y * dist_far,
						camera.current.position.z + ray_tl.direction.z * dist_far,
					},
					Raylib::Vector3 {
						camera.current.position.x + ray_bl.direction.x * dist_far,
						camera.current.position.y + ray_bl.direction.y * dist_far,
						camera.current.position.z + ray_bl.direction.z * dist_far,
					},
					bright_yellow
				);
				// TR->BR
				Raylib::DrawLine3D(
					Raylib::Vector3 {
						camera.current.position.x + ray_tr.direction.x * dist_far,
						camera.current.position.y + ray_tr.direction.y * dist_far,
						camera.current.position.z + ray_tr.direction.z * dist_far,
					},
					Raylib::Vector3 {
						camera.current.position.x + ray_br.direction.x * dist_far,
						camera.current.position.y + ray_br.direction.y * dist_far,
						camera.current.position.z + ray_br.direction.z * dist_far,
					},
					bright_yellow
				);
			}
			
			// draw camera position sphere
			{
				float camera_sphere_radius = vdistance(
					camera.current.position,
					camera_sphere_radius_ref
				);
				
				if(camera_sphere_radius < 1.f) camera_sphere_radius = 1.f;
				camera_sphere_radius *= 0.001f;
				
				Raylib::DrawSphere(camera.current.position, camera_sphere_radius, Raylib::WHITE);
			}
		};
	};
};
