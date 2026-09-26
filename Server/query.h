#pragma once
#include <string>
#include <sstream>
#include "query_result.h"
#include "data_storage.h"

enum class Command { GET, INS, UPD, DEL, UNKNOWN };

class Query {
private:
    Command command;
    std::string key;
    std::string value;
    std::string raw_query;
    Response_format format;
    Query_result result;

    std::string trim(const std::string& str) {
        std::size_t start = 0;
        std::size_t end = str.size();

        while (start < end &&
            std::isspace(static_cast<unsigned char>(str[start]))) {
            ++start;
        }

        while (end > start &&
            std::isspace(static_cast<unsigned char>(str[end - 1]))) {
            --end;
        }

        return str.substr(start, end - start);
    }

    bool get_cleaned_value(std::string* value) {
        *value = trim(*value);

        if (value->empty() || value->front() != '"') {
            return false;
        }

        std::size_t closing_quote = value->find('"', 1);

        if (closing_quote == std::string::npos) {
            return false;
        }

        std::string cleaned_value = trim(value->substr(1, closing_quote - 1));
        *value = cleaned_value;
        return true;
    }

public:
    explicit Query(const std::string& query_str): raw_query(query_str), format(Response_format::TEXT), result(false, 0, "Pending execution"){
        if (query_str.size() < 4 || query_str[3] != ' ') {
            command = Command::UNKNOWN;
            return;
        }

        std::string cmd_str = query_str.substr(0, 3);
        std::stringstream ss(query_str.substr(4));

        if (cmd_str == "GET") {
            command = Command::GET;
            ss >> key;
            std::string format_buffer;
            ss >> format_buffer;
            if (format_buffer == "--json") format = Response_format::JSON;
            else if (format_buffer == "--xml") format = Response_format::XML;
        } 
        else if (cmd_str == "INS" || cmd_str == "UPD") {
            command = (cmd_str == "INS") ? Command::INS : Command::UPD;
            ss >> key;
            std::getline(ss, value);
            if (!get_cleaned_value(&value)) {
                result = Query_result(false, 400, "Value must be enclosed in quotes");
            }
        }
        else if (cmd_str == "DEL") {
            command = Command::DEL;
            ss >> key;
        } else {
            command = Command::UNKNOWN;
        }
    }

    void run(Data_storage& db) {
        if (result.get_status_code() != 0) {
            return; 
        }
        else if (command == Command::UNKNOWN) {
            result = Query_result(false, 400, "Invalid command");
            return;
        }
        

        if (command == Command::GET) {
            std::string out_value;
            ERR_CODE err = db.get(key, out_value);
            if (err == ERR_CODE::SUCCESS) {
                result = Query_result(true, 200, "GET successful", out_value, format);
            } else {
                result = Query_result(false, 404, "Key not found");
            }
        }
        else if (command == Command::INS) {
            ERR_CODE err = db.set(key, value);

            if (err == ERR_CODE::SUCCESS) {
                result = Query_result(true, 200, "Insert successful");
            }
            else if (err == ERR_CODE::KEY_ALREADY_EXISTS) {
                result = Query_result(false, 409, "Key already exists");
            }
            else if (err == ERR_CODE::EMPTY_VALUE) {
                result = Query_result(false, 400, "Value cannot be empty");
            }
            else{
                result = Query_result(false, 400, "Insert failed");
            }
        }
        else if (command == Command::UPD) {
            ERR_CODE err = db.update(key, value);

            if (err == ERR_CODE::SUCCESS) {
                result = Query_result(true,200,"UPDATE successful");
            }
            else if (err == ERR_CODE::KEY_NOT_FOUND) {
                result = Query_result(false,404,"Key not found");
            }
            else if (err == ERR_CODE::EMPTY_VALUE) {
                result = Query_result(false,400,"Value cannot be empty");
            }
            else{
                result = Query_result(false,400,"UPDATE failed");
            }
        }
        else if (command == Command::DEL) {
            ERR_CODE err = db.remove(key);

            if (err == ERR_CODE::SUCCESS) {
                result = Query_result(true,200,"Delete successful");
            }
            else if (err == ERR_CODE::KEY_NOT_FOUND) {
                result = Query_result(false,404,"Key not found");
            }
            else {
                result = Query_result(false,400,"Delete failed");
            }
        }

    }

    Query_result get_result() const { return result; }
    std::string get_raw_query() const { return raw_query; }
};