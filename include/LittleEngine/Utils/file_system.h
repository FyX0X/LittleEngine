#pragma once

#include <iostream>
#include <filesystem>


namespace LittleEngine::Utils::FileSystem
{

	bool FileExists(const std::filesystem::path& path);
	void CreateDirectories(const std::filesystem::path& path);

	/**
	 * Returns the next available filename
	 * ex: 
	 * prefix = "test", ext = ".png"
	 * folder contains:
	 *		test0.png
	 *		test1.png
	 * 
	 * returns folder/test1.png
	 */
	std::string GetNextFreeFilepath(const std::string& folder, const std::string& prefix, const std::string& ext);


	bool WriteTextFile(const std::filesystem::path& path, const std::string& text);
	bool WriteBinaryFile(const std::filesystem::path& path, const std::vector<unsigned char>& data);
	bool ReadTextFile(const std::filesystem::path& path, std::string* target);
	bool ReadBinaryFile(const std::filesystem::path& path, std::vector<unsigned char>* target);
}