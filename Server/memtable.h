#pragma once

#include <map>
#include <string>

#include "entry.h"

class MemTable {
private:
    std::map<std::string, Entry> data;

public:
    static constexpr std::size_t MAX_MEMTABLE_SIZE = 1;

    void put(const std::string& key,const std::string& value){
        data[key] = {value, ENTRY_STATUS::PRESENT};
    };

    void remove(const std::string& key){
        data[key] = {"", ENTRY_STATUS::DELETED};
    };

    ENTRY_STATUS get(const std::string& key,std::string& value) const{
        auto it = data.find(key);

        if (it == data.end())
            return ENTRY_STATUS::NOT_PRESENT;

        if (it->second.status == ENTRY_STATUS::DELETED)
            return ENTRY_STATUS::DELETED;

        value = it->second.value;
        return ENTRY_STATUS::PRESENT;
    };

    bool contains(const std::string& key) const {
        return data.find(key) != data.end();
    }

    bool exists(const std::string& key) const {
        auto it = data.find(key);
        return it != data.end() && it->second.status == ENTRY_STATUS::PRESENT;
    }

    bool full() const {
        return data.size() >= MAX_MEMTABLE_SIZE;
    };

    size_t size() const{
        return data.size();
    };

    void clear() {
        data.clear();
    };

    const std::map<std::string, Entry>& entries() const {
        return data;
    };
};