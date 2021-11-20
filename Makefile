.PHONY: debug release install clean test resources
debug:
	cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DBUILD_EXAMPLE_WIDGET_GALLERY=ON
	cp build/compile_commands.json .
	cmake --build build
release:
	cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DBUILD_ALL_TESTS=ON -DBUILD_ALL_EXAMPLES=ON
	cp build/compile_commands.json .
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
