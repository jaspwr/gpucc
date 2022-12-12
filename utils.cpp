#include "utils.h"


namespace utils {

    int _char_designed(char c) {
        if (c < 0)
            return 127 - (int)c;
        return (int)c;
    }

    bool str_match(char* s1, char* s2) {
#define MAX_LEN 500
        int i;
        for (i = 0; i < MAX_LEN && (s1[i] != '\0'); i++)
        {
            if (s2[i] == '\0' || s1[i] != s2[i])
                return false;
        }
        return (s2[i] == '\0');
#undef MAX_LEN
    }

    std::string load_file_utf8(const char* path) {
        std::string content;
        std::ifstream fileStream(path, std::ios::in);

        if (!fileStream.is_open()) {
            std::cout << "Could not read file " << path << ". File does not exist." << std::endl;
            throw;
            return "";
        }

        std::string line = "";
        while (!fileStream.eof()) {
            std::getline(fileStream, line);
            content.append(line + "\n");
        }

        fileStream.close();
        return content;
    }
}