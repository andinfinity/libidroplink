lib_LIBRARIES = libidroplink.a
libidroplink_a_SOURCES = src/libidroplink.c src/utils.c src/vendor/cJSON.c

if DARWIN
libs:="libcurl"
else
libs:="libcurl librt"
endif

test:
	gcc `pkg-config --libs --cflags libcurl` -lm -lrt $(libidroplink_a_SOURCES) tests/minunit.h tests/libidroplink_tests.c && ./a.out && rm a.out

.PHONY: test
