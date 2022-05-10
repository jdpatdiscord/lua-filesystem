#include "utilities.hpp"

std::wstring utf8_to_utf16(const std::string& utf8_string)
{
	std::string cached_locale = std::locale().name();

	setlocale(LC_ALL, "en_US.utf8");

	std::wstring result_string;
	const char* temp_str_ptr = utf8_string.c_str();
	const char* temp_str_end = utf8_string.c_str() + utf8_string.size();

	std::mbstate_t state = std::mbstate_t();

	int result;
	char16_t result_char;
	while (result = mbrtoc16(&result_char, temp_str_ptr, temp_str_end - temp_str_ptr + 1, &state))
	{
		if (result == -3) // a new character was generated but the pointer doesn't need to advance
		{
			result_string += result_char;
		}
		else if (result == -2) // the next characters are valid but incomplete, just silent fail
		{
			break;
		}
		else if (result == -1) // there was a definite error, just silent fail
		{
			break;
		}
		else if (result >= 0) // continue or stop if == 0
		{
			result_string += result_char;
			// result returns a count of successfully converted characters in this case:
			if (result == 0) [[unlikely]]
				break;
			temp_str_ptr += result;
		}
	}

	setlocale(LC_ALL, cached_locale.c_str());

	return result_string;
}

std::string utf16_to_utf8(const std::wstring& utf16_string)
{
	std::string cached_locale = std::locale().name();
	setlocale(LC_ALL, "en_US.utf8");

	std::string result_string;
	std::mbstate_t state = std::mbstate_t();

	char ccache[MB_LEN_MAX] = { 0 };
	for (char16_t u16c : utf16_string)
	{
		auto result = c16rtomb(ccache, u16c, &state);
		if (result == -1) [[unlikely]]
			break;
		else
			result_string += ccache;
	}

	setlocale(LC_ALL, cached_locale.c_str());
	return result_string;
}

