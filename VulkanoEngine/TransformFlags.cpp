#include "stdafx.h"
#include "TransformFlags.h"

bool TransformFlags::HasFlags(TransformFlag flags)
{
	return m_Flags & flags;
}

void TransformFlags::SetFlags(TransformFlag flags)
{
	m_Flags = m_Flags | flags;
}

void TransformFlags::ClearFlags(TransformFlag flags)
{
	m_Flags = m_Flags & (~flags);
}
