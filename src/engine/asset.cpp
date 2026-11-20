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

#include <string>
#include <unordered_map>
#include "asset.hpp"
#include "../Ox/include/nuclei.hpp"

namespace timecapsule {
	Raylib::RenderTexture2D rl_load_render_texture_with_depth(int width, int height) {
		Raylib::RenderTexture2D target;
		target.id = target.texture.id = target.depth.id = 0;
		target.texture.width = target.depth.width = 0;

		// Load an empty framebufffer.
		target.id = Raylib::rlLoadFramebuffer();

		if(target.id > 0) {
			Raylib::rlEnableFramebuffer(target.id);

			// Create color texture (default to RGBA).
			target.texture.id = Raylib::rlLoadTexture(0, width, height, Raylib::PIXELFORMAT_UNCOMPRESSED_R8G8B8A8, 1);
			target.texture.width = width;
			target.texture.height = height;
			target.texture.format = Raylib::PIXELFORMAT_UNCOMPRESSED_R8G8B8A8;
			target.texture.mipmaps = 1;

			// Create color texture (default to RGBA).
			target.depth.id = Raylib::rlLoadTextureDepth(width, height, false);
			target.depth.width = width;
			target.depth.height = height;
			target.depth.format = 19;
			target.depth.mipmaps = 1;

			// Attach color texture and depth to FBO.
			Raylib::rlFramebufferAttach(target.id, target.texture.id, Raylib::RL_ATTACHMENT_COLOR_CHANNEL0, Raylib::RL_ATTACHMENT_TEXTURE2D, 0);
			Raylib::rlFramebufferAttach(target.id, target.depth.id, Raylib::RL_ATTACHMENT_DEPTH, Raylib::RL_ATTACHMENT_TEXTURE2D, 0);

			// Check if FBO is complete with attachments (valid).
			if(Raylib::rlFramebufferComplete(target.id)) TRACELOG(Raylib::LOG_INFO, "FBO: [ID %i] Framebuffer object created successfully", target.id);

			Raylib::rlDisableFramebuffer();
		} else {
			TRACELOG(Raylib::LOG_WARNING, "FBO: Framebuffer object can not be created");
		}

		return target;
	};

	void rl_unload_render_texture_with_depth(Raylib::RenderTexture2D target) {
		if(target.id > 0) {
			// Color texture attachment to FBO is deleted.
			Raylib::rlUnloadTexture(target.texture.id);
			Raylib::rlUnloadTexture(target.depth.id);

			// NOTE: Depth texture is automatically queried and deleted before deleting framebuffer.
			Raylib::rlUnloadFramebuffer(target.id);
		}
	};

	std::unordered_map<std::string, Raylib::Image> _loaded_images;
	std::unordered_map<std::string, Raylib::Texture2D> _loaded_textures;
	std::unordered_map<std::string, Raylib::RenderTexture2D> _loaded_render_textures;
	std::unordered_map<std::string, Raylib::RenderTexture2D> _loaded_render_textures_with_depth;
	std::unordered_map<std::string, Raylib::Font> _loaded_fonts;
	std::unordered_map<std::string, Raylib::Model> _loaded_models;

	int init_resources(void) {
		deinit_resources();
		return 0;
	};
	
	void deinit_resources(void) {
		for(auto pair : _loaded_images)
			if(Raylib::IsImageValid(pair.second))
				Raylib::UnloadImage(pair.second);
		for(auto pair : _loaded_textures)
			if(Raylib::IsTextureValid(pair.second))
				Raylib::UnloadTexture(pair.second);
		for(auto pair : _loaded_render_textures)
			if(Raylib::IsRenderTextureValid(pair.second))
				Raylib::UnloadRenderTexture(pair.second);
		for(auto pair : _loaded_render_textures_with_depth)
			if(Raylib::IsRenderTextureValid(pair.second))
				rl_unload_render_texture_with_depth(pair.second);
		for(auto pair : _loaded_fonts)
			if(Raylib::IsFontValid(pair.second))
				Raylib::UnloadFont(pair.second);
		for(auto pair : _loaded_models)
			if(Raylib::IsModelValid(pair.second))
				Raylib::UnloadModel(pair.second);
		
		_loaded_images.clear();
		_loaded_textures.clear();
		_loaded_render_textures.clear();
		_loaded_render_textures_with_depth.clear();
		_loaded_fonts.clear();
		_loaded_models.clear();
	};
	
	std::string _get_id_image(Raylib::Image img) {
		std::string id = "";
		
		for(unsigned int shift = 16; shift > 0; shift -= 4)
			id += 'A' + ((img.width >> shift) & 0xf);
		
		id += ",";
		
		for(unsigned int shift = 16; shift > 0; shift -= 4)
			id += 'A' + ((img.height >> shift) & 0xf);
		
		id += ",";
		
		for(Ox::ulong shift = 56; shift > 0; shift -= 4)
			id += 'A' + (((Ox::ulong)img.data >> shift) & 0xf);
		
		return id;
	};
	
