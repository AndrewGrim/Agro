INCLUDE = -Iinclude -I/usr/include -I/usr/include/freetype2
LIB = -Llib -L/usr/lib/i386-linux-gnu -L/usr/lib
LINKER = $(OPENGL)
OPENGL = -lGL -lSDL2 -lfreetype -lX11 -lpthread -lXrandr -lXi -ldl

debug:
	g++ src/main.cpp src/app.cpp src/renderer/glad.c src/renderer/*.cpp src/controls/*.cpp $(INCLUDE) $(LIB) $(LINKER) -o main.out && ./main.out
test_box_vertical:
	g++ test/box_vertical.cpp src/app.cpp src/renderer/glad.c src/renderer/*.cpp src/controls/*.cpp $(INCLUDE) $(LIB) $(LINKER) -o box_vertical.out && ./box_vertical.out
test_box_horizontal:
	g++ test/box_horizontal.cpp src/app.cpp src/renderer/glad.c src/renderer/*.cpp src/controls/*.cpp $(INCLUDE) $(LIB) $(LINKER) -o box_horizontal.out && ./box_horizontal.out