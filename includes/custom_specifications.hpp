#ifndef CUSTOM_SPECIFICATIONS_HPP
#define CUSTOM_SPECIFICATIONS_HPP

#include <vector>
#include <map>
#include <iostream>

namespace std {
    template<typename T>
    std::ostream &operator<<(std::ostream &os, const std::vector<T> vector) {
		int i = 0;
        for (typename std::vector<T>::const_iterator it = vector.begin(); it != vector.end(); it++) {
            os << i++ << ": " << *it << "\n";
        }
        return os;
    }

    template<typename K, typename V>
    std::ostream &operator<<(std::ostream &os, const std::map<K, V> map) {
        for (typename std::map<K, V>::const_iterator it = map.begin(); it != map.end(); it++) {
            os << it->first << ": " << it->second << "\n";
        }
        return os;
    }
}

#endif //CUSTOM_SPECIFICATIONS_HPP
