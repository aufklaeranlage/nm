#include "nm.h"
#include "utils.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

static const Elf32_Shdr null_sym = { };

static int make_strtab(char **dest, FILE *file, Elf32_Shdr *sht, unsigned int idx)
{
	if (sht[idx].sh_type != SHT_STRTAB)
		return 0;
	*dest = malloc(sht[idx].sh_size);
	if (!*dest)
		return -1;
	fseek(file, sht[idx].sh_offset, SEEK_SET);
	fread(*dest, sizeof(**dest), sht[idx].sh_size, file); return 1;
}

struct strtabs {
	unsigned int nelem;
	char **strtabs;
};

static void free_strtabs(struct strtabs *strtabs)
{
	for (unsigned int i = 0; i < strtabs->nelem; i++)
		free(strtabs->strtabs[i]);
	free(strtabs->strtabs);
}

static int make_strtabs(struct strtabs *dest, FILE *file, Elf32_Shdr *sht, unsigned int shnum)
{
	dest->strtabs = calloc(sizeof(*(dest->strtabs)), shnum);
	if (!dest->strtabs)
		return -1;
	dest->nelem = shnum;
	for (unsigned int i = 0; i < shnum; i++)
		if (make_strtab(dest->strtabs + i, file, sht, i) < 0)
			return (free_strtabs(dest), -1);
	return 1;
}

struct symtab {
	unsigned int nelem;
	Elf32_Sym *symbols;
	char **names;
};

static void free_symtab(struct symtab *st)
{
	free(st->symbols);
	free(st->names);
}

static int make_symtab(struct symtab *dest, FILE *file, Elf32_Shdr *sht, unsigned int shnum, struct strtabs *strtabs, unsigned int shstrtabidx)
{
	dest->nelem = 0;
	for (unsigned int i = 0; i < shnum; i++) {
		if (sht[i].sh_type != SHT_SYMTAB)
			continue ;
		Elf32_Sym sym;
		fseek(file, sht[i].sh_offset, SEEK_SET);
		fread(&sym, sht[i].sh_entsize, 1, file);
		if (memcmp(&sym, &null_sym , sizeof(sym))) {
			fprintf(stderr, "ft_nm: Invalid symbol table: First entry is not zeroed.\n");
			return -1;
		}
		if (sht[sht[i].sh_link].sh_type != SHT_STRTAB) {
			fprintf(stderr, "ft_nm: Invalid symbol table: sh_link doesn't point to a string table.\n");
			return -1;
		}
		dest->nelem += (sht[i].sh_size / sht[i].sh_entsize) - 1;
	}

	dest->symbols = malloc(sizeof(*(dest->symbols)) * dest->nelem);
	dest->names = malloc(sizeof(*(dest->names)) * dest->nelem);
	if (!dest->symbols || !dest->names)
		return (free_symtab(dest), -1);

	unsigned int pos = 0;
	for (unsigned int i = 0; i < shnum; i++) {
		if (sht[i].sh_type != SHT_SYMTAB)
			continue ;
		unsigned int symnum = sht[i].sh_size / sht[i].sh_entsize;
		fseek(file, sht[i].sh_offset + sht[i].sh_entsize, SEEK_SET);
		fread(dest->symbols + pos, sht[i].sh_entsize, symnum - 1, file);
		for (unsigned int j = 0; j + 1 < symnum; j++) {
			dest->names[pos + j] = ELF64_ST_TYPE(dest->symbols[pos + j].st_info) == STT_SECTION ?
				strtabs->strtabs[shstrtabidx] + sht[dest->symbols[pos + j].st_shndx].sh_name : strtabs->strtabs[sht[i].sh_link] + dest->symbols[pos + j].st_name;
		}
		pos += symnum - 1;
	}
	return 1;
}

static int __qsort_sym_name_cmp(const char *s1, const char *s2)
{
	if (!s1)
		return -1;
	if (!s2)
		return 1;
	while (s1[0] == '.' || s1[0] == '_')
		++s1;
	while (s2[0] == '.' || s2[0] == '_')
		++s2;
	while (*s1) {
		int c = *s1++;
		c -= *s2++;
		if (c)
			return c;
	}
	return 0;
}

static unsigned int __qsort_symtab_divide(struct symtab *symtab, unsigned int left, unsigned int right)
{
	unsigned int i = left, j = right - 1, pivot = right; 

	while (i < j) {
		while (i < j
				&& __qsort_sym_name_cmp(symtab->names[i], symtab->names[pivot]) <= 0)
			++i;
		while (j > i
				&& __qsort_sym_name_cmp(symtab->names[j], symtab->names[pivot]) > 0)
			--j;

		if (__qsort_sym_name_cmp(symtab->names[i], symtab->names[j]) > 0) {
			{
				Elf32_Sym tmp = symtab->symbols[i];
				symtab->symbols[i] = symtab->symbols[j];
				symtab->symbols[j] = tmp;
			}
			{
				char *tmp = symtab->names[i];
				symtab->names[i] = symtab->names[j];
				symtab->names[j] = tmp;
			}
		}
	}
	if (__qsort_sym_name_cmp(symtab->names[i], symtab->names[pivot]) > 0) {
		{
			Elf32_Sym tmp = symtab->symbols[i];
			symtab->symbols[i] = symtab->symbols[pivot];
			symtab->symbols[pivot] = tmp;
		}
		{
			char *tmp = symtab->names[i];
			symtab->names[i] = symtab->names[pivot];
			symtab->names[pivot] = tmp;
		}
	} else {
		i = right;
	}
	return i;
}

