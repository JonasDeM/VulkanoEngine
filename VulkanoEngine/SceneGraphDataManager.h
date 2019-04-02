#pragma once
#include <vector>

template<typename DataType>
class SceneGraphDataManager
{
public:
	static int32_t CreateNew() { 
		static int32_t IndexCounter;
		Data.resize(IndexCounter+1);
		return IndexCounter++;
	}
	static DataType& GetData(int32_t index) { 
		assert(index < Data.size());
		return Data[index]; 
	}
private:
	static std::vector<DataType> Data;
};
template<typename DataType>
std::vector<DataType> SceneGraphDataManager<DataType>::Data;