#~ CC=/usr/bin/g++
CC=g++
CFLAGS= -Wall -g -std=c++11   -pipe -funit-at-a-time -fopenmp -lz -Isparsepp
LDFLAGS= -lpthread -g -fopenmp -lz  -Isparsepp  -flto smhasher/src/libSMHasherSupport.a

CPPS = $(wildcard *.cpp)
OBJS = $(CPPS:.cpp=.o)
KFC_OBJ = kfc.o SolidSampler.o BitSet.o BloomFilter.o Hash.o	CascadingBloomFilter.o index_min.o


EXEC=kfc kmerCountEvaluator
LIB=kfc.a
all: $(EXEC) $(LIB)

kmerCountEvaluator:   evaluator.o
	$(CC) -o $@ $^ $(LDFLAGS)


kfc: $(KFC_OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)

kfc.a: $(KFC_OBJ)
	ar rcs kfc.a $(KFC_OBJ)

%.o: %.cpp
	$(CC) -o $@ -c $< $(CFLAGS)


clean:
	rm -f *.o *.a
	rm -rf $(EXEC)


rebuild: clean $(EXEC)
