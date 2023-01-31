#ifndef UUID_HPP
#define UUID_HPP

#include <stdint.h>
#include <iostream>
#include <cstdlib>
#include <iomanip>
#include <ctime>
#include <vector>
#include <map>

class UUID {
private:
	uint64_t part1;
	uint64_t part2;

	void init_srand();

public:

	// Constructors
	UUID();

	UUID(const UUID &uuid);

	// operator overloads
	bool operator==(const UUID &uuid) const;

	bool operator!=(const UUID &uuid) const;

	UUID &operator=(const UUID &uuid);

	bool operator<(const UUID &uuid) const;

	bool operator>(const UUID &uuid) const;

	bool operator<=(const UUID &uuid) const;

	bool operator>=(const UUID &uuid) const;

	uint64_t getPart1() const;
	uint64_t getPart2() const;

	// stream operator overloads
	friend std::ostream &operator<<(std::ostream &os, const UUID &uuid);

};

#endif //UUID_HPP
