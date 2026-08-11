#include <map>
#include <string>

class MemTable {
private:
    std::map<std::string, std::string> data;

public:
    void put(const std::string& key, const std::string& value) {
        data[key] = value;
    }

    bool get(const std::string& key, std::string& value) const {
        auto it = data.find(key);

        if (it == data.end())
            return false;

        value = it->second;
        return true;
    }

    bool contains(const std::string& key) const {
        return data.find(key) != data.end();
    }

    size_t size() const {
        return data.size();
    }
};