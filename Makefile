INCLUDE = -Iinclude -I/usr/include -I/usr/include/freetype2
LIB = -Llib -L/usr/lib/i386-linux-gnu -L/usr/lib
LINKER = $(OPENGL)
OPENGL = -lglfw -lGL -lfreetype -lX11 -lpthread -lXrandr -lXi -ldl

debug:
	g++ src/*.cpp src/renderer/glad.c $(INCLUDE) $(LIB) $(LINKER) -o main.out && ./main.out