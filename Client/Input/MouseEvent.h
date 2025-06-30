#pragma once

/**
 * @brief Point helper struct.
 */
struct Point
{
	int X;
	int Y;

	Point(int x, int y) : X(x), Y(y) { }
};

/**
 * @brief Represents a single mouse event.
 */
class MouseEvent
{
public:
	enum EventType
	{
		Invalid,
		Move,
		RawMove,
		WheelUp,
		WheelDown,
		LeftPress,
		LeftRelease,
		RightPress,
		RightRelease,
		MiddlePress,
		MiddleRelease,
		X1Press,
		X1Release,
		X2Press,
		X2Release,
	};

public:
	MouseEvent() = default;
	MouseEvent(const EventType type, const int x, const int y);

	bool IsValid() const;
	EventType GetType() const;
	Point GetPosition() const;
	int GetX() const;
	int GetY() const;

private:
	EventType m_Type = EventType::Invalid;
	int m_X = 0;
	int m_Y = 0;
};
