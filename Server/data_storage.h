#pragma once

#include <string>
#include <vector>
#include <utility>
#include <filesystem>
#include <iostream>

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
        if (sstables.size() == 2)
            compact();
    };

public:

    Data_storage(){
        if(std::filesystem::exists("sstable_0.db")){
            std::string filename = "sstable_0.db";
            SSTable sstable(filename);
            memtable = MemTable(sstable.read_all());
            std::cout << "Sstable loaded" << std::endl;
        }
        else{
            std::cout << "No Sstable detected" << std::endl;
        }
    };

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

    void compact() {
        std::cout << "SStable compacted" << std::endl;
        std::map<std::string, Entry> merged;

        for (const auto& sstable : sstables) {
            auto entries = sstable.read_all();

            for (const auto& [key, entry] : entries) {
                merged[key] = entry;
            }
        }

        for (auto it = merged.begin(); it != merged.end(); ) {
            if (it->second.status == ENTRY_STATUS::DELETED)
                it = merged.erase(it);
            else
                ++it;
        }

        SSTable compacted("sstable_compacted.db");
        compacted.write(merged);

        for (const auto& sstable : sstables) {
            std::filesystem::remove(sstable.get_filename());
        }

        std::filesystem::rename("sstable_compacted.db", "sstable_0.db");

        sstables.clear();
        sstables.emplace_back("sstable_0.db");
    }
};