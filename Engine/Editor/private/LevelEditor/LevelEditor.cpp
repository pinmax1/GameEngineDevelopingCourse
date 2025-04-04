#include <LevelEditor/LevelEditor.h>
#include <LevelEditor/LevelSerializer.h>
#include <LevelEditor/ECS/ecsLevelEditor.h>

#include <Filesystem.h>
#include <ECS/ecsSystems.h>
#include <Parser/WorldParser.h>

#include <imgui.h>
#include <imgui_stdlib.h>

namespace GameEngine
{
	namespace Editor
	{
		LevelEditor::LevelEditor(flecs::world& world)
		{
			m_Level = LevelSerializer::Deserialize(Core::g_FileSystem->GetFilePath("Levels/Main.xml").generic_string());
			auto levelObjects = m_Level->GetLevelObjects();
			for (int i = 0; i < levelObjects.size(); ++i)
			{
				World::LevelObject levelObject = levelObjects[i];

				flecs::entity entity = world.entity(levelObject.GetName().c_str());

				World::LevelObject::ComponentList& componentList = levelObject.GetComponents();

				World::LevelObject::ComponentList::iterator positionAttribute = std::ranges::find_if(componentList,
					[](auto& component)
					{
						return !std::strcmp(component.second.name.c_str(), "Position");
					}
				);

				World::LevelObject::ComponentList::iterator geometryAttribute = std::ranges::find_if(componentList,
					[](auto& component)
					{
						return !std::strcmp(component.second.name.c_str(), "GeometryPtr");
					}
				);

				if (positionAttribute != componentList.end() &&
					geometryAttribute != componentList.end())
				{
					assert(World::WorldParser::GetCustomComponents().contains(geometryAttribute->second.desc));

					entity.set(EntitySystem::LevelEditorECS::PositionDesc{ levelObject.GetLevelObjectId(), positionAttribute->second.id});

					// Can be set to 0 since it doesn't matter now, will be updated by the system
					entity.set(EntitySystem::EditorECS::Position{ 0.0f, 0.0f, 0.0f });
					entity.set(GeometryPtr{
						reinterpret_cast<RenderCore::Geometry*>(
							World::WorldParser::GetCustomComponents()[geometryAttribute->second.desc]
							)
						});
				}
			}
			
			m_world = &world;
			EntitySystem::LevelEditorECS::RegisterLevelEditorEcsSystems(world, *m_Level);
		}

		void LevelEditor::Draw()
		{
			ImGui::Begin(GetName());

			if (m_Level.has_value()) [[likely]]
			{
				for (World::LevelObject& levelObject : m_Level->GetLevelObjects())
				{
					if (ImGui::TreeNode(levelObject.GetName().c_str()))
					{
						for (auto& component : levelObject.GetComponents())
						{
							if (component.second.name == "Position") {
								std::string token;
								std::stringstream ss(component.second.desc);
								float pos[3];
								int i = 0;
								while (std::getline(ss, token, ',')) {
									pos[i] = std::stof(token);
									++i;
								}
								ImGui::InputFloat3(component.second.name.c_str(), pos);
								component.second.desc = std::to_string(pos[0]) + ',' + std::to_string(pos[1]) + ',' + std::to_string(pos[2]);
								continue;
							}
							ImGui::InputText(component.second.name.c_str(), &component.second.desc);
						}

						ImGui::TreePop();
					}
				}
			}

			if (ImGui::Button("Save"))
			{
				m_SaveButtonMessageTimer.Reset();
				m_SaveButtonPressed = true;

				Save();
			}

			if (m_SaveButtonPressed)
			{
				ImGui::SameLine();
				ImGui::Text("Saved!");
			}

			if (ImGui::Button("Add Object"))
			{
				World::LevelObject newLevelObject;
				newLevelObject.SetName((std::string("New Object") + std::to_string(objNum++)).c_str());

				newLevelObject.AddComponent("Position", "0.0f,0.0f,0.0f");
				newLevelObject.AddComponent("GeometryPtr", "Cube");


				m_Level->AddLevelObject(newLevelObject);

				flecs::entity entity = m_world->entity(newLevelObject.GetName().c_str());

				World::LevelObject::ComponentList& componentList = newLevelObject.GetComponents();

				World::LevelObject::ComponentList::iterator positionAttribute = std::ranges::find_if(componentList,
					[](auto& component)
					{
						return !std::strcmp(component.second.name.c_str(), "Position");
					}
				);

				World::LevelObject::ComponentList::iterator geometryAttribute = std::ranges::find_if(componentList,
					[](auto& component)
					{
						return !std::strcmp(component.second.name.c_str(), "GeometryPtr");
					}
				);

				if (positionAttribute != componentList.end() &&
					geometryAttribute != componentList.end())
				{
					assert(World::WorldParser::GetCustomComponents().contains(geometryAttribute->second.desc));

					entity.set(EntitySystem::LevelEditorECS::PositionDesc{ newLevelObject.GetLevelObjectId(), positionAttribute->second.id });

					// Can be set to 0 since it doesn't matter now, will be updated by the system
					entity.set(EntitySystem::EditorECS::Position{ 0.0f, 0.0f, 0.0f });
					entity.set(GeometryPtr{
						reinterpret_cast<RenderCore::Geometry*>(
							World::WorldParser::GetCustomComponents()[geometryAttribute->second.desc]
							)
						});
				}

			}

			ImGui::End();
		}

		void LevelEditor::Update(float dt)
		{
			m_SaveButtonMessageTimer.Tick();

			if (m_SaveButtonMessageTimer.GetTotalTime() > m_TimeToShowSaveButtonMessage)
			{
				m_SaveButtonPressed = false;
			}
		}

		void LevelEditor::Save()
		{
			assert(m_Level.has_value());
			LevelSerializer::Serialize(Core::g_FileSystem->GetFilePath("Levels/Main.xml").generic_string(), m_Level.value());
		}
	}
}