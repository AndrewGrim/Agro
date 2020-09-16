debug:
	g++ src/*.cpp -I/usr/include -Iinclude -L/usr/lib/i386-linux-gnu -Llib -lSDL2 -o gui.out && ./gui.out