// Copyright 2024-2025 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
#pragma once

#include "jngl/Singleton.hpp"

#include <memory>
#include <unordered_map>

namespace jngl {

class Sprite;
class Texture;

class TextureCache : public Singleton<TextureCache> {
public:
	std::shared_ptr<Texture> get(std::string_view filename);
	void insert(std::string_view filename, std::shared_ptr<Texture>);
	void remove(std::string_view filename);

	std::unordered_map<std::string, std::shared_ptr<Sprite>> sprites;

private:
	// https://www.cppstories.com/2021/heterogeneous-access-cpp20/
	struct string_hash {
		using is_transparent = void;
		[[nodiscard]] size_t operator()(const char* txt) const {
			return std::hash<std::string_view>{}(txt);
		}
		[[nodiscard]] size_t operator()(std::string_view txt) const {
			return std::hash<std::string_view>{}(txt);
		}
		[[nodiscard]] size_t operator()(const std::string& txt) const {
			return std::hash<std::string>{}(txt);
		}
	};
	std::unordered_map<std::string, std::shared_ptr<Texture>, string_hash, std::equal_to<>>
	    textures;
};

} // namespace jngl
