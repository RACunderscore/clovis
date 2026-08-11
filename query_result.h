#pragma once

#include <string>
#include <sstream>

class Query_result {
private:
    bool success;
    int status_code;
    std::string message;
    std::string value;

    static std::string json_escape(const std::string& str) {
        std::string result;

        for (char c : str) {
            switch (c) {
                case '"':
                    result += "\\\"";
                    break;
                case '\\':
                    result += "\\\\";
                    break;
                case '\n':
                    result += "\\n";
                    break;
                case '\r':
                    result += "\\r";
                    break;
                case '\t':
                    result += "\\t";
                    break;
                default:
                    result += c;
                    break;
            }
        }

        return result;
    }

    static std::string xml_escape(const std::string& str) {
        std::string result;

        for (char c : str) {
            switch (c) {
                case '&':
                    result += "&amp;";
                    break;
                case '<':
                    result += "&lt;";
                    break;
                case '>':
                    result += "&gt;";
                    break;
                case '"':
                    result += "&quot;";
                    break;
                case '\'':
                    result += "&apos;";
                    break;
                default:
                    result += c;
                    break;
            }
        }

        return result;
    }

    std::string http_status_text() const {
        switch (status_code) {
            case 200:
                return "OK";
            case 201:
                return "Created";
            case 400:
                return "Bad Request";
            case 404:
                return "Not Found";
            case 409:
                return "Conflict";
            case 500:
                return "Internal Server Error";
            default:
                return "Unknown";
        }
    }

public:
    Query_result(bool success_,int status_code_,const std::string& message_,const std::string& value_ = "")
        :success(success_),status_code(status_code_),message(message_),value(value_) {}

    bool is_success() const {
        return success;
    }

    int get_status_code() const {
        return status_code;
    }

    const std::string& get_message() const {
        return message;
    }

    const std::string& get_value() const {
        return value;
    }

    std::string to_text() const {
        if (!success)
            return message + "\n";

        if (!value.empty())
            return value + "\n";

        return message + "\n";
    }

    std::string to_json() const {
        std::stringstream ss;

        ss << "{" << "\"status\":\"" << json_escape(success ? "success" : "error") << "\","
           << "\"message\":\"" << json_escape(message) << "\"";
        if (!value.empty()) {
            ss << "," << "\"value\":\"" << json_escape(value) << "\"";
        }
        ss << "}\n";
        return ss.str();
    }

    std::string to_xml() const {
        std::stringstream ss;
        ss << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
           << "<response>\n"
           << "  <status>"
           << xml_escape(success ? "success" : "error")
           << "</status>\n"
           << "  <message>"
           << xml_escape(message)
           << "</message>\n";

        if (!value.empty()) {
            ss << "  <value>" << xml_escape(value) << "</value>\n";
        }
        ss << "</response>\n";
        return ss.str();
    }

    std::string to_http(const std::string& content_type = "application/json") const {
        std::string body;
        if (content_type == "application/json") {
            body = to_json();
        }
        else if (content_type == "application/xml" || content_type == "text/xml") {
            body = to_xml();
        }
        else {
            body = to_text();
        }

        std::stringstream response;

        response << "HTTP/1.1 " << status_code << " " << http_status_text() << "\r\n";
        response << "Content-Type: " << content_type << "\r\n";
        response << "Content-Length: " << body.size() << "\r\n";
        response << "Connection: close\r\n";
        response << "\r\n";
        response << body;
        return response.str();
    }
};
