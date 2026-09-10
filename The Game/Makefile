CC = gcc
CFLAGS = $(shell pkg-config --cflags allegro-5 allegro_font-5 allegro_ttf-5 allegro_image-5 allegro_primitives-5) -g -Wall -Wextra
LIBS = $(shell pkg-config --libs allegro-5 allegro_main-5 allegro_font-5 allegro_ttf-5 allegro_image-5 allegro_primitives-5) -lm

TARGET = TheGame
SRCS = TheGame.c Player.c Background.c Platform.c Menu.c Enemy.c Ladder.c Pause.c

$(TARGET): $(SRCS)
	$(CC) $(SRCS) -o $(TARGET) $(CFLAGS) $(LIBS)

clean:
	rm -f $(TARGET)