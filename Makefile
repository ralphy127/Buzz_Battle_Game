build:
	gcc -Wall `pkg-config --cflags sdl2 SDL2_image SDL2_ttf` ./src/*.c -o ./bin/buzz_battle `pkg-config --libs sdl2 SDL2_image SDL2_ttf SDL2_mixer` -lm
run:
	./bin/buzz_battle
clean:
	rm ./bin/buzz_battle
