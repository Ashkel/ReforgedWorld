#pragma once

class KeyboardEvent
{
public:

	enum EventType
	{
		Invalid,
		Press,
		Release
	};

public:

	KeyboardEvent();
	KeyboardEvent(EventType type, const unsigned char key);

	bool IsPress() const;
	bool IsRelease() const;
	bool IsValid() const;
	EventType GetType() const;
	unsigned char GetKeyCode() const;

private:
	EventType m_Type;
	unsigned char m_Key;
};

