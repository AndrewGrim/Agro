.PHONY: debug release install clean test resources
debug:
	cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
	cmake --build build
release:
	cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
	cmake --build build
install:
	sudo cmake --install build
clean:
	rm -f -r build
	rm -f -r Agro
test:
	python3 run_tests.py
resources:
	python3 embed_resource.py
