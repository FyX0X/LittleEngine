#include "LittleEngine/Storage/storage_helper.h"
#include "LittleEngine/internal.h"
#include "LittleEngine/error_logger.h"
#include <filesystem>
#include <sstream>

namespace LittleEngine::Storage
{
	//bool FileExists(const std::filesystem::path& path)
	//{
	//	return std::filesystem::exists(path);
	//}

	void EnsureDirectoryExists(const std::filesystem::path& path)
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

	bool WriteTextFile(const std::filesystem::path& path, const std::string& text)
	{
		std::ofstream out(path);
		if (!out.is_open())
			return false;
	
		out << text;
		return out.good();
	}

	bool WriteBinaryFile(const std::filesystem::path& path, const std::vector<char>& data)
	{
		std::ofstream out(path, std::ios::binary);
		if (!out.is_open())
			return false;

		out.write(data.data(), static_cast<std::streamsize>(data.size()));
		return out.good();
	}

	bool ReadTextFile(const std::filesystem::path& path, std::string* target)
	{
		std::ifstream in(path);
		if (!in.is_open())
		{	
			target = nullptr;
			return false;
		}
			

		*target = std::string(std::istreambuf_iterator<char>(in),
			std::istreambuf_iterator<char>());
		return in.good();
	}

	bool ReadBinaryFile(const std::filesystem::path& path, std::vector<char>* target)
	{
		std::ifstream in(path, std::ios::binary | std::ios::ate);		// open at end of file
		if (!in.is_open())
			target = nullptr;
			return false;


		std::streamsize size = in.tellg();		// get size of file
		in.seekg(0, std::ios::beg);				// go back to beginning

		target->reserve(size);
		if (!in.read(target->data(), size))
			return false;
		return true;


	}






}