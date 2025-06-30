#include "pch.h"
#include "MouseQueue.h"

MouseQueue::MouseQueue()
	: m_EventBuffer(), m_X(0), m_Y(0), m_etDoubleClick(MouseEvent::Invalid)
{
	Clear();
}

bool MouseQueue::EventBufferIsEmpty() const
{
	return m_EventBuffer.empty();
}


Point MouseQueue::GetPosition() const
{
	return Point(m_X, m_Y);
}

int MouseQueue::GetX() const
{
	return m_X;
}

int MouseQueue::GetY() const
{
	return m_Y;
}

bool MouseQueue::IsLeftDown() const
{
	return m_LeftIsDown;
}

bool MouseQueue::IsRightDown() const
{
	return m_RightIsDown;
}

bool MouseQueue::IsMiddleDown() const
{
	return m_MiddleIsDown;
}

bool MouseQueue::IsX1Down() const
{
	return m_X1IsDown;
}

bool MouseQueue::IsX2Down() const
{
	return m_X2IsDown;
}

bool MouseQueue::IsDoubleClick() const
{
	return m_IsDoubleClick;
}

MouseEvent::EventType MouseQueue::GetDoubleClickButton() const
{
	return m_etDoubleClick;
}

MouseEvent MouseQueue::ReadEvent()
{
	if(m_EventBuffer.empty())
		return MouseEvent();

	MouseEvent e = m_EventBuffer.front();
	m_EventBuffer.pop();

	return e;
}

void MouseQueue::OnMouseMove(int x, int y)
{
	m_EventBuffer.push(MouseEvent(MouseEvent::Move, x, y));

	m_X = x;
	m_Y = y;
}

void MouseQueue::OnMouseMoveRaw(int x, int y)
{
	m_EventBuffer.push(MouseEvent(MouseEvent::RawMove, x, y));

	m_X = x;
	m_Y = y;
}

void MouseQueue::OnWheelDown(int x, int y)
{
	m_EventBuffer.push(MouseEvent(MouseEvent::WheelDown, x, y));
}

void MouseQueue::OnWheelUp(int x, int y)
{
	m_EventBuffer.push(MouseEvent(MouseEvent::WheelUp, x, y));
}

void MouseQueue::OnLeftPressed(int x, int y)
{
	m_LeftIsDown = true;

	UpdateDoubleClick(MouseEvent::LeftPress);

	m_EventBuffer.push(MouseEvent(MouseEvent::LeftPress, x, y));
}

void MouseQueue::OnLeftReleased(int x, int y)
{
	m_LeftIsDown = false;

	m_EventBuffer.push(MouseEvent(MouseEvent::LeftRelease, x, y));
}

void MouseQueue::OnRightPressed(int x, int y)
{
	m_RightIsDown = true;

	UpdateDoubleClick(MouseEvent::RightPress);

	m_EventBuffer.push(MouseEvent(MouseEvent::RightPress, x, y));
}

void MouseQueue::OnRightReleased(int x, int y)
{
	m_RightIsDown = false;

	m_EventBuffer.push(MouseEvent(MouseEvent::RightRelease, x, y));
}

void MouseQueue::OnMiddlePressed(int x, int y)
{
	m_MiddleIsDown = true;

	UpdateDoubleClick(MouseEvent::MiddlePress);

	m_EventBuffer.push(MouseEvent(MouseEvent::MiddlePress, x, y));
}

void MouseQueue::OnMiddleReleased(int x, int y)
{
	m_MiddleIsDown = false;

	m_EventBuffer.push(MouseEvent(MouseEvent::MiddleRelease, x, y));
}

void MouseQueue::OnX1Pressed(int x, int y)
{
	m_X1IsDown = true;

	UpdateDoubleClick(MouseEvent::X1Press);

	m_EventBuffer.push(MouseEvent(MouseEvent::X1Press, x, y));
}

void MouseQueue::OnX1Released(int x, int y)
{
	m_X1IsDown = false;

	m_EventBuffer.push(MouseEvent(MouseEvent::X1Release, x, y));
}

void MouseQueue::OnX2Pressed(int x, int y)
{
	m_X2IsDown = true;

	UpdateDoubleClick(MouseEvent::X2Press);

	m_EventBuffer.push(MouseEvent(MouseEvent::X2Press, x, y));
}

void MouseQueue::OnX2Released(int x, int y)
{
	m_X2IsDown = false;

	m_EventBuffer.push(MouseEvent(MouseEvent::X2Release, x, y));
}


void MouseQueue::Clear()
{
	std::queue<MouseEvent> empty;
	std::swap(m_EventBuffer, empty);

	m_LeftIsDown = m_RightIsDown = m_MiddleIsDown = m_X1IsDown = m_X2IsDown = false;
	m_IsDoubleClick = false;
	m_etDoubleClick = MouseEvent::Invalid;
	m_LastClickButton = MouseEvent::Invalid;
}

void MouseQueue::UpdateDoubleClick(MouseEvent::EventType pressType)
{
	auto now = std::chrono::steady_clock::now();
	m_IsDoubleClick = false;

	if(m_LastClickButton == pressType)
	{
		auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - m_LastClickTime);
		if(elapsed < m_DoubleClickThreshold)
		{
			m_IsDoubleClick = true;
			m_etDoubleClick = pressType;
		}
	}

	m_LastClickTime = now;
	m_LastClickButton = pressType;
}

