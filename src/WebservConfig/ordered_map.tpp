
#include "ordered_map.hpp"

template<typename K, typename V>
ordered_map<K, V>::ordered_map(const ordered_map &other) {
	*this = other;
}

template<typename K, typename V>
ordered_map<K, V> &ordered_map<K, V>::operator=(const ordered_map &other) {
	if (this != &other) {
		_map = other._map;
		_keys = other._keys;
	}
	return *this;
}

template<typename K, typename V>
V ordered_map<K, V>::operator[](const K &key) const {
	if (_map.find(key) == _map.end()) {
		((std::vector<K>)_keys).push_back(key);
	}
	return ((std::unordered_map<K, V>)_map)[key];
}

template<typename K, typename V>
std::vector<K> ordered_map<K, V>::keys() const {
	return _keys;
}

template<typename K, typename V>
size_t ordered_map<K, V>::size() const {
	return _map.size();
}

template<typename K, typename V>
void ordered_map<K, V>::clear() {
	_map.clear();
	_keys.clear();
}

template<typename K, typename V>
void ordered_map<K, V>::erase(const K &key) {
	_map.erase(key);
	_keys.erase(std::remove(_keys.begin(), _keys.end(), key), _keys.end());
}

template<typename K, typename V>
bool ordered_map<K, V>::contains(const K &key) const {
	return _map.find(key) != _map.end();
}

template<typename K, typename V>
const V &ordered_map<K, V>::at(const K &key) const {
	return _map.at(key);
}

template<typename K, typename V>
V &ordered_map<K, V>::at(const K &key) {
	return _map.at(key);
}

template<typename K, typename V>
ordered_map<K, V>::ordered_map() {

}

template<typename K, typename V>
bool ordered_map<K, V>::empty() const {
	return _map.empty();
}

template<typename K, typename V>
V &ordered_map<K, V>::at(int index) {
	return _map.at(_keys[index]);
}
template<typename K, typename V>
const V &ordered_map<K, V>::at(int index) const {
	return _map.at(_keys[index]);
}

template<typename K, typename V>
std::vector<V> ordered_map<K, V>::values() const {
	std::vector<V> values;
	for (typename std::vector<K>::const_iterator it = _keys.begin(); it != _keys.end(); ++it) {
		values.push_back(_map.at(*it));
	}
	return values;
}

template<typename K, typename V>
typename std::vector<K>::const_iterator ordered_map<K, V>::begin() const {
	return _keys.begin();
}

template<typename K, typename V>
typename std::vector<K>::const_iterator ordered_map<K, V>::end() const {
	return _keys.end();
}

template<typename K, typename V>
std::ostream &operator<<(std::ostream &os, const ordered_map<K, V> &map) {
	os << "{\n";
	for (typename std::vector<K>::const_iterator it = map._keys.begin(); it != map._keys.end(); ++it) {
		os << "  " << *it << ": " << map[*it] << "\n";
	}
	os << "}";
	return os;
}

template<typename K, typename V>
void ordered_map<K, V>::insert(K key, V value) {
	_map[key] = value;
	_keys.push_back(key);
}
