CFLAGS = -O3 -Wall

wpa2pg: wpa2pg.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm wpa2pg

.PHONY: clean
