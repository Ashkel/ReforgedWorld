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
	StepTimer();

	/**
	 * @brief Ticks the timer, computing elapsed time and calling the update function.
	 *
	 * @param update User-supplied update function to run each frame or fixed step.
	 */
	void Tick(const std::function<void()>& update);

	/**
	 * @brief Resets the elapsed time.
	 *
	 * Call this if your app experiences a large intentional pause
	 * (e.g., debugging, blocking I/O) to prevent catch-up spikes.
	 */
	void ResetElapsedTime();

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
