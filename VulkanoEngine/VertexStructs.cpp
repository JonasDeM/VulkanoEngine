#pragma once
#include "stdafx.h"
#include "VertexStructs.h"

//VertexPosColTex
//***************
//1.binding 2.stride 3.inputrate
const VkVertexInputBindingDescription VertexPosColTex::InputBindingDescription{ 0, sizeof(VertexPosColTex), VK_VERTEX_INPUT_RATE_VERTEX };
const std::array<VkVertexInputAttributeDescription, 3> VertexPosColTex::AttributeDescriptions{
	//1.location 2.binding 3.format 4.offset
	VkVertexInputAttributeDescription{ 0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(VertexPosColTex, Position) },
	VkVertexInputAttributeDescription{ 1, 0, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(VertexPosColTex, Color) },
	VkVertexInputAttributeDescription{ 2, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(VertexPosColTex, TexCoord) }
};


//VertexPosCol
//************
//1.binding 2.stride 3.inputrate
const VkVertexInputBindingDescription VertexPosCol::InputBindingDescription{ 0, sizeof(VertexPosCol), VK_VERTEX_INPUT_RATE_VERTEX };
const std::array<VkVertexInputAttributeDescription, 2> VertexPosCol::AttributeDescriptions{
	//1.location 2.binding 3.format 4.offset
	VkVertexInputAttributeDescription{ 0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(VertexPosCol, Position) },
	VkVertexInputAttributeDescription{ 1, 0, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(VertexPosCol, Color) }
};

//VertexPosColNorm
//****************
//1.binding 2.stride 3.inputrate
const VkVertexInputBindingDescription VertexPosColNorm::InputBindingDescription{ 0, sizeof(VertexPosColNorm), VK_VERTEX_INPUT_RATE_VERTEX };
const std::array<VkVertexInputAttributeDescription, 3> VertexPosColNorm::AttributeDescriptions{
	//1.location 2.binding 3.format 4.offset
	VkVertexInputAttributeDescription{ 0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(VertexPosColNorm, Position)},
	VkVertexInputAttributeDescription{ 1, 0, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(VertexPosColNorm, Color)},
	VkVertexInputAttributeDescription{ 2, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(VertexPosColNorm, Normal)}
};

//VertexPosColNormTex
//*******************
//1.binding 2.stride 3.inputrate
const VkVertexInputBindingDescription VertexPosColNormTex::InputBindingDescription{ 0, sizeof(VertexPosColNormTex), VK_VERTEX_INPUT_RATE_VERTEX };
const std::array<VkVertexInputAttributeDescription, 4> VertexPosColNormTex::AttributeDescriptions{
	//1.location 2.binding 3.format 4.offset
	VkVertexInputAttributeDescription{ 0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(VertexPosColNormTex, Position) },
	VkVertexInputAttributeDescription{ 1, 0, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(VertexPosColNormTex, Color) },
	VkVertexInputAttributeDescription{ 2, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(VertexPosColNormTex, Normal) },
	VkVertexInputAttributeDescription{ 3, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(VertexPosColNormTex, TexCoord) }
};

//VertexPosNormTex
//****************
//1.binding 2.stride 3.inputrate
const VkVertexInputBindingDescription VertexPosNormTex::InputBindingDescription{ 0, sizeof(VertexPosNormTex), VK_VERTEX_INPUT_RATE_VERTEX };
const std::array<VkVertexInputAttributeDescription, 3> VertexPosNormTex::AttributeDescriptions{
	//1.location 2.binding 3.format 4.offset
	VkVertexInputAttributeDescription{ 0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(VertexPosNormTex, Position) },
	VkVertexInputAttributeDescription{ 1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(VertexPosNormTex, Normal) },
	VkVertexInputAttributeDescription{ 2, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(VertexPosNormTex, TexCoord) }
};
