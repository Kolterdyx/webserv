
#include "UUID/UUID.hpp"


UUID::UUID() {
	init_srand();
	for (int i = 0; i < 64; i++) {
		if (std::rand() % 2) {
			this->part1 |= (uint64_t) 1 << (i);
		}
		if (std::rand() % 2) {
			this->part2 |= (uint64_t) 1 << (i);
		}
	}
}

UUID::UUID(const UUID &uuid) {
	*this = uuid;
}

bool UUID::operator==(const UUID &uuid) const {
	return this->part1 == uuid.part1 && this->part2 == uuid.part2;
}

bool UUID::operator!=(const UUID &uuid) const {
	return !(*this == uuid);
}

UUID &UUID::operator=(const UUID &uuid) {
	this->part1 = uuid.part1;
	this->part2 = uuid.part2;
	return *this;
}

std::ostream &operator<<(std::ostream &os, const UUID &uuid) {
	// 8 hex digits, 4 hex digits, 4 hex digits, 4 hex digits, 12 hex digits
	uint64_t p1 = uuid.part1 >> 32;
	uint64_t p2 = (uuid.part1 & 0xFFFF0000) >> 16;
	uint64_t p3 = uuid.part1 & 0xFFFF;
	uint64_t p4 = uuid.part2 >> 48;
	uint64_t p5 = uuid.part2 & 0xFFFFFFFFFFFF;

	os << std::hex << std::setfill('0');
	os << std::setw(8) << p1 << "-";
	os << std::setw(4) << p2 << "-";
	os << std::setw(4) << p3 << "-";
	os << std::setw(4) << p4 << "-";
	os << std::setw(12) << p5;
	return os;
}

void UUID::init_srand() {
	static bool srand_initialized = false;
	if (!srand_initialized) {
		std::srand(std::time(NULL));
		srand_initialized = true;
	}
}




