#include <cstdint>
#include <cstdlib>
#include <cstring>

#include <string>
#include <fstream>
#include <filesystem>
#include <locale>
#include <codecvt>
#include <iomanip>
#include <clocale>
#include <cstring>
#include <cwchar>
#include <cuchar>
#include <unordered_map>
#include <map>

//#define LFS_STATEMENT_TRY(statement, variable) try { statement; }

#if defined(_WIN32)
#include <Windows.h>
#endif

std::wstring utf8_to_utf16(const std::string& utf8_string);
std::string utf16_to_utf8(const std::wstring& utf16_string);