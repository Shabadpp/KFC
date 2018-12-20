#include "BloomFilter.hpp"
#include "smhasher/src/MurmurHash3.h"
#include <array>

using namespace std;

BloomFilter::BloomFilter(uint64_t size, uint8_t num_hashes, float reset_ratio) {
	this->m_bits = std::vector<bool>(size);
	this->m_num_hashes = num_hashes;
	this->m_bits_set = 0;
	this->m_reset_ratio = reset_ratio;
}

static std::array<uint64_t, 2> hash_wesh(const uint8_t* data, std::size_t len) {
	std::array<uint64_t, 2> hash_value;
	MurmurHash3_x64_128(data, len, 0, hash_value.data());

	return hash_value;
}

inline uint64_t nthHash(uint8_t n, uint64_t hashA, uint64_t hashB, uint64_t filterSize) {
	return (hashA + n * hashB) % filterSize;
}

void BloomFilter::add(const uint8_t* data, std::size_t len) {
	auto hash_values = hash_wesh(data, len);

	for (int n = 0; n < this->m_num_hashes; n++) {
		uint64_t hash = nthHash(n, hash_values[0], hash_values[1], m_bits.size());
		if (!m_bits[hash]) {
			m_bits_set++;
			m_bits[hash] = true;
		}
	}

	if (this->m_bits_set * 1. / len >= reset_ratio) {
		reset();
	}
}

bool BloomFilter::possiblyContains(const uint8_t* data, std::size_t len) const {
	auto hash_values = hash_wesh(data, len);

	for (int n = 0; n < this->m_num_hashes; n++) {
		if (!m_bits[nthHash(n, hash_values[0], hash_values[1], m_bits.size())]) {
			return false;
		}
	}

	return true;
}

void BloomFilter::reset() {
	this->m_bits.reset();
}

ostream& operator<<(ostream& out, BloomFilter& bf) {
	for (auto val : bf.m_bits) {
		out << (val ? 1 : 0);
	}
	out << " (" << bf.m_bits_set << ')';

	return out;
}
