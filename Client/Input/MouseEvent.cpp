#include "pch.h"
#include "MouseEvent.h"

MouseEvent::MouseEvent(const EventType type, const int x, const int y)
	: m_Type(type), m_X(x), m_Y(y)
{
}

bool MouseEvent::IsValid() const
{
	return m_Type != EventType::Invalid;
}

MouseEvent::EventType MouseEvent::GetType() const
{
	return m_Type;
}

Point MouseEvent::GetPosition() const
{
	return Point(m_X, m_Y);
}

int MouseEvent::GetX() const
{
	return m_X;
}

int MouseEvent::GetY() const
{
	return m_Y;
}
