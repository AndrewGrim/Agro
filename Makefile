INCLUDE = -Iinclude -I/usr/include -I/usr/include/freetype2
LIB = -Llib -L/usr/lib/i386-linux-gnu -L/usr/lib
LINKER = $(OPENGL)
OPENGL = -lGL -lSDL2 -lfreetype -lX11 -lpthread -lXrandr -lXi -ldl

debug:
	g++ src/main.cpp src/app.cpp src/renderer/glad.c src/renderer/*.cpp src/controls/*.cpp $(INCLUDE) $(LIB) $(LINKER) -fno-exceptions -o main.out && ./main.out
test:
	python3 run_tests.py
test_box_vertical:
	g++ tests/box_vertical.cpp src/app.cpp src/renderer/glad.c src/renderer/*.cpp src/controls/*.cpp $(INCLUDE) $(LIB) $(LINKER) -o box_vertical.out && ./box_vertical.out
test_box_horizontal:
	g++ tests/box_horizontal.cpp src/app.cpp src/renderer/glad.c src/renderer/*.cpp src/controls/*.cpp $(INCLUDE) $(LIB) $(LINKER) -o box_horizontal.out && ./box_horizontal.out
test_scrolledbox_outer:
	g++ tests/scrolledbox_outer.cpp src/app.cpp src/renderer/glad.c src/renderer/*.cpp src/controls/*.cpp $(INCLUDE) $(LIB) $(LINKER) -o scrolledbox_outer.out && ./scrolledbox_outer.out
test_scrolledbox_inner:
	g++ tests/scrolledbox_inner.cpp src/app.cpp src/renderer/glad.c src/renderer/*.cpp src/controls/*.cpp $(INCLUDE) $(LIB) $(LINKER) -o scrolledbox_inner.out && ./scrolledbox_inner.out
test_scrolledbox_both:
	g++ tests/scrolledbox_both.cpp src/app.cpp src/renderer/glad.c src/renderer/*.cpp src/controls/*.cpp $(INCLUDE) $(LIB) $(LINKER) -o scrolledbox_both.out && ./scrolledbox_both.out
test_scrolledbox_inception_clipping:
	g++ tests/scrolledbox_inception_clipping.cpp src/app.cpp src/renderer/glad.c src/renderer/*.cpp src/controls/*.cpp $(INCLUDE) $(LIB) $(LINKER) -o scrolledbox_inception_clipping.out && ./scrolledbox_inception_clipping.out