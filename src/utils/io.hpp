#pragma once

#include <stdafx.hpp>

namespace utils::io 
{
	bool file_exists(const std::string& file);
	bool write_file(const std::string& file, const std::string& data, bool append = false);
	bool write_file(const std::string& file, const std::string& data, int size, const bool append);
	bool read_file(const std::string& file, std::string* data);
	bool delete_file(const std::string& file);
	std::string read_file(const std::string& file);
	size_t file_size(const std::string& file);
	bool create_directory(const std::string& directory);
	bool directory_exists(const std::string& directory);
	bool directory_is_empty(const std::string& directory);
	std::vector<std::string> list_files(const std::string& directory);
	bool read_file_into_buffer(std::string Path, std::basic_string<uint8_t>& Buffer);
}