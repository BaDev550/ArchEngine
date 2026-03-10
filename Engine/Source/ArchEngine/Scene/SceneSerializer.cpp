#include "ArchPch.h"
#include "SceneSerializer.h"
#include <yaml-cpp/yaml.h>
#include <fstream>
#include "Scene.h"

namespace ae {
	YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec3& v) {
		out << YAML::Flow << YAML::BeginSeq << v.x << v.y << v.z << YAML::EndSeq;
		return out;
	}

	SceneSerializer::SceneSerializer(const memory::Ref<Scene>& scene) : _scene(scene) {}
	void SceneSerializer::Serialize(const std::string& filepath)
	{
        YAML::Emitter out;
        out << YAML::BeginMap;
        out << YAML::Key << "Scene" << YAML::Value << _scene->GetName();
        out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;

        for (auto& [id, entity] : _scene->GetEntities()) {
            out << YAML::BeginMap;
            out << YAML::Key << "EntityID" << YAML::Value << (uint64_t)id;
            out << YAML::Key << "Type" << YAML::Value << _scene->GetEntityType(id);
            out << YAML::Key << "Name" << YAML::Value << entity->GetName();
            out << YAML::Key << "Transform" << YAML::BeginMap;
            out << YAML::Key << "Position" << YAML::Value << entity->GetPosition();
            out << YAML::Key << "Rotation" << YAML::Value << entity->GetEulerRotation();
            out << YAML::Key << "Scale" << YAML::Value << entity->GetScale();
#if 0
            if (entity->IsDrawnable()) {
                out << YAML::Key << "Drawnable" << YAML::BeginMap;
                out << YAML::Key << "StaticMeshHandle" << YAML::Value << entity->GetDrawnable().StaticMeshHandle;
                out << YAML::Key << "IsVisible" << YAML::Value << entity->GetDrawnable().IsVisible;
                out << YAML::EndMap;
            }
#endif
            entity->OnSerialize(out);
            out << YAML::EndMap;
            out << YAML::EndMap;
        }

        out << YAML::EndSeq;
        out << YAML::EndMap;

        std::ofstream fout(filepath);
        fout << out.c_str();
	}

    bool SceneSerializer::Deserialize(const std::string& filepath)
	{
        _scene->Destroy();

        std::ifstream stream(filepath);
        std::stringstream strStream;
        strStream << stream.rdbuf();

        YAML::Node data = YAML::Load(strStream.str());
        if (!data["Scene"]) return false;

        auto entities = data["Entities"];
        if (entities) {
            for (auto entityNode : entities) {
                uint64_t id = entityNode["EntityID"].as<uint64_t>();
                std::string type = entityNode["Type"].as<std::string>();
                std::string name = entityNode["Name"].as<std::string>();
                Entity* deserializedEntity = _scene->CreateEntity(type, id);
                if (deserializedEntity) {
                    deserializedEntity->SetName(name);
                    deserializedEntity->OnDeserialize(entityNode);
                    auto transform = entityNode["Transform"];
                    deserializedEntity->SetPosition({ transform["Position"][0].as<float>(), transform["Position"][1].as<float>(), transform["Position"][2].as<float>() });
                    deserializedEntity->SetRotation({ transform["Rotation"][0].as<float>(), transform["Rotation"][1].as<float>(), transform["Rotation"][2].as<float>() });
                    deserializedEntity->SetScale({ transform["Scale"][0].as<float>(), transform["Scale"][1].as<float>(), transform["Scale"][2].as<float>() });

#if 0
                    auto drawnable = entityNode["Drawnable"];
                    if (drawnable) {
                        AssetHandle handle = drawnable["StaticMeshHandle"].as<uint64_t>();
                        bool isVisible = drawnable["IsVisible"].as<bool>();
                        deserializedEntity->RegisterAsDrawnable(handle);
                        deserializedEntity->GetDrawnable().IsVisible = isVisible;
                    }
#endif
                }
            }
        }
        return true;
	}
}