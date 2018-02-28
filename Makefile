CXX_FLAG := -fPIC -std=c++11 -fno-rtti
CLEAN_FILE := flatten.o crypto_util.o util.o split.o libtrob.so
BUILD_FILE := flatten.o crypto_util.o util.o split.o 

build:
	clang++ -c flatten.cc $(CXX_FLAG)
	clang++ -c crypto_util.cc $(CXX_FLAG)
	clang++ -c util.cc $(CXX_FLAG)
	clang++ -c split.cc $(CXX_FLAG)
	clang++ trob.cc flatten.o crypto_util.o util.o split.o -shared -o libtrob.so $(CXX_FLAG)

clean:
	rm $(CLEAN_FILE)
finish: build
	rm $(BUILD_FILE)