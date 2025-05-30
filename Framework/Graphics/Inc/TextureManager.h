#pragma once

#include "Texture.h"

namespace IExeEngine::Graphics
{
	using TextureId = std::size_t;

	class TextureManager final
	{
	public:
		static void StaticInitialize(const std::filesystem::path& root);
		static void StaticTerminate();
		static TextureManger* Get();

		TextureManager() = default;
		~TextureManager();

		TextureManager(const TextureManger&) = delete;
		TextureManager(const TextureManager&&) = delete;
		TextureManager& operator=(const TextureManager&) = delete;
		TextureManager& operator=(const TextureManager&&) = delete;

		void SetRootDirectory(const std::filesystem::path& root);
		TextureId LoadTexture(const std::filesystem::path& filename, bool useRootDir = true);
		const Texture* GetTexture(TextureId id);
		void ReleaseTexture(TextureId id);

		void BindVS(TextureId id, uint32_t slot) const;
		void BindPS(TextureId id, uint32_t slot) const;

	private:
		struct Entry
		{

		};
	};
}
