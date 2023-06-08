CC = gcc
CFLAGS = -Wall -Wextra

all: poweriteration_2_3 poweriteration poweriteration2 poweriteration3

poweriteration_2_3: poweriteration_2_3.c
	$(CC) $(CFLAGS) -fopenmp -o poweriteration_2_3 poweriteration_2_3.c

poweriteration: poweriteration.c
	$(CC) $(CFLAGS) -o poweriteration poweriteration.c

poweriteration2: poweriteration2.c
	$(CC) $(CFLAGS) -o poweriteration2 poweriteration2.c

poweriteration3: poweriteration3.c
	$(CC) $(CFLAGS) -fopenmp -o poweriteration3 poweriteration3.c

clean:
	rm -f poweriteration_2_3 poweriteration poweriteration2 poweriteration3
