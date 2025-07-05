#pragma once

#include <chrono>
#include <atomic>
#include <mutex>
#include <functional>

/**
 * @brief Cross-platform high-resolution step timer for game loops and simulations.
 *
 * Provides both fixed timestep (e.g., physics) and variable timestep modes.
 * Tracks elapsed time, total time, frame count, and frames per second (FPS).
 * Thread-safe for use in multi-threaded game loops.
 */
class StepTimer
{
public:
	using Clock = std::chrono::high_resolution_clock;
	using TimePoint = Clock::time_point;
	using Duration = std::chrono::nanoseconds;

	/**
	 * @brief Constructs a new StepTimer with default settings.
	 *
	 * Default is variable timestep with no target step duration.
	 */
	StepTimer()
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

	/**
	 * @brief Ticks the timer, computing elapsed time and calling the update function.
	 *
	 * @param update User-supplied update function to run each frame or fixed step.
	 */
	void Tick(const std::function<void()>& update)
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

	/**
	 * @brief Resets the elapsed time.
	 *
	 * Call this if your app experiences a large intentional pause
	 * (e.g., debugging, blocking I/O) to prevent catch-up spikes.
	 */
	void ResetElapsedTime()
	{
		std::scoped_lock lock(m_mutex);
		m_lastTime = Clock::now();
		m_leftOver = Duration::zero();
		m_framesThisSecond = 0;
		m_framesPerSecond = 0;
		m_fpsCounterStart = m_lastTime;
	}

	/// @name Accessors
	/// @{

	/** @brief Elapsed time in nanoseconds since last Tick. */
	uint64_t GetElapsedNanoseconds() const noexcept { return m_elapsed.load().count(); }

	/** @brief Elapsed time in seconds since last Tick. */
	double GetElapsedSeconds() const noexcept { return std::chrono::duration<double>(m_elapsed.load()).count(); }

	/** @brief Total accumulated time in nanoseconds. */
	uint64_t GetTotalNanoseconds() const noexcept { return m_total.count(); }

	/** @brief Total accumulated time in seconds. */
	double GetTotalSeconds() const noexcept { return std::chrono::duration<double>(m_total).count(); }

	/** @brief Number of simulation frames run since start. */
	uint64_t GetFrameCount() const noexcept { return m_frameCount.load(); }

	/** @brief Last calculated frames per second. */
	uint32_t GetFramesPerSecond() const noexcept { return m_framesPerSecond.load(); }

	/// @}

	/// @name Configuration
	/// @{

	/** @brief Enable or disable fixed timestep mode. */
	void SetFixedTimeStep(bool isFixed) noexcept { m_fixedTimeStep = isFixed; }

	/** @brief Set target elapsed time for fixed timestep mode. */
	void SetTargetElapsed(Duration duration) noexcept { m_targetElapsed = duration; }

	/** @brief Set target elapsed time for fixed timestep mode in seconds. */
	void SetTargetElapsedSeconds(double seconds) noexcept
	{
		m_targetElapsed = std::chrono::duration_cast<Duration>(std::chrono::duration<double>(seconds));
	}

	/// @}

private:
	mutable std::mutex m_mutex;

	std::atomic<Duration> m_elapsed;
	Duration m_total;
	Duration m_leftOver;

	Duration m_targetElapsed;
	const Duration m_maxDelta = std::chrono::milliseconds(100); ///< Max allowed delta, clamped to 100ms

	std::atomic<uint64_t> m_frameCount;
	std::atomic<uint32_t> m_framesPerSecond;
	uint32_t m_framesThisSecond;

	TimePoint m_lastTime;
	TimePoint m_fpsCounterStart;

	bool m_fixedTimeStep;
};
