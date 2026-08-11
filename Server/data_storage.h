#pragma once

#include <unordered_map>
#include <string>
#include <vector>

enum class ERR_CODE { 
    SUCCESS = 0, 
    KEY_NOT_FOUND, 
    KEY_ALREADY_EXISTS, 
    EMPTY_VALUE 
};

class Data_storage {
private:
    std::unordered_map<std::string, std::string> data;

public:
    Data_storage() = default;

    ERR_CODE get(const std::string& key, std::string& value) const {
        auto it = data.find(key);
        if (it == data.end()) 
            return ERR_CODE::KEY_NOT_FOUND;
        
        value = it->second;
        return ERR_CODE::SUCCESS;
    }


    ERR_CODE set(const std::string& key, const std::string& value) {
        if (check(key))
            return ERR_CODE::KEY_ALREADY_EXISTS;
        if (!value.length())
            return ERR_CODE::EMPTY_VALUE;

        data[key] = value;
        return ERR_CODE::SUCCESS;
    }


    ERR_CODE remove(const std::string& key) {
        if (!check(key))
            return ERR_CODE::KEY_NOT_FOUND;

        data.erase(key);
        return ERR_CODE::SUCCESS;
    }


    bool check(const std::string& key) const {
        return data.find(key) != data.end();
    }


    ERR_CODE update(const std::string& key, const std::string& value) {
        if (!check(key))
            return ERR_CODE::KEY_NOT_FOUND;
        if (!value.length())
            return ERR_CODE::EMPTY_VALUE;

        data[key] = value;
        return ERR_CODE::SUCCESS;
    }

    size_t length() const{
        return data.size();
    };

    std::string* keys() const {
        std::string* result = new std::string[data.size()];

        size_t i = 0;
        for (const auto& [key, value] : data) {
            result[i++] = key;
        }

        return result;
    }
};