CFLAGS = -Wall -Werror

HEADERS = char_list_list.h
OBJECTS = wpa2pg.o char_list_list.o

wpa2pg: $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $@

debug: CFLAGS += -DDEBUG -g
debug: clean wpa2pg

release: CFLAGS += -O2
release: clean wpa2pg

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS)
	rm -f wpa2pg

.PHONY: clean debug release
