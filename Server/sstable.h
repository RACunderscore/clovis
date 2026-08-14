#pragma once

#include <fstream>
#include <map>
#include <string>
#include <cstddef>

#include "entry.h"

class SSTable {
private:
    std::string filename;

public:
    explicit SSTable(const std::string& filename): filename(filename) {}

    const std::string& get_filename() const {
        return filename;
    }

    void write(const std::map<std::string, Entry>& data) {
        std::ofstream file(filename);

        if (!file.is_open())
            return;

        for (const auto& [key, entry] : data) {
            file << key << '|' << static_cast<int>(entry.status) << '|' << entry.value << '\n';
        }
    }

    ENTRY_STATUS get(const std::string& key,std::string& value) const {
        std::ifstream file(filename);

        if (!file.is_open())
            return ENTRY_STATUS::NOT_PRESENT;

        std::string line;

        while (std::getline(file, line)) {
            std::size_t first = line.find('|');
            if (first == std::string::npos)
                continue;

            std::size_t second = line.find('|', first + 1);

            if (second == std::string::npos)
                continue;

            std::string current_key =
                line.substr(0, first);

            if (current_key != key)
                continue;

            int status = std::stoi(line.substr(first + 1, second - first - 1));
            std::string current_value = line.substr(second + 1);

            ENTRY_STATUS entry_status = static_cast<ENTRY_STATUS>(status);

            if (entry_status == ENTRY_STATUS::DELETED)
                return ENTRY_STATUS::DELETED;

            if (entry_status == ENTRY_STATUS::PRESENT) {
                value = current_value;
                return ENTRY_STATUS::PRESENT;
            }
        }

        return ENTRY_STATUS::NOT_PRESENT;
    }

    std::map<std::string, Entry> read_all() const {
        std::map<std::string, Entry> data;

        std::ifstream file(filename);

        if (!file.is_open())
            return data;

        std::string line;

        while (std::getline(file, line)) {
            std::size_t first = line.find('|');
            if (first == std::string::npos)
                continue;

            std::size_t second = line.find('|', first + 1);
            if (second == std::string::npos)
                continue;

            std::string key = line.substr(0, first);

            int status = std::stoi(
                line.substr(first + 1, second - first - 1)
            );

            std::string value = line.substr(second + 1);

            Entry entry;
            entry.status = static_cast<ENTRY_STATUS>(status);
            entry.value = value;

            data[key] = entry;
        }

        return data;
    }
};