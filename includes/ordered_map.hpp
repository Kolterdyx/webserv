
#ifndef WEBSERVCONFIG_ORDERED_MAP_HPP
#define WEBSERVCONFIG_ORDERED_MAP_HPP

#include <unordered_map>
#include <vector>

template <typename K, typename V>
class ordered_map {

private:
	std::unordered_map<K, V> _map;
	std::vector<K> _keys;

public:
	ordered_map();
	ordered_map(const ordered_map &other);
	ordered_map &operator=(const ordered_map &other);

	V operator[](const K &key) const;

	V &at(const K &key);

	V &at(int index);

	const V &at(const K &key) const;

	const V &at(int index) const;

	bool contains(const K &key) const;

	void erase(const K &key);

	void clear();

	size_t size() const;

	std::vector<K> keys() const;

	std::vector<V> values() const;

	typename std::vector<K>::const_iterator begin() const;

	typename std::vector<K>::const_iterator end() const;

	bool empty() const;

	template<typename K1, typename V1>
	friend std::ostream &operator<<(std::ostream &os, const ordered_map<K1, V1> &map);

	void insert(K key, V value);
};

#include "../src/WebservConfig/ordered_map.tpp"


#endif //WEBSERVCONFIG_ORDERED_MAP_HPP
