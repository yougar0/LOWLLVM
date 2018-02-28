CXX_FLAG := -fPIC -std=c++11 -fno-rtti
CLEAN_FILE := flatten.o crypto_util.o util.o split.o libtrob.so
BUILD_FILE := flatten.o crypto_util.o util.o split.o
SOURCE_FILE := flatten.cc crypto_util.cc util.cc split.cc

build:
	clang++ -c $(SOURCE_FILE) $(CXX_FLAG)
	clang++ trob.cc $(BUILD_FILE) -shared -o libtrob.so $(CXX_FLAG)

clean:
	rm $(CLEAN_FILE)
finish: build
	rm $(BUILD_FILE)