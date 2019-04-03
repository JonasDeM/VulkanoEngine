#pragma once
#include <vector>

using SceneGraphDataIndex = uint32_t;

// Manages all objects of type Datatype in a static vector
template<typename DataType>
class SceneGraphDataManager
{
public:
	static SceneGraphDataIndex CreateNew() {
		static SceneGraphDataIndex IndexCounter = 1;
		Data.resize(IndexCounter);
		return IndexCounter++;
	}
	static DataType& GetData(SceneGraphDataIndex index) {
		assert(index < Data.size());
		assert(index > 0);
		return Data[index-1]; 
	}
private:
	static std::vector<DataType> Data;
};
template<typename DataType>
std::vector<DataType> SceneGraphDataManager<DataType>::Data;