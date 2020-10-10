# wpa2pg

WPA2 password generator.

- each charachter is drawn randomly from the set of allowed characters
- random number generation is cryptographically secure by using [`getrandom`](https://man7.org/linux/man-pages/man2/getrandom.2.html)
- linux only (for now)

## prerequisites

- [make](https://www.gnu.org/software/make/)
- a working C compiler (e.g., [gcc](https://gcc.gnu.org/))

## build

```
make wpa2pg
```

## example

```
$ make wpa2pg
cc -O3 -Wall -o wpa2pg wpa2pg.c
$ ./wpa2pg
2jF/?g.z*U&tYakCWR/-,*^6o9mnGHkX^$BnOt~0FWc+_y6-=OAv$5pcyH@L~!T
$ ./wpa2pg 8
t~r_8#Uz
```
