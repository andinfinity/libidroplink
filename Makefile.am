lib_LIBRARIES = libidroplink.a
libidroplink_a_SOURCES = src/libidroplink.c src/utils.c src/vendor/cJSON.c

test:
	gcc -lCURL -lm src/vendor/cJSON.c src/utils.c src/libidroplink.c tests/minunit.h tests/libidroplink_tests.c && ./a.out && rm a.out

.PHONY: test
