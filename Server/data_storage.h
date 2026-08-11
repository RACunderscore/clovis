#pragma once

#include <unordered_map>
#include <string>
#include <vector>

#include "sstable.h"
#include "memtable.h"

enum class ERR_CODE { 
    SUCCESS = 0, 
    KEY_NOT_FOUND, 
    KEY_ALREADY_EXISTS, 
    EMPTY_VALUE 
};

class Data_storage {
private:
    MemTable memtable;

    std::vector<SSTable> sstables;

public:
    ERR_CODE get(const std::string& key, std::string& value) const;
    ERR_CODE set(const std::string& key, const std::string& value);
    ERR_CODE remove(const std::string& key);
    bool check(const std::string& key) const;
    ERR_CODE update(const std::string& key, const std::string& value);

    size_t length() const;
};