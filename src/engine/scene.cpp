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

#include "scene.hpp"
#include "math.hpp"
#include "gl.hpp"
#include "lighting.hpp"

using namespace Raylib;
#include "../thirdparty/raylibExtras/RLFrustum.hpp"

namespace timecapsule {
	model_t invalid_model {
		timecapsule::gl_model_t {
			Raylib::Matrix {},
			0,
			0,
			nullptr,
			nullptr,
			nullptr,
			Raylib::ModelSkeleton {},
			Raylib::ModelAnimPose {},
			nullptr,
		},
		timecapsule::vector3_t {},
		timecapsule::vector3_t {},
		timecapsule::vector3_t {},
		timecapsule::srgb_color_t { 0x00, 0x00, 0x00, 0x00 },
		false,
	};
	
	Scene::Scene(void) {
		models.clear();
	};

	Scene::~Scene(void) {
		models.clear();
	};

	void Scene::deinit(void) {
		models.clear();
	};

	model_t &Scene::add_model(std::string id, timecapsule::gl_model_t model) {
		if(model_exists(id))
			models.erase(id);

		if(timecapsule::is_model_valid(model) == false)
			return invalid_model;

		models.insert_or_assign(id, timecapsule::model_t {
			timecapsule::gl_model_t {
				model.transform,
				model.meshCount,
				model.materialCount,
				model.meshes,
				model.materials,
				model.meshMaterial,
				model.skeleton,
				model.currentPose,
				model.boneMatrices,
			},
			timecapsule::vector3_t { 0.f, 0.f, 0.f },
			timecapsule::vector3_t { 1.f, 1.f, 1.f },
			timecapsule::vector3_t { 0.f, 0.f, 0.f },
			timecapsule::srgb_color_t { 0xff, 0xff, 0xff, 0xff },
			true,
		});

		return models.at(id);
	};

	model_t &Scene::get_model(std::string id) {
		if(model_exists(id) == false)
			return invalid_model;
		
		return models.at(id);
	};

	bool Scene::model_exists(std::string id) {
		return models.find(id) != models.cend();
	};

	void Scene::render(timecapsule::camera_t camera, timecapsule::gl_render_texture_t target, bool update_cull) {
		if(Raylib::IsRenderTextureValid(target) == false)
			return;

		Raylib::BeginTextureMode(target);
		render(camera, update_cull);
		Raylib::EndTextureMode();
	};

	void Scene::render(camera_t camera, bool update_cull) {
		triangles_count = 0;
		
		Raylib::ClearBackground(Raylib::BLACK);
		Raylib::BeginMode3D(camera.current);

		timecapsule::lighting::update(camera);
		timecapsule::lighting::begin_rendering();

		for(std::pair<std::string, model_t> entry : models) {
			model_t model = entry.second;
			Raylib::Model rmodel = model.rmodel;
			
			if(Raylib::IsModelValid(rmodel) == false || model.visible == false)
				continue;
			
			rmodel.transform = Raylib::MatrixMultiply(
				Raylib::MatrixMultiply(
					Raylib::MatrixScale(model.scale.x, model.scale.y, model.scale.z),
					Raylib::MatrixRotateXYZ(Raylib::Vector3 {
						model.rotation.x * M_PI2f,
						model.rotation.y * M_PI2f,
						model.rotation.z * M_PI2f,
					})
				),
				Raylib::MatrixTranslate(model.position.x, model.position.y, model.position.z)
			);
			
			Raylib::BoundingBox bb;
			
			if(rmodel.meshCount > 0) {
				Raylib::Vector3 temp { 0.f, 0.f, 0.f };
				bb = Raylib::GetMeshBoundingBox(rmodel.meshes[0]);
				
				for(int i = 1; i < rmodel.meshCount; i++) {
					Raylib::BoundingBox tempBounds = Raylib::GetMeshBoundingBox(rmodel.meshes[i]);
					tempBounds.min = Raylib::Vector3Transform(tempBounds.min, rmodel.transform);
					tempBounds.max = Raylib::Vector3Transform(tempBounds.max, rmodel.transform);
					
					temp.x = (bb.min.x < tempBounds.min.x) ? bb.min.x : tempBounds.min.x;
					temp.y = (bb.min.y < tempBounds.min.y) ? bb.min.y : tempBounds.min.y;
					temp.z = (bb.min.z < tempBounds.min.z) ? bb.min.z : tempBounds.min.z;
					bb.min = temp;
					
					temp.x = (bb.max.x > tempBounds.max.x) ? bb.max.x : tempBounds.max.x;
					temp.y = (bb.max.y > tempBounds.max.y) ? bb.max.y : tempBounds.max.y;
					temp.z = (bb.max.z > tempBounds.max.z) ? bb.max.z : tempBounds.max.z;
					bb.max = temp;
				};
			}

			RLFrustum frustum;
			frustum.Extract();

			if(update_cull) {
				model.culled = frustum.AABBoxIn(bb.min, bb.max) == false;
				models.at(entry.first).culled = model.culled;
			}

			if(model.culled)
				continue;

			timecapsule::lighting::update_model_lighting(rmodel);
			
			Raylib::DrawModel(
				rmodel,
				Raylib::Vector3 {
					0.f,
					0.f,
					0.f,
				},
				1.f,
				model.tint
			);

			for(int i = 0; i < rmodel.meshCount; i++)
				triangles_count += rmodel.meshes[i].triangleCount;
		};
		
		timecapsule::lighting::end_rendering();
		Raylib::EndMode3D();
	};

	unsigned int Scene::get_sent_triangles_count(void) {
		return triangles_count;
	};
};
