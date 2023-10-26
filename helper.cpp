#include "helper.h"

namespace Helper{

inline std::string AsString(std::string_view sv){
    return {sv.data(), sv.size()};
}

std::string WildcardToRegex(std::string wildcard){
    std::string s = "^";
    for( const char c: wildcard){
        switch(c) {
        case '*':
            s += ".*";
            break;
        case '?':
            s += ".";
            break;
        case '^': // escape character in cmd.exe
            s +="\\";
            break;
            // escape special regexp-characters
        case '(': case ')': case '[': case ']': case '$':
        case '.': case '{': case '}': case '|':
        case '\\':
            s += "\\";
            s += std::tolower(c);
            break;
        default:
            s += std::tolower(c);
            break;
        }
    }
    s += "$";
    return s;
}

}
