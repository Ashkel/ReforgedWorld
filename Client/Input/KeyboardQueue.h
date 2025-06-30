#pragma once

#include <queue>

#include "KeyboardEvent.h"
#include <Core/SubsystemManager.hpp>

class KeyboardQueue : public Subsystem
{
public:

	KeyboardQueue();

	SUBSYSTEM(KeyboardQueue)

	void EnableAutoRepeatKeys();
	void DisableAutoRepeatKeys();
	bool IsKeysAutoRepeat() const;

	void EnableAutoRepeatChars();
	void DisableAutoRepeatChars();
	bool IsCharsAutoRepeat() const;

	bool KeyBufferIsEmpty() const;
	bool CharBufferIsEmpty() const;

	KeyboardEvent ReadKey();
	unsigned char ReadChar();

	bool IsKeyPressed(const unsigned char key) const;
	void OnKeyPressed(const unsigned char key);
	void OnKeyReleased(const unsigned char key);
	void OnChar(const unsigned char key);

private:
	void Clear();

private:
	bool m_AutoRepeatKeys = false;
	bool m_AutoRepeatChars = false;
	bool m_KeyStates[256];

	std::queue<KeyboardEvent> m_KeyBuffer;
	std::queue<unsigned char> m_CharBuffer;
};

