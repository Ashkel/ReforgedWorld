#pragma once

#include <string>
#include <unordered_map>
#include <memory>
#include <mutex>
#include <stdexcept>
#include <type_traits>

/**
 * @brief Base class for all subsystems.
 *
 * All custom subsystems must inherit from Subsystem and implement GetTypeCode().
 */
class Subsystem
{
public:
	Subsystem() = default;
	virtual ~Subsystem() = default;

	/**
	 * @brief Returns the unique type code for this subsystem instance.
	 * @return Unique type code.
	 */
	virtual unsigned int GetTypeCode() const = 0;
};

namespace Internal
{
	/**
	 * @brief Internal singleton that manages all subsystems.
	 * 
	 * Do not use directly — use GetSubsystem, RemoveSubsystem, or ClearAllSubsystems.
	 */
	class _Subsystem
	{
	public:
		_Subsystem(const _Subsystem&) = delete;
		_Subsystem operator=(const _Subsystem&) = delete;
		
		/**
		 * @brief Destructor. Destroys all stored subsystems on exit.
		 */
		~_Subsystem() = default;

		/**
		 * @brief Gets the singleton instance.
		 * @return Reference to the singleton.
		 */
		static _Subsystem& GetInstance()
		{
			static _Subsystem instance;

			return instance;
		}

	private:
		_Subsystem() = default;

	public:
		/**
		 * @brief Get or create a subsystem of type T.
		 * 
		 * If the subsystem does not exist yet, it will be constructed in place.
		 * 
		 * @tparam T Subsystem type. Must inherit from Subsystem.
		 * @tparam Args Constructor argument types.
		 * @param args Constructor arguments forwarded to T's constructor.
		 * @return Reference to the created or existing subsystem.
		 */
		template<typename T, typename... Args>
		T& Get(Args&&... args)
		{
			static_assert(std::is_base_of<Subsystem, T>::value, "T must derive from Subsystem");

			const unsigned int typeCode = T::GetStaticTypeCode();

			{
				std::lock_guard<std::mutex> lock(m_Mutex);
				auto it = m_Subsystems.find(typeCode);
				if(it != m_Subsystems.end())
				{
					return *static_cast<T*>(it->second.get());
				}

				auto subsystem = std::make_unique<T>(std::forward<Args>(args)...);
				T& ref = *subsystem;
				m_Subsystems[typeCode] = std::move(subsystem);
				return ref;
			}
		}

		/**
		 * @brief Remove a specific subsystem.
		 *
		 * @tparam T Subsystem type.
		 */
		template<typename T>
		void Remove()
		{
			static_assert(std::is_base_of<Subsystem, T>::value, "T must derive from Subsystem");

			std::lock_guard<std::mutex> lock(m_Mutex);
			const unsigned int typeCode = T::GetStaticTypeCode();
			m_Subsystems.erase(typeCode);
		}

		/**
		 * @brief Remove all stored subsystems.
		 */
		void Clear()
		{
			std::lock_guard<std::mutex> lock(m_Mutex);
			m_Subsystems.clear();
		}

	private:
		std::unordered_map<unsigned int, std::unique_ptr<Subsystem>> m_Subsystems; ///< Map of subsystems.
		std::mutex m_Mutex; ///< Guards access to m_Subsystems.
	};
}
/**
 * @brief Get or create a subsystem globally.
 *
 * Shortcut for Shared::Internal::_Subsystem::GetInstance().Get().
 *
 * @tparam T Subsystem type.
 * @tparam Args Constructor argument types.
 * @param args Constructor arguments.
 * @return Reference to created or existing subsystem.
 */
template<typename T, typename... Args>
inline T& GetSubsystem(Args&&... args)
{
	return Internal::_Subsystem::GetInstance().Get<T>(std::forward<Args>(args)...);
}

/**
 * @brief Remove a specific subsystem.
 *
 * @tparam T Subsystem type.
 */
template<typename T>
inline void RemoveSubsystem()
{
	Internal::_Subsystem::GetInstance().Remove<T>();
}

/**
 * @brief Remove all subsystems.
 */
inline void ClearAllSubsystems()
{
	Internal::_Subsystem::GetInstance().Clear();
}

/**
 * @brief Compile-time FNV-1a hash.
 *
 * Converts a string literal to a unique integer at compile time.
 *
 * @param str Null-terminated string.
 * @return Hash value.
 */
constexpr unsigned int Str2Int(const char* str)
{
	constexpr unsigned int fnv_offset = 2166136261u;
	constexpr unsigned int fnv_prime = 16777619u;

	unsigned int hash = fnv_offset;
	while(*str)
	{
		hash ^= static_cast<unsigned int>(*str++);
		hash *= fnv_prime;
	}

	return hash;
}

/**
 * @brief Macro to implement unique type ID methods for a subsystem.
 *
 * Usage: Add SUBSYSTEM(MyClass) inside your derived class.
 */
#define SUBSYSTEM(ClassName) \
	unsigned int GetTypeCode() const final override { return GetStaticTypeCode(); } \
	static constexpr unsigned int GetStaticTypeCode() { return Str2Int(#ClassName); }