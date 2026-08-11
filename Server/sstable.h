#include <fstream>
#include <map>
#include <string>

class SSTable {
private:
    std::string filename;

public:
    explicit SSTable(const std::string& filename)
        : filename(filename) {}

    void write(const std::map<std::string, std::string>& data) {
        std::ofstream file(filename);

        for (const auto& [key, value] : data) {
            file << key << '\t' << value << '\n';
        }
    }

    bool get(const std::string& key, std::string& value) const {
        std::ifstream file(filename);

        std::string current_key;
        std::string current_value;

        while (file >> current_key >> current_value) {
            if (current_key == key) {
                value = current_value;
                return true;
            }
        }

        return false;
    }
};