static void __qsort_symtab(struct symtab *symtab, unsigned int left, unsigned int right)
{
	if (left < right) {
		unsigned int divide = __qsort_symtab_divide(symtab, left, right);
		__qsort_symtab(symtab, left, divide == 0 ? 0 : divide - 1);
		__qsort_symtab(symtab, divide + 1, right);
	}
}

static void sort_symtab(struct symtab *symtab)
{
	__qsort_symtab(symtab, 0, symtab->nelem - 1);
}

static void reverse_symtab(struct symtab *symtab)
{
	for (unsigned int i = 0, j = symtab->nelem - 1; i < j; i++, j--) {
		{
			Elf32_Sym tmp = symtab->symbols[i];
			symtab->symbols[i] = symtab->symbols[j];
			symtab->symbols[j] = tmp;
		}
		{
			char *tmp = symtab->names[i];
			symtab->names[i] = symtab->names[j];
			symtab->names[j] = tmp;
		}
	}
}

int elf32(FILE *file, struct nm_args *args)
{
	int ret = 1;
	Elf32_Ehdr edhr;
	fread(&edhr, sizeof(edhr), 1, file);
	fseek(file, edhr.e_shoff, SEEK_SET);
	Elf32_Shdr *sht = malloc(sizeof(*sht) * edhr.e_shnum);
	if (!sht)
		return ret;
	fread(sht, sizeof(*sht), edhr.e_shnum, file);

	struct strtabs strtabs;
	if (make_strtabs(&strtabs, file, sht, edhr.e_shnum) < 0)
		return (free(sht), ret);

	struct symtab symtab;
	if (make_symtab(&symtab, file, sht, edhr.e_shnum, &strtabs, edhr.e_shstrndx) < 0)
		return (free(sht), free_strtabs(&strtabs), 0);

	if (!args->no_sort)
		sort_symtab(&symtab);

	if (args->reverse)
		reverse_symtab(&symtab);

	for (unsigned int i = 0; i < symtab.nelem; i++) {
		Elf32_Sym *symbol = symtab.symbols + i;
		if (!symbol->st_name && !args->debug)
			continue ;
		char sym = '?';

		switch (symbol->st_shndx) {
			case SHN_ABS: sym = 'a'; break ;
			case SHN_COMMON: sym = 'c'; break ;
			case SHN_UNDEF: sym = 'u'; break ;
			default: switch (sht[symbol->st_shndx].sh_type) {
								 case SHT_PROGBITS:
									 switch (sht[symbol->st_shndx].sh_flags) {
										 case SHF_ALLOC | SHF_EXECINSTR :
										 case SHF_ALLOC | SHF_EXECINSTR | SHF_TLS : sym = 't'; break ;
										 case SHF_ALLOC | SHF_WRITE :
										 case SHF_ALLOC | SHF_WRITE | SHF_TLS : sym = 'd'; break ;
										 case SHF_ALLOC : sym = 'r'; break ;
										 case 0: if (!args->debug) continue ; sym = 'N'; break ;
										 default: if (!(sht[symbol->st_shndx].sh_flags & 0x1)) sym = 'r';
									 } break ;
								 case SHT_NOBITS:
									 switch (sht[symbol->st_shndx].sh_flags) {
										 case SHF_ALLOC | SHF_WRITE :
										 case SHF_ALLOC | SHF_WRITE | SHF_TLS : sym = 'b'; break ;
									 } break ;
							 } break ;
		}

		switch ELF64_ST_TYPE(symbol->st_info) {
			case STT_COMMON: sym = 'c'; break ;
			case STT_FILE:
			case STT_SECTION:
				if (!args->debug) continue ;
				if (!strncmp(".debug", symtab.names[i], 6)) sym = 'N'; break ;
		}

		int local = !(ELF64_ST_BIND(symbol->st_info));
		sym = local ? sym : toupper(sym);
		if (args->only_extern && local)
			continue ;
		if (args->only_undefined && toupper(sym) != 'U')
			continue ;

		if (symbol->st_shndx != SHN_UNDEF)
			printf("%.16x %c %s\n", symbol->st_value, sym, symtab.names[i]); 
		else
			printf("                 %c %s\n", sym, symtab.names[i]); 
	}
	free_symtab(&symtab);
	free_strtabs(&strtabs);
	free(sht);
	return ret;
}
