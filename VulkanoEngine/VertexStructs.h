#pragma once
#include "stdafx.h"
#include <array>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
using namespace glm;

struct VertexBase
{
	VertexBase() :
		Position(vec3(0, 0, 0)),
		Color(vec4(.3f, .4f, .4f, 1.f)),
		Normal(vec3(0, 0, 0)),
		TexCoord(vec2(0, 0))
	{}

	vec3 Position;
	vec4 Color;
	vec3 Normal;
	vec2 TexCoord;
};

struct VertexPosColTex
{
	VertexPosColTex(vec3 pos, vec4 col, vec2 texCoord) :
		Position(pos),
		Color(col),
		TexCoord(texCoord)
	{}

	VertexPosColTex(VertexBase& vertex) :
		Position(vertex.Position),
		Color(vertex.Color),
		TexCoord(vertex.TexCoord)
	{}

	VertexPosColTex() :
		Position(0,0,0),
		Color(0, 0, 0, 0),
		TexCoord(0,0)
	{}

	vec3 Position;
	vec4 Color;
	vec2 TexCoord;

	static VkVertexInputBindingDescription GetBindingDescription() {
		VkVertexInputBindingDescription bindingDescription = {};
		bindingDescription.binding = 0;
		bindingDescription.stride = sizeof(VertexPosColTex);
		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		return bindingDescription;
	}


	static std::array<VkVertexInputAttributeDescription, 3> GetAttributeDescriptions() {
		std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions = {};

		attributeDescriptions[0].binding = 0;
		attributeDescriptions[0].location = 0;
		attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[0].offset = offsetof(VertexPosColTex, Position);

		attributeDescriptions[1].binding = 0;
		attributeDescriptions[1].location = 1;
		attributeDescriptions[1].format = VK_FORMAT_R32G32B32A32_SFLOAT;
		attributeDescriptions[1].offset = offsetof(VertexPosColTex, Color);

		attributeDescriptions[2].binding = 0;
		attributeDescriptions[2].location = 2;
		attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
		attributeDescriptions[2].offset = offsetof(VertexPosColTex, TexCoord);

		return attributeDescriptions;
	}
};

struct VertexPosCol
{
	VertexPosCol(vec3 pos, vec4 col) :
		Position(pos),
		Color(col)
	{}

	VertexPosCol(VertexBase& vertex) :
		Position(vertex.Position),
		Color(vertex.Color)
	{}

	vec3 Position;
	vec4 Color;


	static VkVertexInputBindingDescription GetBindingDescription() {
		VkVertexInputBindingDescription bindingDescription = {};
		bindingDescription.binding = 0;
		bindingDescription.stride = sizeof(VertexPosCol);
		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		return bindingDescription;
	}


	static std::array<VkVertexInputAttributeDescription, 2> GetAttributeDescriptions() {
		std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions = {};

		attributeDescriptions[0].binding = 0;
		attributeDescriptions[0].location = 0;
		attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[0].offset = offsetof(VertexPosCol, Position);

		attributeDescriptions[1].binding = 0;
		attributeDescriptions[1].location = 1;
		attributeDescriptions[1].format = VK_FORMAT_R32G32B32A32_SFLOAT;
		attributeDescriptions[1].offset = offsetof(VertexPosCol, Color);

		return attributeDescriptions;
	}
};

struct VertexPosColNorm
{
	VertexPosColNorm() {}
	VertexPosColNorm(vec3 pos, vec4 col, vec3 norm) :
		Position(pos),
		Color(col),
		Normal(norm)
	{}
	VertexPosColNorm(VertexBase& vertex) :
		Position(vertex.Position),
		Color(vertex.Color),
		Normal(vertex.Normal)
	{}

	vec3 Position;
	vec4 Color;
	vec3 Normal;


	static VkVertexInputBindingDescription GetBindingDescription() {
		VkVertexInputBindingDescription bindingDescription = {};
		bindingDescription.binding = 0;
		bindingDescription.stride = sizeof(VertexPosColNorm);
		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		return bindingDescription;
	}


