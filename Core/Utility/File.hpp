#pragma once

#include <filesystem>
#include <string>
#include <string_view>
#include <stdexcept>

/**
 * @brief Provides static helper methods for common filesystem operations.
 */
class File
{
public:
	/**
	 * @brief Creates a directory and any necessary parent directories.
	 * @param path The directory path to create.
	 * @return True if created or already exists.
	 */
	static bool CreateDirectory(std::string_view path)
	{
		std::filesystem::path p(path);
		if(!std::filesystem::exists(p))
			return std::filesystem::create_directories(p);
		return true;
	}

	/**
	 * @brief Gets the current working directory as a string.
	 */
	static std::string GetCurrentDirectory()
	{
		return std::filesystem::current_path().string();
	}

	/**
	 * @brief Combines two path segments.
	 */
	static std::string CombinePath(std::string_view root, std::string_view toAppend)
	{
		return (std::filesystem::path(root) / toAppend).string();
	}

	/**
	 * @brief Gets the relative path for a file path.
	 */
	static std::string GetRelativePath(std::string_view filePath)
	{
		return std::filesystem::path(filePath).relative_path().string();
	}

	/**
	 * @brief Gets the parent path of a file path.
	 */
	static std::string GetParentPath(std::string_view filePath)
	{
		return std::filesystem::path(filePath).parent_path().string();
	}

	/**
	 * @brief Gets the root directory (drive or root slash).
	 */
	static std::string GetRootPath(std::string_view filePath)
	{
		return std::filesystem::path(filePath).root_directory().string();
	}

	/**
	 * @brief Gets the file extension, including the dot.
	 */
	static std::string GetFileExtension(std::string_view filePath)
	{
		return std::filesystem::path(filePath).extension().string();
	}

	/**
	 * @brief Sets or replaces the file extension.
	 * @param filePath Original file path.
	 * @param extension New extension (with or without dot).
	 */
	static std::string SetFileExtension(std::string_view filePath, std::string_view extension)
	{
		std::filesystem::path p(filePath);
		std::string ext = extension.starts_with('.') ? std::string(extension) : "." + std::string(extension);
		return p.replace_extension(ext).string();
	}

	/**
	 * @brief Gets the file name. Optionally removes the extension.
	 */
	static std::string GetFileName(std::string_view filePath, bool withExtension = true)
	{
		std::filesystem::path p(filePath);
		return withExtension ? p.filename().string() : p.stem().string();
	}

	/**
	 * @brief Sets the file name, optionally keeping the extension.
	 */
	static std::string SetFileName(std::string_view filePath, std::string_view name, bool keepExtension = true)
	{
		std::filesystem::path p(filePath);
		auto ext = p.extension();
		p.replace_filename(name);
		if(keepExtension && !ext.empty())
			p.replace_extension(ext);
		return p.string();
	}

	/**
	 * @brief Gets the file size in bytes. Throws if not found.
	 */
	static uintmax_t GetFileSize(std::string_view filePath)
	{
		std::filesystem::path p(filePath);
		if(!std::filesystem::exists(p))
			throw std::runtime_error("File does not exist: " + p.string());
		return std::filesystem::file_size(p);
	}

	/**
	 * @brief Gets the number of mipmap levels for a texture file.
	 * Looks for files named with _mm1, _mm2, ...
	 */
	static unsigned int GetMipMapLevelCount(std::string_view textureFile, unsigned int maxLevels = 4)
	{
		unsigned int levels = 0;
		std::filesystem::path p(textureFile);
		auto base = p.stem().string();
		auto ext = p.extension().string();
		auto dir = p.parent_path();

		for(unsigned int i = 1; i <= maxLevels; ++i)
		{
			auto test = dir / (base + "_mm" + std::to_string(i) + ext);
			if(std::filesystem::exists(test))
				++levels;
		}
		return levels;
	}

	/**
	 * @brief Normalize a path to a cross-platform friendly format.
	 * - On Windows, converts '\' to '/'.
	 * - Removes redundant dots and slashes.
	 * - Makes the path absolute if requested.
	 */
	static std::string NormalizePath(std::string_view path, bool makeAbsolute = false)
	{
		std::filesystem::path p(path);

		if(makeAbsolute)
			p = std::filesystem::absolute(p);

		p = p.lexically_normal();

#ifdef _WIN32
		// Convert backslashes to forward slashes for config files or logs
		std::string s = p.string();
		std::replace(s.begin(), s.end(), '\\', '/');
		return s;
#else
		return p.string();
#endif
	}

	/**
	 * @brief Cross-platform path equality.
	 * On Windows, compares ignoring case.
	 */
	static bool ArePathsEqual(std::string_view a, std::string_view b)
	{
		std::filesystem::path pa(a);
		std::filesystem::path pb(b);

		pa = pa.lexically_normal();
		pb = pb.lexically_normal();

#ifdef _WIN32
		auto sa = pa.string();
		auto sb = pb.string();
		std::transform(sa.begin(), sa.end(), sa.begin(), ::tolower);
		std::transform(sb.begin(), sb.end(), sb.begin(), ::tolower);
		return sa == sb;
#else
		return pa == pb;
#endif
	}

};
