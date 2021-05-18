.PHONY: build
build:
	cmake -S . -B build
	cmake --build build
install: build
	cmake --install build
clean:
	rm -f -r build
	rm -f -r gui
test: build
	python3 run_tests.py
resources:
	python3 embed_resource.py
	make build
