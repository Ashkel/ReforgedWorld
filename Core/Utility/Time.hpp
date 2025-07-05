#pragma once

#include <chrono>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <string>
#include <thread>
#include <stdexcept>

/// @brief
/// 
/// Static time utility class for:
///  * Current local or UTC timestamp with optional format.
///  * ISO8601 string.
///  * Epoch ms.
///  * String → time_point parsing.
///  * Sleep helpers.
///  * Stopwatch for measuring elapsed time.
///  * Duration formatting helpers (H:M:S, ms, us, ns).
///  * Date difference (days, hours, seconds).
///  * Local timezone UTC offset in seconds.
///
/// Cross-platform. C++17 only. No dependencies.
///
class Time
{
public:
	/// @brief Get current local timestamp.
	/// Default: "YYYY-MM-DD HH:MM:SS.mmm"
	/// @param format strftime-compatible format string.
	static std::string CurrentTimestamp(const std::string& format = "%Y-%m-%d %H:%M:%S")
	{
		return FormatTimestamp(std::chrono::system_clock::now(), format, /*utc=*/false);
	}
	
	/// @brief Get current timestamp for file names.
	/// Default: "YYYYMMDD_HH-MM-SS"
	/// @param format strftime-compatible format string.
	static std::string CurrentTimestampFile(const std::string& format = "%Y%m%d_%H-%M-%S", bool utc = false)
	{
		return FormatTimestamp(std::chrono::system_clock::now(), format, utc);
	}

	/// @brief Get current UTC timestamp.
	/// Default: "YYYY-MM-DD HH:MM:SS.mmm"
	/// @param format strftime-compatible format string.
	static std::string CurrentTimestampUTC(const std::string& format = "%Y-%m-%d %H:%M:%S")
	{
		return FormatTimestamp(std::chrono::system_clock::now(), format, /*utc=*/true);
	}

	/// @brief Get current timestamp in ISO 8601 format.
	/// Format: "YYYY-MM-DDTHH:MM:SSZ"
	static std::string CurrentISO8601()
	{
		using namespace std::chrono;

		const auto now = system_clock::now();
		const auto now_time_t = system_clock::to_time_t(now);

		std::tm utc_tm{};
#if defined(_WIN32) || defined(_WIN64)
		gmtime_s(&utc_tm, &now_time_t);
#else
		gmtime_r(&now_time_t, &utc_tm);
#endif

		std::ostringstream oss;
		oss << std::put_time(&utc_tm, "%Y-%m-%dT%H:%M:%SZ");
		return oss.str();
	}

	/// @brief Get milliseconds since Unix epoch.
	static int64_t CurrentEpochMillis()
	{
		using namespace std::chrono;
		return duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
	}

