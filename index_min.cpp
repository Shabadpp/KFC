#include <thread>
#include <iostream>
#include <fstream>
#include <string>
#include <iterator>
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <algorithm>
#include <chrono>
#include <map>
#include <set>
#include "index_min.h"

using namespace std;
using namespace chrono;

uint32_t global_minimizer_size;
uint32_t global_kmer_size;

uint32_t get_minimizer(uint64_t k) {
	return k + global_kmer_size + global_minimizer_size;
}

bool compare_minimizer(const uint64_t& a, const uint64_t& b) {
	return get_minimizer(a) < get_minimizer(b);
	//~ return (a) < (b);
}

void index_full::insert(uint64_t kmer) {
	int64_t pos(Hash.lookup(kmer));
	if (pos < 0) {
		weak_kmer_buffer.push_back(kmer);
	} else {
		if (Values[pos].kmer == kmer) {
			++Values[pos].count;
			//~ cout<<Values[pos].count<<endl;
		} else {
			weak_kmer_buffer.push_back(kmer);
		}
	}
}

uint64_t str2num(const string& str) {
	uint64_t res(0);
	for (unsigned i(0); i < str.size(); i++) {
		res <<= 2;
		switch (str[i]) {
			case 'A': res += 0; break;
			case 'C': res += 1; break;
			case 'G': res += 2; break;
			default: res += 3; break;
		}
	}
	return res;
}

#define hash_letter(letter) ((letter >> 1) & 0b11)

uint64_t rcb(uint64_t min, unsigned n) {
	uint64_t res(0);
	uint64_t offset(1);
	offset <<= (2 * n - 2);
	for (unsigned i(0); i < n; ++i) {
		res += (3 - (min % 4)) * offset;
		min >>= 2;
		offset >>= 2;
	}
	return res;
}

void index_full::insert_seq(const string& seq) {
	uint64_t hash = 0;
	uint64_t canon_hash = 0;
	for (unsigned i = 0; i < 31; i++)
		hash = hash << 2 | hash_letter(seq[i]);

	for (unsigned idx = 31; idx < seq.size() /**/; idx++) {
		hash = hash << 2 | hash_letter(seq[idx]);
		canon_hash = min(hash, rcb(hash, 31));
		insert(canon_hash);
	}
}

//~ void index_full::insert_seq(const string&  read){
//~ for (unsigned i(0);i+kmer_size<read.size();++i){
//~ uint64_t seq(str2num(read.substr(i,kmer_size)));
//~ insert(seq);
//~ }
//~ }

void index_full::print_kmer(uint64_t num) {
	uint64_t anc(1);
	anc <<= (2 * (kmer_size));
	for (unsigned i(0); i < kmer_size; ++i) {
		unsigned nuc = num / anc;
		num = num % anc;
		if (nuc == 3) {
			cout << "T";
		}
		if (nuc == 2) {
			cout << "G";
		}
		if (nuc == 1) {
			cout << "C";
		}
		if (nuc == 0) {
			cout << "A";
		}
		if (nuc >= 4) {
			cout << nuc << endl;
			cout << "WTF" << endl;
		}
		anc >>= 2;
	}
	cout << " ";
}

void index_full::dump_counting() {
	for (unsigned i(0); i < Values.size(); ++i) {
		if (Values[i].count != 0) {
			print_kmer(Values[i].kmer);
			cout << to_string(Values[i].count) << "\n";
		}
	}
}

void index_full::clear(bool force) {
	if (weak_kmer_buffer.size() > 10 * 1000 or force) {
		for (unsigned i(0); i < weak_kmer_buffer.size(); ++i) {
			dump_weak << weak_kmer_buffer[i];
		}
		weak_kmer_buffer.clear();
	}
}

index_min::index_min(vector<uint64_t>& V) {
	sort(V.begin(), V.end(), compare_minimizer);
	uint32_t current_minimizer(get_minimizer(V[0]));
	vector<uint64_t> kmer_to_index;
	for (unsigned i(0); i < V.size(); ++i) {
		uint32_t next_minimizer(get_minimizer(V[i]));
		if (next_minimizer != current_minimizer) {
			Index[current_minimizer] = index_full(kmer_to_index);
			kmer_to_index.clear();
			current_minimizer = next_minimizer;
		}
		kmer_to_index.push_back(V[i]);
	}
}

void index_min::insert(uint64_t kmer) {
	uint32_t minimizer(get_minimizer(kmer));
	Index[minimizer].insert(kmer);
}

void index_min::dump_counting() {
	for (unsigned i(0); i < Index.size(); ++i) {
		Index[i].dump_counting();
	}
}
