#include <Level.h>

namespace GameEngine::World
{
	Level::Level(std::string& name)
		: m_Name(std::move(name))
	{
	}

	void Level::AddLevelObject(const LevelObject& levelObject)
	{
		levelObject.SetLevelObjectId(m_LevelObjectList.size());
		m_LevelObjectList.push_back(levelObject);
	}
}