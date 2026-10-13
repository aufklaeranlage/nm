#include "nm.h"
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

static int validate_elf_magic(char *ident)
{
	return (ident[EI_MAG0] == ELFMAG0
			&& ident[EI_MAG1] == ELFMAG1
			&& ident[EI_MAG2] == ELFMAG2
			&& ident[EI_MAG3] == ELFMAG3);
}

int main(int ac, char *av[])
{
	char *filename = "a.out";
	int ret = 1, opt;
	struct nm_args args = {};
	while ((opt = getopt(ac, av, "agupr")) != -1) {
		switch (opt) {
			case 'a' : args.debug = 1; break ;
			case 'g' : args.only_extern = 1; break ;
			case 'u' : args.only_undefined = 1; break ;
			case 'p' : args.no_sort = 1; break ;
			case 'r' : args.reverse = 1; break ;
			default: fprintf(stderr, "Usage: %s [option(s)] [file(s)]\n\
  List symbols in [file(s)] (a.out by default).\n\
	The options are:\n\
	-a	Display debugger-only symbols\n\
	-g	Display only extern symbols\n\
	-u	Display only undefined symbols\n\
	-p	Do not sort the symbols\n\
	-r	Reverse the sense of the sort\n", av[0]);
							 return 1;
		}
	}

	int pos = optind, nfiles = ac - pos;
	if (!nfiles)
		--pos;
	ret = 0;
	for (;pos < ac; pos++) {
		if (nfiles > 0)
			filename = av[pos];
		char ident[EI_NIDENT];
		FILE *file = fopen(filename, "r");
		if (!file || ferror(file)) {
			fprintf(stderr, "%s: %s: %s\n", av[0], filename, strerror(errno));
			continue ;
		}
		if (fread(ident, sizeof(char), EI_NIDENT, file) != EI_NIDENT
				|| ferror(file)) {
			fprintf(stderr, "%s: %s: couldn't read file\n", av[0], filename);
			++ret;
			goto close_file_continue;
		}
		fseek(file, 0, SEEK_SET);
		if (!validate_elf_magic(ident)) {
			fprintf(stderr, "%s: %s: file format not recognized\n", av[0], filename);
			++ret;
			goto close_file_continue;
		}
		if (ident[EI_CLASS] == ELFCLASS64) {
			if (nfiles > 1)
				fprintf(stdout, "\n%s\n", filename);
			elf64(file, &args);
		} else if (ident[EI_CLASS] == ELFCLASS32) {
			if (nfiles > 1)
				fprintf(stdout, "\n%s\n", filename);
			elf32(file, &args);
		} else {
			fprintf(stderr, "%s: %s: file format not recognized\n", av[0], filename);
			++ret;
		}
close_file_continue:
		fclose(file);
	}
	return ret;
}
