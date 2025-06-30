#include "pch.h"
#include "KeyboardEvent.h"

KeyboardEvent::KeyboardEvent()
	: m_Type(EventType::Invalid), m_Key(0u)
{
}

KeyboardEvent::KeyboardEvent(EventType type, const unsigned char key)
	: m_Type(type), m_Key(key)
{
}

bool KeyboardEvent::IsPress() const
{
	return m_Type == KeyboardEvent::EventType::Press;
}

bool KeyboardEvent::IsRelease() const
{
	return m_Type == KeyboardEvent::EventType::Release;
}

bool KeyboardEvent::IsValid() const
{
	return m_Type != KeyboardEvent::EventType::Invalid;;
}

KeyboardEvent::EventType KeyboardEvent::GetType() const
{
	return m_Type;
}

unsigned char KeyboardEvent::GetKeyCode() const
{
	return m_Key;
}
