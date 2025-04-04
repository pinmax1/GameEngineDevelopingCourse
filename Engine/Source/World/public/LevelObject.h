#pragma once

#include <World/export.h>
#include <unordered_map>

namespace GameEngine::World
{

	class WORLD_API LevelObject final
	{
	public:
		using ComponentName = std::string;
		using ComponentDesc = std::string;
		struct Component {
			ComponentName name;
			ComponentDesc desc;
			size_t id;
			ComponentDesc& GetDesc() {
				return desc;
			}
		};

		using ComponentList = std::unordered_map<size_t, Component>;

	public:
		LevelObject() = default;
		~LevelObject() = default;

	public:
		void SetName(const char* name) { m_Name = name; }
		const std::string& GetName() const { return m_Name; }
		void AddComponent(const ComponentName& name, const ComponentDesc& desc);
		const ComponentList& GetComponents() const { return m_ComponentList; }
		ComponentList& GetComponents() { return m_ComponentList; }
		void SetLevelObjectId(size_t id) const { levelObjectId = id; }
		size_t GetLevelObjectId() const { return levelObjectId; }
		Component& GetComponentById(size_t id) { return m_ComponentList[id]; }

	private:
		std::string m_Name = "None";
		ComponentList m_ComponentList;
		size_t curComponentId = 0;
		mutable size_t levelObjectId = 0;
	};
}