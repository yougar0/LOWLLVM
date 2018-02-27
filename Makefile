build:
	clang++ -fPIC -c flatten.cc -std=c++11 -fno-rtti
	clang++ -fPIC -c crypto_util.cc -std=c++11 -fno-rtti
	clang++ -fPIC -c util.cc -std=c++11 -fno-rtti
	clang++ trob.cc flatten.o crypto_util.o util.o -fPIC -shared -o libtrob.so -std=c++11 -fno-rtti

clean:
	rm flatten.o
	rm crypto_util.o
	rm util.o
	rm libtrob.so
finish: build
	rm flatten.o
	rm crypto_util.o
	rm util.o
	mv libtrob.so test/