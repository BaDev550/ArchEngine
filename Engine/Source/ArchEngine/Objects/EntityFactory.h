#pragma once
#include "ArchEngine/Core/Memory.h"
#include <string>
#include <unordered_map>
#include <functional>

namespace ae {
    class Entity;
    using EntityInstantiator = std::function<memory::Ref<Entity>()>;
    class EntityFactory {
    public:
        static void Register(const std::string& className, EntityInstantiator instantiator) {
            GetRegistry()[className] = instantiator;
        }

        static memory::Ref<Entity> Create(const std::string& name) {
            auto& registry = GetRegistry();
            if (registry.find(name) != registry.end())
                return registry[name]();
            return nullptr;
        }
    private:
        static std::unordered_map<std::string, EntityInstantiator>& GetRegistry() {
            static std::unordered_map<std::string, EntityInstantiator> s_Registry;
            return s_Registry;
        }
    };
}
#define REGISTER_ENTITY(ClassName) \
    inline static bool ClassName##_Registered = []() { \
        ae::EntityFactory::Register(#ClassName, []() { return ae::memory::Ref<ClassName>::Create(); }); \
        return true; \
    }()