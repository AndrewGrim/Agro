INCLUDE = -Iinclude -I/usr/include -I/usr/include/freetype2
LINK = -Llib -L/usr/lib/i386-linux-gnu -L/usr/lib -Lbuild
LIBS = -lGL -lSDL2 -lfreetype -lX11 -lpthread -lXrandr -lXi -ldl
CXX = g++
CXX_FLAGS = -fno-exceptions -fPIC
LIB_NAME = gui
# TODO add a release version of cxx flags
# which also disables asserts
OBJECT_FILES = \
	build/application.o \
	build/style.o \
	build/box.o \
	build/button.o \
	build/image.o \
	build/label.o \
	build/lineedit.o \
	build/notebook.o \
	build/scrollable.o \
	build/scrollbar.o \
	build/scrolledbox.o \
	build/slider.o \
	build/spacer.o \
	build/splitter.o \
	build/treeview.o \
	build/widget.o \
	build/window.o \
	build/drawing_context.o \
	build/font.o \
	build/glad.o \
	build/renderer.o \
	build/stb_image.o
OUT = main.out

run: local_build
	./$(OUT)
install: build/lib$(LIB_NAME).so
	cp src/*.hpp -t build/$(LIB_NAME)
	cp src/controls/*.hpp -t build/$(LIB_NAME)/controls
	cp src/common/*.hpp -t build/$(LIB_NAME)/common
	cp src/renderer/*.hpp -t build/$(LIB_NAME)/renderer
	sudo cp build/lib$(LIB_NAME).so /usr/lib
	sudo cp -r build/$(LIB_NAME) /usr/include
	sudo ldconfig

# BUILD DIRECTORY
dir:
	mkdir -p build
	mkdir -p build/$(LIB_NAME)
	mkdir -p build/$(LIB_NAME)/controls
	mkdir -p build/$(LIB_NAME)/common
	mkdir -p build/$(LIB_NAME)/renderer

# BUILD lib and main
local_build: dir build/main.o $(OBJECT_FILES)
	$(CXX) build/main.o $(OBJECT_FILES) $(LINK) $(LIBS) -o $(OUT)
build: install build/main.o
	$(CXX) build/main.o $(LINK) $(LIBS) -l$(LIB_NAME) -o $(OUT)
build/libgui.so: dir $(OBJECT_FILES)
	$(CXX) $(OBJECT_FILES) $(LINK) $(LIBS) -shared -o build/libgui.so

# ROOT
build/main.o: src/main.cpp $(OBJECT_FILES)
	$(CXX) -c src/main.cpp $(INCLUDE) $(CXX_FLAGS) -o $@
build/application.o: build/window.o src/application.cpp src/application.hpp
	$(CXX) -c src/application.cpp $(INCLUDE) $(CXX_FLAGS) -o $@
build/window.o: build/widget.o build/style.o build/drawing_context.o build/font.o src/window.cpp src/window.hpp
	$(CXX) -c src/window.cpp $(INCLUDE) $(CXX_FLAGS) -o $@

# COMMON
build/style.o: src/common/style.cpp src/common/style.hpp
	$(CXX) -c src/common/style.cpp $(INCLUDE) $(CXX_FLAGS) -o $@

# CONTROLS
build/box.o: build/widget.o src/controls/box.cpp src/controls/box.hpp
	$(CXX) -c src/controls/box.cpp $(INCLUDE) $(CXX_FLAGS) -o $@
build/button.o: build/widget.o src/controls/button.cpp src/controls/button.hpp
	$(CXX) -c src/controls/button.cpp $(INCLUDE) $(CXX_FLAGS) -o $@
build/image.o: build/widget.o src/controls/image.cpp src/controls/image.hpp src/renderer/texture.hpp
	$(CXX) -c src/controls/image.cpp $(INCLUDE) $(CXX_FLAGS) -o $@
build/label.o: build/widget.o src/controls/label.cpp src/controls/label.hpp
	$(CXX) -c src/controls/label.cpp $(INCLUDE) $(CXX_FLAGS) -o $@
build/lineedit.o: build/widget.o src/controls/lineedit.cpp src/controls/lineedit.hpp
	$(CXX) -c src/controls/lineedit.cpp $(INCLUDE) $(CXX_FLAGS) -o $@
build/notebook.o: build/button.o src/controls/notebook.cpp src/controls/notebook.hpp
	$(CXX) -c src/controls/notebook.cpp $(INCLUDE) $(CXX_FLAGS) -o $@
build/scrollable.o: build/scrollbar.o src/controls/scrollable.cpp src/controls/scrollable.hpp
	$(CXX) -c src/controls/scrollable.cpp $(INCLUDE) $(CXX_FLAGS) -o $@
build/scrollbar.o: build/slider.o build/button.o src/controls/scrollbar.cpp src/controls/scrollbar.hpp
	$(CXX) -c src/controls/scrollbar.cpp $(INCLUDE) $(CXX_FLAGS) -o $@
build/scrolledbox.o: build/scrollable.o src/controls/scrolledbox.cpp src/controls/scrolledbox.hpp
	$(CXX) -c src/controls/scrolledbox.cpp $(INCLUDE) $(CXX_FLAGS) -o $@
build/slider.o: build/box.o src/controls/slider.cpp src/controls/slider.hpp
	$(CXX) -c src/controls/slider.cpp $(INCLUDE) $(CXX_FLAGS) -o $@
build/spacer.o: build/widget.o src/controls/spacer.cpp src/controls/spacer.hpp
	$(CXX) -c src/controls/spacer.cpp $(INCLUDE) $(CXX_FLAGS) -o $@
build/splitter.o: build/widget.o src/controls/splitter.cpp src/controls/splitter.hpp
	$(CXX) -c src/controls/splitter.cpp $(INCLUDE) $(CXX_FLAGS) -o $@
build/treeview.o: build/scrollable.o src/controls/treeview.cpp src/controls/treeview.hpp
	$(CXX) -c src/controls/treeview.cpp $(INCLUDE) $(CXX_FLAGS) -o $@
build/widget.o: build/style.o build/drawing_context.o build/font.o src/controls/widget.cpp src/controls/widget.hpp
	$(CXX) -c src/controls/widget.cpp $(INCLUDE) $(CXX_FLAGS) -o $@

# RENDERER
build/drawing_context.o: build/style.o build/font.o build/renderer.o src/renderer/drawing_context.cpp src/renderer/drawing_context.hpp
	$(CXX) -c src/renderer/drawing_context.cpp $(INCLUDE) $(CXX_FLAGS) -o $@
build/font.o: src/renderer/font.cpp src/renderer/font.hpp
	$(CXX) -c src/renderer/font.cpp $(INCLUDE) $(CXX_FLAGS) -o $@
build/glad.o: src/renderer/glad.c include/glad/glad.h
	$(CXX) -c src/renderer/glad.c $(INCLUDE) $(CXX_FLAGS) -o $@
build/renderer.o: build/font.o src/renderer/renderer.cpp src/renderer/renderer.hpp src/renderer/batch.hpp
	$(CXX) -c src/renderer/renderer.cpp $(INCLUDE) $(CXX_FLAGS) -o $@
build/stb_image.o: src/renderer/stb_image.cpp src/renderer/stb_image.hpp
	$(CXX) -c src/renderer/stb_image.cpp $(INCLUDE) $(CXX_FLAGS) -o $@

clean:
	rm -f -r build

test:
	python3 run_tests.py

test_scrolledbox_outer:
	$(CXX) tests/scrolledbox_outer.cpp $(OBJECT_FILES) $(INCLUDE) $(LINK) $(LIBS) $(CXX_FLAGS) -o tests/scrolledbox_outer.out && ./tests/scrolledbox_outer.out
test_scrolledbox_inner:
	$(CXX) tests/scrolledbox_inner.cpp $(OBJECT_FILES) $(INCLUDE) $(LINK) $(LIBS) $(CXX_FLAGS) -o tests/scrolledbox_inner.out && ./tests/scrolledbox_inner.out
test_scrolledbox_both:
	$(CXX) tests/scrolledbox_both.cpp $(OBJECT_FILES) $(INCLUDE) $(LINK) $(LIBS) $(CXX_FLAGS) -o tests/scrolledbox_both.out && ./tests/scrolledbox_both.out
test_scrolledbox_inception_clipping:
	$(CXX) tests/scrolledbox_inception_clipping.cpp $(OBJECT_FILES) $(INCLUDE) $(LINK) $(LIBS) $(CXX_FLAGS) -o tests/scrolledbox_inception_clipping.out && ./tests/scrolledbox_inception_clipping.out
test_one_million_labels:
	$(CXX) tests/one_million_labels.cpp $(OBJECT_FILES) $(INCLUDE) $(LINK) $(LIBS) $(CXX_FLAGS) -o tests/one_million_labels.out && ./tests/one_million_labels.out
test_notebook:
	$(CXX) tests/notebook.cpp $(OBJECT_FILES) $(INCLUDE) $(LINK) $(LIBS) $(CXX_FLAGS) -o tests/notebook.out && ./tests/notebook.out

example_box:
	$(CXX) examples/box.cpp $(OBJECT_FILES) $(INCLUDE) $(LINK) $(LIBS) $(CXX_FLAGS) -o examples/box.out && ./examples/box.out
example_tooltips:
	$(CXX) examples/tooltips.cpp $(OBJECT_FILES) $(INCLUDE) $(LINK) $(LIBS) $(CXX_FLAGS) -o examples/tooltips.out && ./examples/tooltips.out
example_image:
	$(CXX) examples/image.cpp $(OBJECT_FILES) $(INCLUDE) $(LINK) $(LIBS) $(CXX_FLAGS) -o examples/image.out && ./examples/image.out
example_lineedit:
	$(CXX) examples/lineedit.cpp $(OBJECT_FILES) $(INCLUDE) $(LINK) $(LIBS) $(CXX_FLAGS) -o examples/lineedit.out && ./examples/lineedit.out
example_keybindings:
	$(CXX) examples/keybindings.cpp $(OBJECT_FILES) $(INCLUDE) $(LINK) $(LIBS) $(CXX_FLAGS) -o examples/keybindings.out && ./examples/keybindings.out
example_callbacks:
	$(CXX) examples/callbacks.cpp $(OBJECT_FILES) $(INCLUDE) $(LINK) $(LIBS) $(CXX_FLAGS) -o examples/callbacks.out && ./examples/callbacks.out
example_custom_widget_styling:
	$(CXX) examples/custom_widget_styling.cpp $(OBJECT_FILES) $(INCLUDE) $(LINK) $(LIBS) $(CXX_FLAGS) -o examples/custom_widget_styling.out && ./examples/custom_widget_styling.out
example_custom_widget_drawing:
	$(CXX) examples/custom_widget_drawing.cpp $(OBJECT_FILES) $(INCLUDE) $(LINK) $(LIBS) $(CXX_FLAGS) -o examples/custom_widget_drawing.out && ./examples/custom_widget_drawing.out
example_treeview: install
	cd examples/treeview && make