#pragma once

#include <string>

enum class ENTRY_STATUS {
    PRESENT,
    NOT_PRESENT,
    DELETED
};

struct Entry {
    std::string value;
    ENTRY_STATUS status;
};