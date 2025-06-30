#pragma once

#include <queue>
#include <chrono>

#include "MouseEvent.h"
#include <Core/SubsystemManager.hpp>

/**
 * @brief Mouse input event queue with state tracking and double-click detection.
 */
class MouseQueue : public Subsystem
{
public:
	MouseQueue();

	SUBSYSTEM(MouseQueue)

	bool EventBufferIsEmpty() const;
	MouseEvent ReadEvent();

	Point GetPosition() const;
	int GetX() const;
	int GetY() const;

	bool IsLeftDown() const;
	bool IsRightDown() const;
	bool IsMiddleDown() const;
	bool IsX1Down() const;
	bool IsX2Down() const;

	bool IsDoubleClick() const;
	MouseEvent::EventType GetDoubleClickButton() const;

	void OnMouseMove(int x, int y);
	void OnMouseMoveRaw(int x, int y);
	void OnWheelDown(int x, int y);
	void OnWheelUp(int x, int y);

	void OnLeftPressed(int x, int y);
	void OnLeftReleased(int x, int y);
	void OnRightPressed(int x, int y);
	void OnRightReleased(int x, int y);
	void OnMiddlePressed(int x, int y);
	void OnMiddleReleased(int x, int y);
	void OnX1Pressed(int x, int y);
	void OnX1Released(int x, int y);
	void OnX2Pressed(int x, int y);
	void OnX2Released(int x, int y);

	void Clear();

private:
	void UpdateDoubleClick(MouseEvent::EventType pressType);

private:
	std::queue<MouseEvent> m_EventBuffer;

	int m_X = 0;
	int m_Y = 0;
	bool m_LeftIsDown = false;
	bool m_RightIsDown = false;
	bool m_MiddleIsDown = false;
	bool m_X1IsDown = false;
	bool m_X2IsDown = false;

	bool m_IsDoubleClick = false;
	MouseEvent::EventType m_etDoubleClick = MouseEvent::Invalid;
	MouseEvent::EventType m_LastClickButton = MouseEvent::Invalid;

	std::chrono::steady_clock::time_point m_LastClickTime;
	const std::chrono::milliseconds m_DoubleClickThreshold = std::chrono::milliseconds(400);
};

