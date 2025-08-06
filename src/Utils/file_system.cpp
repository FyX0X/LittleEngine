#include "LittleEngine/Utils/file_system.h"
#include "LittleEngine/internal.h"
#include "LittleEngine/Utils/logger.h"
#include <filesystem>
#include <sstream>
#include <fstream>

namespace LittleEngine::Utils::FileSystem
{

	bool FileExists(const std::filesystem::path& path)
	{
		return std::filesystem::exists(path) && std::filesystem::is_regular_file(path);
	}


	void CreateDirectories(const std::filesystem::path& path)
	{
		std::filesystem::create_directories(path);
	
	}

	std::string GetNextFreeFilepath(const std::string& folder, const std::string& prefix, const std::string& ext)
	{
		int index = 0;
		std::filesystem::path path;

		CreateDirectories(folder);


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

		Utils::Logger::Warning("Storage::GetNextFreeFilePath: too many iterations to find new available filename: overwriting: " + path.string());
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

	bool WriteBinaryFile(const std::filesystem::path& path, const std::vector<unsigned char>& data)
	{
		std::ofstream out(path, std::ios::binary);
		if (!out.is_open())
			return false;

		out.write(reinterpret_cast<const char*>(data.data()), static_cast<std::streamsize>(data.size()));
		return out.good();
	}

	bool ReadTextFile(const std::filesystem::path& path, std::string* target)
	{
		std::ifstream in(path);
		if (!in.is_open())
		{
			return false;
		}
			

		*target = std::string(std::istreambuf_iterator<char>(in),
			std::istreambuf_iterator<char>());
		return in.good();
	}

	bool ReadBinaryFile(const std::filesystem::path& path, std::vector<unsigned char>* target)
	{
		std::ifstream in(path, std::ios::binary | std::ios::ate);		// open at end of file
		if (!in.is_open())
		{
			return false;
		}


		std::streamsize size = in.tellg();		// get size of file
		in.seekg(0, std::ios::beg);				// go back to beginning

		target->resize(size);
		if (!in.read(reinterpret_cast<char*>(target->data()), size))
			return false;
		return true;


	}






}