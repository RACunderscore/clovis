#pragma once

#include <sstream>
#include <string>

#include "data_storage.h"
#include "query_result.h"

class Query_parser {
private:
    Data_storage& data;

public:
    Query_parser(Data_storage& data_): data(data_) {}

    std::string get_command(const std::string& query) {
        if (query.size() < 4 || query[3] != ' ')
            return "";

        std::string cmd = query.substr(0, 3);

        bool valid = cmd == "INS" ||
            cmd == "GET" ||
            cmd == "DEL" ||
            cmd == "UPD";

        if (!valid)
            return "";

        return cmd;
    }

    Query_result parser(const std::string& query) {
        std::stringstream ss(query);

        std::string cmd;
        std::string key;
        std::string value;

        ss >> cmd;

        if (cmd == "INS") {
            ss >> key >> value;

            ERR_CODE result = data.set(key, value);

            if (result == ERR_CODE::SUCCESS) {
                return Query_result(true,200,"Insert successful");
            }

            if (result == ERR_CODE::KEY_ALREADY_EXISTS) {
                return Query_result(false,409,"Key already exists");
            }

            if (result == ERR_CODE::EMPTY_VALUE) {
                return Query_result(false,400,"Value cannot be empty");
            }

            return Query_result(false,400,"Insert failed");
        }

        if (cmd == "GET") {
            ss >> key;

            std::string value;

            ERR_CODE result = data.get(key, value);

            if (result == ERR_CODE::SUCCESS) {
                return Query_result(true,200,"GET successful",value);
            }

            if (result == ERR_CODE::KEY_NOT_FOUND) {
                return Query_result(false,404,"Key not found");
            }

            return Query_result(false,400,"GET failed");
        }

        if (cmd == "DEL") {
            ss >> key;

            ERR_CODE result = data.remove(key);

            if (result == ERR_CODE::SUCCESS) {
                return Query_result(true,200,"Delete successful");
            }

            if (result == ERR_CODE::KEY_NOT_FOUND) {
                return Query_result(false,404,"Key not found");
            }

            return Query_result(false,400,"Delete failed");
        }

        if (cmd == "UPD") {
            ss >> key >> value;

            ERR_CODE result = data.update(key, value);

            if (result == ERR_CODE::SUCCESS) {
                return Query_result(true,200,"UPDATE successful");
            }

            if (result == ERR_CODE::KEY_NOT_FOUND) {
                return Query_result(false,404,"Key not found");
            }

            if (result == ERR_CODE::EMPTY_VALUE) {
                return Query_result(false,400,"Value cannot be empty");
            }

            return Query_result(false,400,"UPDATE failed");
        }

        return Query_result(false,400,"Invalid command");
    }
};