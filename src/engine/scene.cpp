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
#include "../math.hpp"

namespace Raylib {
	#include <raylib.h>
	#include <raymath.h>
};

namespace timecapsule {
	Scene::Scene(void) {
		models.clear();
	};

	Scene::~Scene(void) {
		models.clear();
	};

	void Scene::deinit(void) {
		models.clear();
	};

	model_t &Scene::add_model(std::string id, Raylib::Model model) {
		if(model_exists(id))
			models.erase(id);

		if(Raylib::IsModelValid(model) == false)
			return invalid_model;

		models.insert_or_assign(id, model_t {
			Raylib::Model {
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
			Raylib::Vector3 { 0.f, 0.f, 0.f },
			Raylib::Vector3 { 1.f, 1.f, 1.f },
			Raylib::Vector3 { 0.f, 0.f, 0.f },
			Raylib::WHITE
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

	void Scene::render(Raylib::Camera3D camera, Raylib::RenderTexture target) {
		if(Raylib::IsRenderTextureValid(target) == false)
			return;

		Raylib::BeginTextureMode(target);
		render(camera);
		Raylib::EndTextureMode();
	};

	void Scene::render(Raylib::Camera3D camera) {
		Raylib::ClearBackground(Raylib::BLACK);
		Raylib::BeginMode3D(camera);

		for(std::pair<std::string, model_t> entry : models) {
			model_t model = entry.second;
			Raylib::Model rmodel = model.rmodel;
			
			if(Raylib::IsModelValid(rmodel) == false)
				continue;

			rmodel.transform = Raylib::MatrixRotateXYZ(Raylib::Vector3 {
				model.rotation.x * M_PI2f,
				model.rotation.y * M_PI2f,
				model.rotation.z * M_PI2f,
			});

			Raylib::DrawModelEx(
				rmodel,
				model.position,
				Raylib::Vector3 { 0.f, 0.f, 0.f },
				0.f,
				model.scale,
				model.tint
			);
		};
		
		Raylib::EndMode3D();
	};
};
