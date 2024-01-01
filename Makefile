all:
	cmake -B build -S .
	cmake --build build

clean:
	rm -rf build

fresh:
	make clean
	make all
