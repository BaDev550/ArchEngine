#pragma once
#include "ArchEngine/Core/Memory.h"
#include "ArchEngine/Grapichs/TextureCube.h"

namespace ae::grapichs {
	class Enviroment : public Asset {
	public:
		Enviroment() = default;
		Enviroment(const memory::Ref<TextureCube>& enviromentMap) : _environmentMap(enviromentMap) {}

		void SetEnviromentMap(memory::Ref<TextureCube>& environmentMap) { _environmentMap = environmentMap; }
		memory::Ref<TextureCube>& GetEnvironmentMap() { return _environmentMap; }

		static AssetType GetStaticAssetType() { return AssetType::EnviromentMap; }
		virtual AssetType GetAssetType() const { return GetStaticAssetType(); }
	private:
		memory::Ref<TextureCube> _environmentMap;
	};
}