#include "StepTimer.h"

StepTimer::StepTimer()
	: m_elapsed(Duration::zero()),
	m_total(Duration::zero()),
	m_leftOver(Duration::zero()),
	m_targetElapsed(std::chrono::nanoseconds(16'666'667)), // ~60fps
	m_fixedTimeStep(false),
	m_frameCount(0),
	m_framesThisSecond(0),
	m_framesPerSecond(0),
	m_lastTime(Clock::now()),
	m_fpsCounterStart(m_lastTime)
{
}

void StepTimer::Tick(const std::function<void()>& update)
{
	std::scoped_lock lock(m_mutex);

	TimePoint current = Clock::now();
	Duration delta = std::chrono::duration_cast<Duration>(current - m_lastTime);
	m_lastTime = current;

	// Clamp if app paused or large delay (e.g. debugger)
	if(delta > m_maxDelta)
		delta = m_maxDelta;

	m_frameCount++;
	m_framesThisSecond++;

	if(m_fixedTimeStep)
	{
		m_leftOver += delta;

		while(m_leftOver >= m_targetElapsed)
		{
			m_elapsed.store(m_targetElapsed);
			m_total += m_targetElapsed;
			m_leftOver -= m_targetElapsed;

			update();
		}
	}
	else
	{
		m_elapsed.store(delta);
		m_total += delta;
		m_leftOver = Duration::zero();

		update();
	}

	// FPS update every second
	auto timeSinceFpsCounter = current - m_fpsCounterStart;
	if(timeSinceFpsCounter >= std::chrono::seconds(1))
	{
		m_framesPerSecond.store(m_framesThisSecond);
		m_framesThisSecond = 0;
		m_fpsCounterStart = current;
	}
}

void StepTimer::ResetElapsedTime()
{
	std::scoped_lock lock(m_mutex);
	m_lastTime = Clock::now();
	m_leftOver = Duration::zero();
	m_framesThisSecond = 0;
	m_framesPerSecond = 0;
	m_fpsCounterStart = m_lastTime;
}
