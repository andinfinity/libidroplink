#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

char *join_url(char *comp, ...)
{
	va_list argp;
	char **args;
	char *out, *temp;
	int i = 0, argc = 0, strsize = 0, size = 0;

	args = malloc(sizeof(char *));

	va_start(argp, comp);

	while (comp != NULL) {
		/* resize array */
		/* +1 for '\0' */
		args = realloc(args, (argc + 1) * sizeof(char *));

		if (args == NULL) {
			fprintf(stderr, "Ran out of memory.\n");
		}

		/* remove slash at the beginning if any */
		if (comp[0] == '/') {
			strsize = strlen(comp) - 1;
			comp++;
		}

		/* be sure to have a slash as the last char */
		if (comp[strlen(comp) - 1] != '/') {
			temp = malloc((strlen(comp)) * sizeof(char));
			strncpy(temp, comp, strlen(comp) + 1);
			comp = strncat(temp, "/", 1);
		}

		args[argc] = malloc((strlen(comp) + 1) * sizeof(char));

		if (args[argc] == NULL) {
			fprintf(stderr, "Ran out of memory.\n");
		}

		strcpy(args[argc], comp);

		size += strlen(comp);
		comp = va_arg(argp, char *);
		argc++;
	}

	out = malloc((size + 1) * sizeof(char));

	while (i < argc) {
		strcat(out, args[i]);
		i++;
	}

	va_end(argp);
	return out;
}

/* TESTING
int main(int argc, char **argv)
{
    printf("%s\n", join_url("http://a.b/", "/bar", "/comp", NULL));
    printf("%s\n", join_url("http://a.b/", "/bar/", "/foo/", NULL));
}
*/
