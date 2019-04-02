#include "stdafx.h"
#include "Transform.h"

void Transform::SetPosition(const glm::vec3 & pos)
{
	GetData().Position = pos;
}

void Transform::GetPosition(glm::vec3 & pos)
{
	pos = GetData().Position;
}