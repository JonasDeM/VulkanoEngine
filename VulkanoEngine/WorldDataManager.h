#pragma once
#include <vector>

class DataManager
{
public:
	DataManager() = default;
	virtual ~DataManager() = default; // enables me to keep pointers to all WorldDataManagers in a container
	DataManager(const DataManager &obj) = delete;
	DataManager& operator=(const DataManager& obj) = delete;
};

// Manages all objects of type Datatype in a vector
template<typename HandleType>
class WorldDataManager : public DataManager
{
private:
	using DataType = typename HandleType::DataType;
public:
	DataType* CreateNew() {
		m_Data.resize(m_Data.size()+1);
		DataType* data = &m_Data[m_Data.size() - 1];
		data->Build();
		return data;
	}

	// Needs to be defined here or all hell breaks loose
	DataType* GetData(size_t index)
	{
		return &m_Data[index];
	}

	template<typename... Args>
	void UpdateAll(Args... as);

	//template<typename... Args>
	//void UpdateAllAsync(Args... as)

	//template<typename... Args>
	//void RecordVulkanDrawCommands(Args... as);

private:
	std::vector<DataType> m_Data;
};

template<typename DataType>
template<typename ...Args>
void WorldDataManager<DataType>::UpdateAll(Args ...as)
{
	for (DataType& data: m_Data)
	{
		data.Update();
	}
}

