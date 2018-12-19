#include <fstream>
#include <cstring>
#include <string>
#include <vector>
#include <iostream>
#include <algorithm>
#include "index_min.h"

#include "BloomFilter.hpp"




using namespace std;


string getLineFasta(ifstream* in){
	string line,result;
	getline(*in,line);
	char c=in->peek();
	while(c!='>' and c!=EOF){
		getline(*in,line);
		result+=line;
		c=in->peek();
	}
	return result;
}


void clean(string& str){
	for(uint i(0); i< str.size(); ++i){
		switch(str[i]){
			case 'a':break;
			case 'A':break;
			case 'c':break;
			case 'C':break;
			case 'g':break;
			case 'G':break;
			case 't':break;
			case 'T':break;
			default: str="";return;
		}
	}
	transform(str.begin(), str.end(), str.begin(), ::toupper);
}

void insert_sequence(const string& seq){
	return;
}



int main(int argc, char ** argv){
	BloomFilter bf(1000, 3);

	if(argc<2){
		cout<<"[Fasta file]"<<endl;
		exit(0);
	}
	string input(argv[1]);

	srand (time(NULL));
	string header, sequence,line;
	ifstream in(input);


	while(not in.eof()){
		getline(in,header);
		if(header[0]!='>'){continue;}
		char c=in.peek();
		while(c!='>' and c!=EOF){
			getline(in,line);
			sequence+=line;
			c=in.peek();
		}
		insert_sequence(sequence);
		sequence="";
	}
	vector<uint64_t> abundant_kmer;
	index_full index(abundant_kmer);
}
