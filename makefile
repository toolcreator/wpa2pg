CFLAGS = -O3 -Wall -Werror

wpa2pg: wpa2pg.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm wpa2pg

.PHONY: clean
