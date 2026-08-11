#pragma once

#include <string>
#include <vector>
#include <utility>

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

    void flush() {
        std::string filename = "sstable_" + std::to_string(sstables.size()) + ".db";
        SSTable sstable(filename);
        sstable.write(memtable.entries());
        sstables.push_back(std::move(sstable));
        memtable.clear();
    };

public:
    ERR_CODE get(const std::string& key,std::string& value) const {
        ENTRY_STATUS status = memtable.get(key, value);

        if (status == ENTRY_STATUS::PRESENT)
            return ERR_CODE::SUCCESS;

        if (status == ENTRY_STATUS::DELETED)
            return ERR_CODE::KEY_NOT_FOUND;

        for (auto it = sstables.rbegin(); it != sstables.rend(); ++it){
            ENTRY_STATUS status = it->get(key, value);

            if (status == ENTRY_STATUS::PRESENT)
                return ERR_CODE::SUCCESS;

            if (status == ENTRY_STATUS::DELETED)
                return ERR_CODE::KEY_NOT_FOUND;
        }

        return ERR_CODE::KEY_NOT_FOUND;
    }

    ERR_CODE set(const std::string& key,const std::string& value){
        if (check(key))
            return ERR_CODE::KEY_ALREADY_EXISTS;

        if (value.empty())
            return ERR_CODE::EMPTY_VALUE;

        memtable.put(key, value);

        if (memtable.full())
            flush();

        return ERR_CODE::SUCCESS;
    };

    ERR_CODE remove(const std::string& key){
        if (!check(key))
            return ERR_CODE::KEY_NOT_FOUND;

        memtable.remove(key);

        return ERR_CODE::SUCCESS;
    };

    bool check(const std::string& key) const {
        std::string value;
        return get(key, value) == ERR_CODE::SUCCESS;
    }

    ERR_CODE update(const std::string& key, const std::string& value){
        if (!check(key))
            return ERR_CODE::KEY_NOT_FOUND;

        if (value.empty())
            return ERR_CODE::EMPTY_VALUE;

        memtable.put(key, value);
        return ERR_CODE::SUCCESS;
    };
};