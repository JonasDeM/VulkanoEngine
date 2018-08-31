#pragma once
#include <vector>
#include <memory>
#include <unordered_map>
#include <limits>
#include "Component.h"
class VulkanContext;
class GameScene;

class CompObj final
{
public:
	enum Flags : uint8_t {
		Invisible = 1 << 0, // doesn't need to be drawn
		Static = 1 << 1, // doesn't move
		//Resilient = 1 << 2, // needs to be moved to new scene
		// ...
	};
public:
	CompObj(Flags flags=(Flags)0);
	~CompObj() = default; //Non-virtal destructor, final class
	CompObj(const CompObj& t) = delete;
	CompObj& operator=(const CompObj& t) = delete;

	// Component
	// *********
	template <class T>
	void AddComponent(T* pComponent);
	template <class T>
	T* GetComponent();

	// Child-Parent
	// ************
	// Transfers ownership to this
	void AttachChild(CompObj* pObject);
	// Object is deleted
	void DeleteChild(CompObj* pObject);
	// Moves ownership to scene
	void DetachChild(CompObj* pObject);
	// Moves ownership to newParent
	void DetachChild(CompObj* pObject, CompObj* newParent);

	GameScene* GetScene() const { return m_pScene; };
	CompObj* GetParent() const { return m_pParent; }
	Flags GetFlags() const { return m_Flags; }
	// if setflags gets implemented, update draw if invisible is set to true from false

protected:
	friend class GameScene;
	void Build(VulkanContext* vkContext);
	void SceneStart();
	void Update(VulkanContext* vkContext);
	void PostUpdate();
	void RecordVulkanDrawCommands(VkCommandBuffer cmdBuffer, const int frameBufferIndex);
	void SceneStop();

	GameScene* m_pScene = nullptr;
	CompObj* m_pParent = nullptr;
	std::vector<std::unique_ptr<CompObj>> m_ChildrenPtrs;
	Flags m_Flags;

	// Custom RTTI
	// ***********
	using id_type = uint8_t;
	using index_type = id_type;
	template <typename T>
	struct RttiWrap
	{
		static id_type id;
	};
	static int generateId()
	{
		static int typeID; // automatically init with 0
		return ++typeID;
	}
private:
	// this way the order of execution(order in vector) is the order of adding the component
	std::unordered_map<id_type, index_type> m_RttiMap;
	std::vector<std::unique_ptr<Component>> m_ComponentPtrs;
};

template <typename T>
CompObj::id_type CompObj::RttiWrap<T>::id = CompObj::generateId();

template<class T>
void CompObj::AddComponent(T* pComponent)
{
	static_assert(std::is_base_of_v<Component, T>, "Must derive from Component");
	index_type i = (index_type)m_ComponentPtrs.size();
	if (m_RttiMap.find(RttiWrap<T>::id) != m_RttiMap.end())
	{
		Debug::LogError(L"Component of same type already added.");
		return;
	}
	m_RttiMap[RttiWrap<T>::id] = i;
	m_ComponentPtrs.push_back(std::unique_ptr<T>(pComponent));
	//after 1 emplace back, the size we got beforehand = index for last added item
	m_ComponentPtrs[i]->m_pCompObj = this;
	return;
}

template<class T>
T * CompObj::GetComponent()
{
	static_assert(std::is_base_of_v<Component, T>, "Must derive from Component");
	if (m_RttiMap.find(RttiWrap<T>::id) == m_RttiMap.end())
		return nullptr;
	return static_cast<T *>(m_ComponentPtrs[m_RttiMap[RttiWrap<T>::id]].get());
}