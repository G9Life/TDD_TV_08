#ifndef GOLDEN_TEST_UTIL_H
#define GOLDEN_TEST_UTIL_H

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>

namespace golden {

inline std::string normalizeNewlines(std::string text) {
    std::string out;
    out.reserve(text.size());
    for (char c : text) {
        if (c == '\r') {
            continue;
        }
        out.push_back(c);
    }
    return out;
}

inline std::string readFile(const std::filesystem::path& path) {
    std::ifstream in(path, std::ios::binary);
    if (!in) {
        return {};
    }
    std::ostringstream buffer;
    buffer << in.rdbuf();
    return normalizeNewlines(buffer.str());
}

inline void writeFile(const std::filesystem::path& path, const std::string& content) {
    std::filesystem::create_directories(path.parent_path());
    std::ofstream out(path, std::ios::binary);
    out << content;
}

inline bool shouldUpdateGolden() {
    const char* env = std::getenv("TV_UPDATE_GOLDEN");
    return env != nullptr && env[0] != '\0' && env[0] != '0';
}

inline std::filesystem::path goldenPath(const std::filesystem::path& goldenDir,
                                        const std::string& scenarioName) {
    return goldenDir / (scenarioName + ".golden.txt");
}

}  // namespace golden

#endif  // GOLDEN_TEST_UTIL_H