	/// @brief Sleep for given milliseconds.
	static void SleepForMilliseconds(uint64_t ms)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(ms));
	}

	/// @brief Sleep for given seconds.
	static void SleepForSeconds(uint64_t s)
	{
		std::this_thread::sleep_for(std::chrono::seconds(s));
	}

	/// @brief Stopwatch: simple high-resolution timer.
	class Stopwatch
	{
	public:
		Stopwatch() { Restart(); }

		/// @brief Restart the stopwatch.
		void Restart() { m_Start = std::chrono::high_resolution_clock::now(); }

		/// @brief Get elapsed seconds.
		double ElapsedSeconds() const
		{
			return std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - m_Start).count();
		}

		/// @brief Get elapsed milliseconds.
		int64_t ElapsedMilliseconds() const
		{
			return std::chrono::duration_cast<std::chrono::milliseconds>(
				std::chrono::high_resolution_clock::now() - m_Start)
				.count();
		}

		/// Get elapsed microseconds.
		int64_t ElapsedMicroseconds() const
		{
			return std::chrono::duration_cast<std::chrono::microseconds>(
				std::chrono::high_resolution_clock::now() - m_Start)
				.count();
		}

	private:
		std::chrono::high_resolution_clock::time_point m_Start;
	};

	/// @brief Parse a timestamp string → time_point.
	/// @param str Date/time string.
	/// @param format strftime-compatible format.
	/// @param utc Parse as UTC if true, otherwise local.
	static std::chrono::system_clock::time_point ParseTimestamp(const std::string& str, const std::string& format = "%Y-%m-%d %H:%M:%S", bool utc = false)
	{
		std::tm tm = {};
		std::istringstream ss(str);
		ss >> std::get_time(&tm, format.c_str());
		if(ss.fail()) throw std::runtime_error("Time::ParseTimestamp() failed to parse: " + str);

		std::time_t time;
		if(utc)
		{
#if defined(_WIN32) || defined(_WIN64)
			time = _mkgmtime(&tm);
#else
			time = timegm(&tm);
#endif
		}
		else
		{
			time = std::mktime(&tm);
		}

		return std::chrono::system_clock::from_time_t(time);
	}

	/// @brief Format duration as "HH:MM:SS".
	static std::string FormatDurationHMS(std::chrono::seconds duration)
	{
		int64_t totalSeconds = duration.count();
		int hours = static_cast<int>(totalSeconds / 3600);
		int minutes = static_cast<int>((totalSeconds % 3600) / 60);
		int seconds = static_cast<int>(totalSeconds % 60);

		std::ostringstream oss;
		oss << std::setw(2) << std::setfill('0') << hours << ":"
			<< std::setw(2) << std::setfill('0') << minutes << ":"
			<< std::setw(2) << std::setfill('0') << seconds;
		return oss.str();
	}

	/// @brief Convert duration to ms.
	template<typename DurationT>
	static int64_t DurationMilliseconds(DurationT duration)
	{
		return std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
	}

	/// @brief Convert duration to microseconds.
	template<typename DurationT>
	static int64_t DurationMicroseconds(DurationT duration)
	{
		return std::chrono::duration_cast<std::chrono::microseconds>(duration).count();
	}

	/// @brief Convert duration to nanoseconds.
	template<typename DurationT>
	static int64_t DurationNanoseconds(DurationT duration)
	{
		return std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count();
	}

	/// @brief Date difference in days.
	static int64_t DateDifferenceDays(const std::chrono::system_clock::time_point& a, const std::chrono::system_clock::time_point& b)
	{
		using namespace std::chrono;
		return duration_cast<hours>(a - b).count() / 24;
	}

	/// @brief Date difference in hours.
	static int64_t DateDifferenceHours(const std::chrono::system_clock::time_point& a, const std::chrono::system_clock::time_point& b)
	{
		using namespace std::chrono;
		return duration_cast<hours>(a - b).count();
	}

	/// @brief Date difference in seconds.
	static int64_t DateDifferenceSeconds(const std::chrono::system_clock::time_point& a, const std::chrono::system_clock::time_point& b)
	{
		using namespace std::chrono;
		return duration_cast<seconds>(a - b).count();
	}

	/// @brief Get local UTC offset in seconds.
	static int GetLocalUTCOffsetSeconds()
	{
		std::time_t now = std::time(nullptr);

		std::tm local_tm{};
		std::tm utc_tm{};
#if defined(_WIN32) || defined(_WIN64)
		localtime_s(&local_tm, &now);
		gmtime_s(&utc_tm, &now);
#else
		localtime_r(&now, &local_tm);
		gmtime_r(&now, &utc_tm);
#endif

		int local = local_tm.tm_hour * 3600 + local_tm.tm_min * 60 + local_tm.tm_sec;
		int utc = utc_tm.tm_hour * 3600 + utc_tm.tm_min * 60 + utc_tm.tm_sec;
		int offset = local - utc;

		if(local_tm.tm_yday != utc_tm.tm_yday)
		{
			offset += (local_tm.tm_yday > utc_tm.tm_yday) ? 86400 : -86400;
		}

		return offset;
	}

private:
	/// @brief Internal timestamp formatter.
	static std::string FormatTimestamp(std::chrono::system_clock::time_point tp, const std::string& format, bool utc)
	{
		using namespace std::chrono;

		const auto now_time_t = system_clock::to_time_t(tp);

		std::tm tm{};
		if(utc)
		{
#if defined(_WIN32) || defined(_WIN64)
			gmtime_s(&tm, &now_time_t);
#else
			gmtime_r(&now_time_t, &tm);
#endif
		}
		else
		{
#if defined(_WIN32) || defined(_WIN64)
			localtime_s(&tm, &now_time_t);
#else
			localtime_r(&now_time_t, &tm);
#endif
		}

		std::ostringstream oss;
		oss << std::put_time(&tm, format.c_str());

		if(format == "%Y-%m-%d %H:%M:%S")
		{
			auto ms = duration_cast<milliseconds>(tp.time_since_epoch()) % 1000;
			oss << '.' << std::setw(3) << std::setfill('0') << ms.count();
		}

		return oss.str();
	}
};