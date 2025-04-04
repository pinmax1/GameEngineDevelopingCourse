#include <LevelObject.h>

namespace GameEngine::World
{
	void LevelObject::AddComponent(const ComponentName& name, const ComponentDesc& desc)
	{
		m_ComponentList[curComponentId] = Component(name, desc, curComponentId);
		++curComponentId;
	}
}