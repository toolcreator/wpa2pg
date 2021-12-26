CFLAGS = -Wall -Werror -Iinc

HEADERS = $(addsuffix .h,$(addprefix inc/, \
	char_list_list \
))

OBJECTS = $(addsuffix .o,$(addprefix src/, \
	wpa2pg \
	char_list_list \
))

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
