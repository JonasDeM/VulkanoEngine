#pragma once
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

	const static VkVertexInputBindingDescription InputBindingDescription;
	const static std::array<VkVertexInputAttributeDescription, 3> AttributeDescriptions;
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

	const static VkVertexInputBindingDescription InputBindingDescription;
	const static std::array<VkVertexInputAttributeDescription, 2> AttributeDescriptions;
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

	const static VkVertexInputBindingDescription InputBindingDescription;
	const static std::array<VkVertexInputAttributeDescription, 3> AttributeDescriptions;
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

	const static VkVertexInputBindingDescription InputBindingDescription;
	const static std::array<VkVertexInputAttributeDescription, 4> AttributeDescriptions;

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

	const static VkVertexInputBindingDescription InputBindingDescription;
	const static std::array<VkVertexInputAttributeDescription, 3> AttributeDescriptions;
};
