#include <iostream>
#include <string>
#include "MyString.hpp"

int main() {
    // This main implements a minimal interactive tester
    // Input format (ad-hoc): commands to manipulate MyString
    // If no input, just quick sanity and exit 0
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    MyString s; // start empty
    std::string cmd;
    if (!(std::cin >> cmd)) return 0;
    do {
        if (cmd == "set") {
            std::string x; std::cin >> x; s = MyString(x.c_str());
        } else if (cmd == "append") {
            std::string x; std::cin >> x; s.append(x.c_str());
        } else if (cmd == "size") {
            std::cout << s.size() << "\n";
        } else if (cmd == "cap") {
            std::cout << s.capacity() << "\n";
        } else if (cmd == "resize") {
            size_t n; std::cin >> n; s.resize(n);
        } else if (cmd == "reserve") {
            size_t n; std::cin >> n; s.reserve(n);
        } else if (cmd == "get") {
            size_t i; std::cin >> i; try { std::cout << s.at(i) << "\n"; } catch (...) { std::cout << "err\n"; }
        } else if (cmd == "print") {
            std::cout << s.c_str() << "\n";
        }
    } while (std::cin >> cmd);
    return 0;
}
