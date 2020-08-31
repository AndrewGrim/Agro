debug:
	g++ src/*.cpp -I/usr/include -L/usr/lib/i386-linux-gnu -lSDL2-2.0 -o gui.out && ./gui.out