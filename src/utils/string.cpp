#include <stdafx.hpp>

namespace utils::string
{
	const char* va(const char* fmt, ...)
	{
		static thread_local va_provider<8, 256> provider;

		va_list ap;
		va_start(ap, fmt);

		const char* result = provider.get(fmt, ap);

		va_end(ap);
		return result;
	}

	std::string to_lower(std::string text)
	{
		std::transform(text.begin(), text.end(), text.begin(), [](const char input)
		{
			return CHAR(tolower(input));
		});

		return text;
	}

	std::string to_upper(std::string text)
	{
		std::transform(text.begin(), text.end(), text.begin(), [](const char input)
		{
			return CHAR(toupper(input));
		});

		return text;
	}

	std::string dump_hex(const std::string& data, const std::string& separator)
	{
		std::string result;

		for (unsigned int i = 0; i < data.size(); ++i)
		{
			if (i > 0)
			{
				result.append(separator);
			}

			result.append(va("%02X", data[i] & 0xFF));
		}

		return result;
	}

#pragma warning(push)
#pragma warning(disable: 4100)
	std::string convert(const std::wstring& wstr)
	{
		std::string result;
		result.reserve(wstr.size());

		for (const auto& chr : wstr)
		{
			result.push_back(static_cast<char>(chr));
		}

		return result;
	}

	std::wstring convert(const std::string& str)
	{
		std::wstring result;
		result.reserve(str.size());

		for (const auto& chr : str)
		{
			result.push_back(static_cast<wchar_t>(chr));
		}

		return result;
	}
#pragma warning(pop)

	bool starts_with(const std::string& input, const std::string& needle)
	{
		return (input.size() >= needle.size() && input.substr(0, needle.size()) == needle);
	}

	bool ends_with(const std::string& input, const std::string& needle)
	{
		return (input.size() >= needle.size() && input.substr(input.size() - needle.size(), needle.size()) == needle);
	}

	std::vector<std::string> split(const std::string& s, const char delim)
	{
		std::stringstream ss_(s);
		std::string item;
		std::vector<std::string> elems;

		while (std::getline(ss_, item, delim))
		{
			elems.push_back(item); // elems.push_back(std::move(item)); // if C++11 (based on comment from @mchiasson)
		}

		return elems;
	}

	std::wstring to_wstring(const std::string& string)
	{
		return std::wstring(string.begin(), string.end());
	}

	std::string to_string(const std::wstring& string)
	{
		return std::string(string.begin(), string.end());
	}

	std::string trim(const std::string& str)
	{
		if (str.empty())
		{
			return "";
		}

		auto s = str;

		s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch_) 
		{
			return !std::isspace(ch_);
		}));

		return s;
	}

	bool compare(const std::string& input, const std::string& text, const bool exact)
	{
		if (exact && text == input) return true;
		if (!exact && text.find(input) != std::string::npos) return true;
		return false;
	}

	std::string replace(std::string str, const std::string& from, const std::string& to)
	{
		if (from.empty())
		{
			return str;
		}

		size_t start_pos = 0;
		while ((start_pos = str.find(from, start_pos)) != std::string::npos)
		{
			str.replace(start_pos, from.length(), to);
			start_pos += to.length();
		}

		return str;
	}

	bool is_numeric(const std::string_view& str)
	{
		auto it = str.begin();
		while (it != str.end() && std::isdigit(static_cast<unsigned char>(*it)))
			++it;
		return !str.empty() && it == str.end();
	}

	std::string replace_all(std::string string, std::string find, std::string replace)
	{
		size_t pos = 0;
		while ((pos = string.find(find, pos)) != std::string::npos)
		{
			string = string.replace(pos, find.size(), replace);
			pos += replace.length();
		}

		return string;
	}
}
