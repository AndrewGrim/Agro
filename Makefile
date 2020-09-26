INCLUDE = -Iinclude -I/usr/include -I/usr/include/freetype2
LIB = -Llib -L/usr/lib/i386-linux-gnu -L/usr/lib
LINKER = $(OPENGL)
OPENGL = -lGL -lSDL2 -lfreetype -lX11 -lpthread -lXrandr -lXi -ldl

debug:
	g++ src/*.cpp src/renderer/glad.c src/renderer/*.cpp $(INCLUDE) $(LIB) $(LINKER) -o main.out && ./main.out