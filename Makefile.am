lib_LIBRARIES = libidroplink.a
libidroplink_a_SOURCES = src/libidroplink.c src/utils.c src/vendor/cJSON.c
libidroplink_test_SOURCES = tests/minunit.h tests/libidroplink_tests.c

if DARWIN
libs:="libcurl"
else
libs:="libcurl librt"
endif

test:
	gcc $(libidroplink_a_SOURCES) $(libidroplink_test_SOURCES) \
		`pkg-config --libs --cflags libcurl` -lm && ./a.out && rm a.out

.PHONY: test
