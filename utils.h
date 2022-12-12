#pragma once
#include "inc.h"

namespace utils{
	extern int _char_designed(char c);
	extern bool str_match(char* s1, char* s2);
	extern std::string load_file_utf8(const char* path);

	enum result {
		Ok,
		Error
	};
}
