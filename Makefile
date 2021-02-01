INCLUDE = -Iinclude -I/usr/include -I/usr/include/freetype2
LINK = -Llib -L/usr/lib/i386-linux-gnu -L/usr/lib
LIBS = -lGL -lSDL2 -lfreetype -lX11 -lpthread -lXrandr -lXi -ldl
CXX = g++
CXX_FLAGS = -fno-exceptions
OBJECT_FILES = \
	build/app.o \
	build/box.o \
	build/button.o \
	build/image.o \
	build/label.o \
	build/lineedit.o \
	build/notebook.o \
	build/scrollbar.o \
	build/scrolledbox.o \
	build/simplescrollbar.o \
	build/slider.o \
	build/widget.o \
	build/drawing_context.o \
	build/font.o \
	build/glad.o \
	build/renderer.o \
	build/stb_image.o
OUT = main.out

run: build
	./$(OUT)
build: dir build/main.o $(OBJECT_FILES)
	$(CXX) build/main.o $(OBJECT_FILES) $(LINK) $(LIBS) -o $(OUT)

# BUILD DIRECTORY
dir:
	mkdir -p build

# ROOT
build/main.o: src/main.cpp
	$(CXX) -c src/main.cpp $(INCLUDE) $(CXX_FLAGS) -o $@
build/app.o: src/app.cpp src/app.hpp
	$(CXX) -c src/app.cpp $(INCLUDE) $(CXX_FLAGS) -o $@

# CONTROLS
build/box.o: src/controls/box.cpp src/controls/box.hpp
	$(CXX) -c src/controls/box.cpp $(INCLUDE) $(CXX_FLAGS) -o $@
build/button.o: src/controls/button.cpp src/controls/button.hpp
	$(CXX) -c src/controls/button.cpp $(INCLUDE) $(CXX_FLAGS) -o $@
build/image.o: src/controls/image.cpp src/controls/image.hpp
	$(CXX) -c src/controls/image.cpp $(INCLUDE) $(CXX_FLAGS) -o $@
build/label.o: src/controls/label.cpp src/controls/label.hpp
	$(CXX) -c src/controls/label.cpp $(INCLUDE) $(CXX_FLAGS) -o $@
build/lineedit.o: src/controls/lineedit.cpp src/controls/lineedit.hpp
	$(CXX) -c src/controls/lineedit.cpp $(INCLUDE) $(CXX_FLAGS) -o $@
build/notebook.o: src/controls/notebook.cpp src/controls/notebook.hpp
	$(CXX) -c src/controls/notebook.cpp $(INCLUDE) $(CXX_FLAGS) -o $@
build/scrollbar.o: src/controls/scrollbar.cpp src/controls/scrollbar.hpp
	$(CXX) -c src/controls/scrollbar.cpp $(INCLUDE) $(CXX_FLAGS) -o $@
build/scrolledbox.o: src/controls/scrolledbox.cpp src/controls/scrolledbox.hpp
	$(CXX) -c src/controls/scrolledbox.cpp $(INCLUDE) $(CXX_FLAGS) -o $@
build/simplescrollbar.o: src/controls/simplescrollbar.cpp src/controls/simplescrollbar.hpp
	$(CXX) -c src/controls/simplescrollbar.cpp $(INCLUDE) $(CXX_FLAGS) -o $@
build/slider.o: src/controls/slider.cpp src/controls/slider.hpp
	$(CXX) -c src/controls/slider.cpp $(INCLUDE) $(CXX_FLAGS) -o $@
build/widget.o: src/controls/widget.cpp src/controls/widget.hpp
	$(CXX) -c src/controls/widget.cpp $(INCLUDE) $(CXX_FLAGS) -o $@

# RENDERER
build/drawing_context.o: src/renderer/drawing_context.cpp src/renderer/drawing_context.hpp
	$(CXX) -c src/renderer/drawing_context.cpp $(INCLUDE) $(CXX_FLAGS) -o $@
build/font.o: src/renderer/font.cpp src/renderer/font.hpp
	$(CXX) -c src/renderer/font.cpp $(INCLUDE) $(CXX_FLAGS) -o $@
build/glad.o: src/renderer/glad.c include/glad/glad.h
	$(CXX) -c src/renderer/glad.c $(INCLUDE) $(CXX_FLAGS) -o $@
build/renderer.o: src/renderer/renderer.cpp src/renderer/renderer.hpp
	$(CXX) -c src/renderer/renderer.cpp $(INCLUDE) $(CXX_FLAGS) -o $@
build/stb_image.o: src/renderer/stb_image.cpp src/renderer/stb_image.h
	$(CXX) -c src/renderer/stb_image.cpp $(INCLUDE) $(CXX_FLAGS) -o $@

clean:
	rm -f -r build

test:
	python3 run_tests.py
test_box_vertical:
	$(CXX) tests/box_vertical.cpp $(OBJECT_FILES) $(INCLUDE) $(LINK) $(LIBS) $(CXX_FLAGS) -o box_vertical.out && ./box_vertical.out
test_box_horizontal:
	$(CXX) tests/box_horizontal.cpp $(OBJECT_FILES) $(INCLUDE) $(LINK) $(LIBS) $(CXX_FLAGS) -o box_horizontal.out && ./box_horizontal.out
test_scrolledbox_outer:
	$(CXX) tests/scrolledbox_outer.cpp $(OBJECT_FILES) $(INCLUDE) $(LINK) $(LIBS) $(CXX_FLAGS) -o scrolledbox_outer.out && ./scrolledbox_outer.out
test_scrolledbox_inner:
	$(CXX) tests/scrolledbox_inner.cpp $(OBJECT_FILES) $(INCLUDE) $(LINK) $(LIBS) $(CXX_FLAGS) -o scrolledbox_inner.out && ./scrolledbox_inner.out
test_scrolledbox_both:
	$(CXX) tests/scrolledbox_both.cpp $(OBJECT_FILES) $(INCLUDE) $(LINK) $(LIBS) $(CXX_FLAGS) -o scrolledbox_both.out && ./scrolledbox_both.out
test_scrolledbox_inception_clipping:
	$(CXX) tests/scrolledbox_inception_clipping.cpp $(OBJECT_FILES) $(INCLUDE) $(LINK) $(LIBS) $(CXX_FLAGS) -o scrolledbox_inception_clipping.out && ./scrolledbox_inception_clipping.out
test_lineedit_search:
	$(CXX) tests/lineedit_search.cpp $(OBJECT_FILES) $(INCLUDE) $(LINK) $(LIBS) $(CXX_FLAGS) -o lineedit_search.out && ./lineedit_search.out
test_keybindings:
	$(CXX) tests/keybindings.cpp $(OBJECT_FILES) $(INCLUDE) $(LINK) $(LIBS) $(CXX_FLAGS) -o keybindings.out && ./keybindings.out
test_one_million_labels:
	$(CXX) tests/one_million_labels.cpp $(OBJECT_FILES) $(INCLUDE) $(LINK) $(LIBS) $(CXX_FLAGS) -o one_million_labels.out && ./one_million_labels.out
test_callbacks:
	$(CXX) tests/callbacks.cpp $(OBJECT_FILES) $(INCLUDE) $(LINK) $(LIBS) $(CXX_FLAGS) -o callbacks.out && ./callbacks.out