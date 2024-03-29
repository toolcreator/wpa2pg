# wpa2pg

WPA2 password generator.

- each charachter is drawn randomly from the set of allowed characters
- uses [`getrandom`](https://man7.org/linux/man-pages/man2/getrandom.2.html)
- linux only (for now)

## prerequisites

- [make](https://www.gnu.org/software/make/)
- a working C compiler (e.g., [gcc](https://gcc.gnu.org/))

## build

```
make wpa2pg
```

## usage

```
$ wpa2pg --help
Usage: wpa2pg [OPTION...] LENGTH
WPA2 password generator

  -a, --exclude-ambiguous    Exclude ambiguous characters
  -c, --exclude-capital      Exclude capital letters
  -e, --exclude-extra=CHAR_STRING
                             More characters to exclude
  -s, --exclude-special      Exclude special characters
  -?, --help                 Give this help list
      --usage                Give a short usage message
  -V, --version              Print program version

Mandatory or optional arguments to long options are also mandatory or optional
for any corresponding short options.

Report bugs to <de.toolcreator@gmail.com>.
```

## example

```
$ make release
rm -f wpa2pg.o char_list_list.o
rm -f wpa2pg
cc -Wall -Werror -O2 -c wpa2pg.c -o wpa2pg.o
cc -Wall -Werror -O2 -c char_list_list.c -o char_list_list.o
cc -Wall -Werror -O2 wpa2pg.o char_list_list.o -o wpa2pg
$ ./wpa2pg
m7GIZ8$#XB_3TE,Rvr&l7qk_g7hOr,HfOMBIp*y,KJWMhJ#!B/FlMk,x2$GRx4U
$ ./wpa2pg -as 8
4NePv9kD
```