	std::string _get_id_texture(Raylib::Texture2D tex) {
		std::string id = "";
		
		for(unsigned int shift = 16; shift > 0; shift -= 4)
			id += 'A' + ((tex.width >> shift) & 0xf);
		
		id += ",";
		
		for(unsigned int shift = 16; shift > 0; shift -= 4)
			id += 'A' + ((tex.height >> shift) & 0xf);
		
		id += ",";
		
		for(unsigned int shift = 56; shift > 0; shift -= 4)
			id += 'A' + ((tex.id >> shift) & 0xf);
		
		return id;
	};
	
	std::string _get_id_render_texture(Raylib::RenderTexture tex) {
		std::string id = "";
		
		for(unsigned int shift = 16; shift > 0; shift -= 4)
			id += 'A' + ((tex.texture.width >> shift) & 0xf);
		
		id += ",";
		
		for(unsigned int shift = 16; shift > 0; shift -= 4)
			id += 'A' + ((tex.texture.height >> shift) & 0xf);
		
		id += ",";
		
		for(unsigned int shift = 56; shift > 0; shift -= 4)
			id += 'A' + ((tex.id >> shift) & 0xf);
		
		return id;
	};
	
	std::string _get_id_font(Raylib::Font font) {
		std::string id = "";
		
		for(unsigned int shift = 16; shift > 0; shift -= 4)
			id += 'A' + ((font.texture.width >> shift) & 0xf);
		
		id += ",";
		
		for(unsigned int shift = 16; shift > 0; shift -= 4)
			id += 'A' + ((font.texture.height >> shift) & 0xf);
		
		id += ",";
		
		for(unsigned int shift = 56; shift > 0; shift -= 4)
			id += 'A' + ((font.texture.id >> shift) & 0xf);
		
		return id;
	};
	
	std::string _get_id_model(Raylib::Model model) {
		std::string id = "";
		
		for(Ox::ulong shift = 56; shift > 0; shift -= 4)
			id += 'A' + (((Ox::ulong)model.meshes >> shift) & 0xf);
		
		id += ",";
		
		for(Ox::ulong shift = 56; shift > 0; shift -= 4)
			id += 'A' + (((Ox::ulong)model.materials >> shift) & 0xf);
		
		return id;
	};
	
	Raylib::Image load_image(const char *filename) {
		Raylib::Image img = Raylib::LoadImage(filename);
		if(Raylib::IsImageValid(img))
			_loaded_images.insert_or_assign(_get_id_image(img), img);
		
		return img;
	};
	
	void unload_image(Raylib::Image img) {
		if(Raylib::IsImageValid(img) == false)
			return;
		
		std::string id = _get_id_image(img);
		Raylib::UnloadImage(img);
		
		if(_loaded_images.find(id) != _loaded_images.cend())
			_loaded_images.erase(id);
	};
	
	Raylib::Texture2D load_texture_from_image(Raylib::Image img) {
		if(Raylib::IsImageValid(img) == false)
			return Raylib::Texture2D { 0, -1, -1, -1, 0 };
		
		Raylib::Texture2D tex = Raylib::LoadTextureFromImage(img);
		if(Raylib::IsTextureValid(tex)) {
			_loaded_textures.insert_or_assign(_get_id_texture(tex), tex);
			Raylib::SetTextureFilter(tex, Raylib::TEXTURE_FILTER_BILINEAR);
		}
		
		return tex;
	};
	
	Raylib::Texture2D load_texture(const char *filename) {
		Raylib::Texture2D tex = Raylib::LoadTexture(filename);
		
		if(Raylib::IsTextureValid(tex)) {
			_loaded_textures.insert_or_assign(_get_id_texture(tex), tex);
			Raylib::SetTextureFilter(tex, Raylib::TEXTURE_FILTER_BILINEAR);
		}
		
		return tex;
	};
	
	void unload_texture(Raylib::Texture2D tex) {
		if(Raylib::IsTextureValid(tex) == false)
			return;
		
		std::string id = _get_id_texture(tex);
		Raylib::UnloadTexture(tex);
		
		if(_loaded_textures.find(id) != _loaded_textures.cend())
			_loaded_textures.erase(id);
	};
	
	Raylib::RenderTexture2D load_render_texture(int width, int height) {
		Raylib::RenderTexture2D tex = Raylib::LoadRenderTexture(width, height);
		if(Raylib::IsRenderTextureValid(tex) == false)
			return tex;
		
		_loaded_render_textures.insert_or_assign(_get_id_render_texture(tex), tex);
		Raylib::SetTextureFilter(tex.texture, Raylib::TEXTURE_FILTER_BILINEAR);
		return tex;
	};

	void unload_render_texture(Raylib::RenderTexture2D tex) {
		if(Raylib::IsRenderTextureValid(tex) == false)
			return;
		
		std::string id = _get_id_render_texture(tex);
		Raylib::UnloadRenderTexture(tex);
		
		if(_loaded_render_textures.find(id) != _loaded_render_textures.cend())
			_loaded_render_textures.erase(id);
	};