	static std::array<VkVertexInputAttributeDescription, 3> GetAttributeDescriptions() {
		std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions = {};

		attributeDescriptions[0].binding = 0;
		attributeDescriptions[0].location = 0;
		attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[0].offset = offsetof(VertexPosColNorm, Position);

		attributeDescriptions[1].binding = 0;
		attributeDescriptions[1].location = 1;
		attributeDescriptions[1].format = VK_FORMAT_R32G32B32A32_SFLOAT;
		attributeDescriptions[1].offset = offsetof(VertexPosColNorm, Color);

		attributeDescriptions[2].binding = 0;
		attributeDescriptions[2].location = 2;
		attributeDescriptions[2].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[2].offset = offsetof(VertexPosColNorm, Normal);

		return attributeDescriptions;
	}
};

struct VertexPosColNormTex
{
	VertexPosColNormTex(VertexBase& vertex) :
		Position(vertex.Position),
		Color(vertex.Color),
		Normal(vertex.Normal),
		TexCoord(vertex.TexCoord)
	{}

	vec3 Position;
	vec4 Color;
	vec3 Normal;
	vec2 TexCoord;


	static VkVertexInputBindingDescription GetBindingDescription() {
		VkVertexInputBindingDescription bindingDescription = {};
		bindingDescription.binding = 0;
		bindingDescription.stride = sizeof(VertexPosColNormTex);
		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		return bindingDescription;
	}


	static std::array<VkVertexInputAttributeDescription, 4> GetAttributeDescriptions() {
		std::array<VkVertexInputAttributeDescription, 4> attributeDescriptions = {};

		attributeDescriptions[0].binding = 0;
		attributeDescriptions[0].location = 0;
		attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[0].offset = offsetof(VertexPosColNormTex, Position);

		attributeDescriptions[1].binding = 0;
		attributeDescriptions[1].location = 1;
		attributeDescriptions[1].format = VK_FORMAT_R32G32B32A32_SFLOAT;
		attributeDescriptions[1].offset = offsetof(VertexPosColNormTex, Color);

		attributeDescriptions[2].binding = 0;
		attributeDescriptions[2].location = 2;
		attributeDescriptions[2].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[2].offset = offsetof(VertexPosColNormTex, Normal);

		attributeDescriptions[3].binding = 0;
		attributeDescriptions[3].location = 3;
		attributeDescriptions[3].format = VK_FORMAT_R32G32_SFLOAT;
		attributeDescriptions[3].offset = offsetof(VertexPosColNormTex, TexCoord);

		return attributeDescriptions;
	}
};

struct VertexPosNormTex
{
	VertexPosNormTex(VertexBase& vertex) :
		Position(vertex.Position),
		Normal(vertex.Normal),
		TexCoord(vertex.TexCoord)
	{}

	vec3 Position;
	vec3 Normal;
	vec2 TexCoord;

	static VkVertexInputBindingDescription GetBindingDescription() {
		VkVertexInputBindingDescription bindingDescription = {};
		bindingDescription.binding = 0;
		bindingDescription.stride = sizeof(VertexPosNormTex);
		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		return bindingDescription;
	}

	static std::array<VkVertexInputAttributeDescription, 3> GetAttributeDescriptions() {
		std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions = {};

		attributeDescriptions[0].binding = 0;
		attributeDescriptions[0].location = 0;
		attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[0].offset = offsetof(VertexPosNormTex, Position);
		
		attributeDescriptions[1].binding = 0;
		attributeDescriptions[1].location = 1;
		attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[1].offset = offsetof(VertexPosNormTex, Normal);

		attributeDescriptions[2].binding = 0;
		attributeDescriptions[2].location = 2;
		attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
		attributeDescriptions[2].offset = offsetof(VertexPosNormTex, TexCoord);

		return attributeDescriptions;
	}
};
