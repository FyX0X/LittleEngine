#include "LittleEngine/Storage/storage_helper.h"
#include "LittleEngine/internal.h"
#include "LittleEngine/error_logger.h"
#include <filesystem>
#include <sstream>

namespace LittleEngine::Storage
{


	void EnsureDirectoryExists(const std::string& path)
	{
		// check is screenshot folder exists
		if (!std::filesystem::exists("screenshots"))
		{
			std::filesystem::create_directory("screenshots");
		}
	}

	std::string GetNextFreeFilepath(const std::string& folder, const std::string& prefix, const std::string& ext)
	{
		int index = 0;
		std::filesystem::path path;

		EnsureDirectoryExists(folder);


		while (index < defaults::maxFileCount)
		{
			std::ostringstream ss;
			ss << prefix << index << ext;
			path = folder + "/" + ss.str();

			if (!std::filesystem::exists(path))
				return path.string();
			
			index++;

		}
		std::ostringstream ss;
		ss << prefix << 0 << ext;
		path = folder + "/" + ss.str();

		LogWarning("Storage::GetNextFreeFilePath: too many iterations to find new available filename: overwriting: " + path.string());
		return path.string();
	}



}