	Raylib::RenderTexture2D load_render_texture_with_depth(int width, int height) {
		Raylib::RenderTexture2D tex = rl_load_render_texture_with_depth(width, height);
		if(Raylib::IsRenderTextureValid(tex) == false)
			return tex;
		
		_loaded_render_textures_with_depth.insert_or_assign(_get_id_render_texture(tex), tex);
		Raylib::SetTextureFilter(tex.texture, Raylib::TEXTURE_FILTER_BILINEAR);
		return tex;
	};

	void unload_render_texture_with_depth(timecapsule::gl_render_texture_t tex) {
		if(Raylib::IsRenderTextureValid(tex) == false)
			return;
		
		std::string id = _get_id_render_texture(tex);
		rl_unload_render_texture_with_depth(tex);
		
		if(_loaded_render_textures_with_depth.find(id) != _loaded_render_textures_with_depth.cend())
			_loaded_render_textures_with_depth.erase(id);
	};
	
	Raylib::Font load_font(const char *filename, int font_size, int num_of_characters_to_load) {
		Raylib::Font font = Raylib::LoadFontEx(
			filename,
			font_size,
			nullptr,
			num_of_characters_to_load
		);
		
		if(Raylib::IsFontValid(font))
			_loaded_fonts.insert_or_assign(_get_id_font(font), font);
		
		return font;
	};
	
	void unload_font(Raylib::Font font) {
		if(Raylib::IsFontValid(font) == false)
			return;
		
		std::string id = _get_id_font(font);
		Raylib::UnloadFont(font);
		
		if(_loaded_fonts.find(id) != _loaded_fonts.cend())
			_loaded_fonts.erase(id);
	};
	
	Raylib::Model load_model(const char *filename) {
		Raylib::Model model = Raylib::LoadModel(filename);
		
		if(Raylib::IsModelValid(model))
			_loaded_models.insert_or_assign(_get_id_model(model), model);
		
		return model;
	};
	
	Raylib::Model load_model_from_mesh(Raylib::Mesh mesh) {
		Raylib::Model model = Raylib::LoadModelFromMesh(mesh);
		
		if(Raylib::IsModelValid(model))
			_loaded_models.insert_or_assign(_get_id_model(model), model);
		
		return model;
	};

	void unload_model(Raylib::Model model) {
		if(Raylib::IsModelValid(model) == false)
			return;
		
		std::string id = _get_id_model(model);
		Raylib::UnloadModel(model);
		
		if(_loaded_models.find(id) != _loaded_models.cend())
			_loaded_models.erase(id);
	};

	Raylib::Model update_model_diffuse_texture(Raylib::Model model, Raylib::Texture2D updated_diffuse_texture) {
		if(Raylib::IsModelValid(model) == false)
			return model;

		std::string id = _get_id_model(model);
		if(_loaded_models.find(id) != _loaded_models.cend())
			_loaded_models.erase(id);

		model.materials[0].maps[Raylib::MATERIAL_MAP_DIFFUSE].texture = updated_diffuse_texture;

		if(Raylib::IsModelValid(model))
			_loaded_models.insert_or_assign(_get_id_model(model), model);

		return model;
	};
	
	void debug_copy_texture(gl_texture_t &target, unsigned int it, unsigned int &len, std::string &target_id) {
		unsigned int i = 0;
		target.id = 0;
		len = _loaded_textures.size();
		for(std::pair<std::string, Raylib::Texture2D> entry : _loaded_textures) {
			if(i != it) {
				i++;
				continue;
			}
			
			target_id = entry.first;
			target = entry.second;
			
			break;
		};
	};
	void debug_copy_render_texture(gl_render_texture_t &target, unsigned int it, unsigned int &len, std::string &target_id) {
		unsigned int i = 0;
		target.id = 0;
		len = _loaded_render_textures.size();
		for(std::pair<std::string, Raylib::RenderTexture2D> entry : _loaded_render_textures) {
			if(i != it) {
				i++;
				continue;
			}
			
			target_id = entry.first;
			target = entry.second;
			
			break;
		};
	};
	void debug_copy_render_texture_with_depth(gl_render_texture_t &target, unsigned int it, unsigned int &len, std::string &target_id) {
		unsigned int i = 0;
		target.id = 0;
		len = _loaded_render_textures_with_depth.size();
		for(std::pair<std::string, Raylib::RenderTexture2D> entry : _loaded_render_textures_with_depth) {
			if(i != it) {
				i++;
				continue;
			}
			
			target_id = entry.first;
			target = entry.second;
			
			break;
		};
	};
	void debug_copy_model(gl_model_t &target, unsigned int it, unsigned int &len, std::string &target_id) {
		unsigned int i = 0;
		target.meshes = nullptr, target.meshCount = 0;
		len = _loaded_render_textures_with_depth.size();
		for(std::pair<std::string, Raylib::Model> entry : _loaded_models) {
			if(i != it) {
				i++;
				continue;
			}
			
			target_id = entry.first;
			target = entry.second;
			
			break;
		};
	};
};
