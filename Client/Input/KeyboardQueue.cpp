#include "pch.h"
#include "KeyboardQueue.h"

KeyboardQueue::KeyboardQueue()
{
	Clear();
}

void KeyboardQueue::EnableAutoRepeatKeys()
{
	m_AutoRepeatKeys = true;
}

void KeyboardQueue::DisableAutoRepeatKeys()
{
	m_AutoRepeatKeys = false;
}

bool KeyboardQueue::IsKeysAutoRepeat() const
{
	return m_AutoRepeatKeys;
}

void KeyboardQueue::EnableAutoRepeatChars()
{
	m_AutoRepeatChars = true;
}

void KeyboardQueue::DisableAutoRepeatChars()
{
	m_AutoRepeatChars = false;
}

bool KeyboardQueue::IsCharsAutoRepeat() const
{
	return m_AutoRepeatChars;
}

bool KeyboardQueue::KeyBufferIsEmpty() const
{
	return m_KeyBuffer.empty();
}

bool KeyboardQueue::CharBufferIsEmpty() const
{
	return m_CharBuffer.empty();
}

KeyboardEvent KeyboardQueue::ReadKey()
{
	if(m_KeyBuffer.empty())
		return KeyboardEvent();

	KeyboardEvent key = m_KeyBuffer.front();

	m_KeyBuffer.pop();

	return key;
}

unsigned char KeyboardQueue::ReadChar()
{
	if(m_CharBuffer.empty())
		return 0u;

	unsigned char ch = m_CharBuffer.front();

	m_CharBuffer.pop();

	return ch;
}

bool KeyboardQueue::IsKeyPressed(const unsigned char key) const
{
	return m_KeyStates[key];
}

void KeyboardQueue::OnKeyPressed(const unsigned char key)
{
	m_KeyStates[key] = true;
	m_KeyBuffer.push(KeyboardEvent(KeyboardEvent::Press, key));
}

void KeyboardQueue::OnKeyReleased(const unsigned char key)
{
	m_KeyStates[key] = false;
	m_KeyBuffer.push(KeyboardEvent(KeyboardEvent::Release, key));
}

void KeyboardQueue::OnChar(const unsigned char key)
{
	m_CharBuffer.push(key);
}

void KeyboardQueue::Clear()
{
	for(size_t i = 0; i < _countof(m_KeyStates); i++)
	{
		m_KeyStates[i] = false;
	}

	if(m_KeyBuffer.size() > 0)
	{
		for(size_t i = 0; i < m_KeyBuffer.size(); i++)
		{
			m_KeyBuffer.pop();
		}
	}

	if(m_CharBuffer.size() > 0)
	{
		for(size_t i = 0; i < m_CharBuffer.size(); i++)
		{
			m_CharBuffer.pop();
		}
	}
}
