#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

static int validate_elf_magic(char *ident)
{
	return (ident[EI_MAG0] == ELFMAG0
			&& ident[EI_MAG1] == ELFMAG1
			&& ident[EI_MAG2] == ELFMAG2
			&& ident[EI_MAG3] == ELFMAG3);
}

// static inline int section_is_bss(Elf64_Shdr *sec)
// {
// 	return sec->sh_type == SHT_NOBITS && sec->sh_flags == (SHF_ALLOC | SHF_WRITE);
// }
//
// static inline int section_is_comment(Elf64_Shdr *sec)
// {
// 	return sec->sh_type == SHT_PROGBITS && sec->sh_flags == 0;
// }
//
// static inline int section_is_data(Elf64_Shdr *sec)
// {
// 	return sec->sh_type == SHT_PROGBITS && sec->sh_flags == (SHF_ALLOC | SHF_WRITE);
// }
//
// static inline int section_is_debug(Elf64_Shdr *sec)
// {
// 	return sec->sh_type == SHT_PROGBITS && sec->sh_flags == 0;
// }
//
// static inline int section_is_dynamic(Elf64_Shdr *sec)
// {
// 	return sec->sh_type == SHT_DYNAMIC && sec->sh_flags == 0 /* TODOT */;
// }
//
// static inline int section_is_fini(Elf64_Shdr *sec)
// {
// 	return sec->sh_type == SHT_PROGBITS && sec->sh_flags == 0;
// }

void elf32(FILE *file)
{
	Elf32_Ehdr edhr;
	fread(&edhr, sizeof(edhr), 1, file);
	printf("ELF 32 IDENT: %s\n", edhr.e_ident);
	fseek(file, edhr.e_shoff, SEEK_SET);
	for (Elf32_Half i = 0; i < edhr.e_shnum; i++) {
		Elf32_Shdr shdr;
		fread(&shdr, sizeof(shdr), 1, file);
		if (shdr.sh_type != SHT_SYMTAB)
			continue ;
		printf("%d", shdr.sh_link);
	}
}

enum section_type {
	SRT_TEXT = 0,
	SRT_DATA = 1,
	SRT_RODATA = 2,
	SRT_BSS = 4,
	SRT_DEBUG = 8,
};

static void fill_section_res_table64(enum section_type *srt, const char *strt, const Elf64_Shdr *sht, unsigned int shnum)
{
	for (unsigned int i = 0; i < shnum; i++) {
		const char *name = strt + sht[i].sh_name; 
		if (!strcmp(name, ".text")
				|| !strcmp(name, ".text"))
			srt[i] = SRT_TEXT;
		else if (!strcmp(name, ".data")
				|| !strcmp(name, ".data1")
				|| !strcmp(name, ".tdata1")
				|| !strcmp(name, ".tdata1"))
			srt[i] = SRT_DATA;
		else if (!strcmp(name, ".rodata")
				|| !strcmp(name, ".rodata1"))
			srt[i] = SRT_RODATA;
		else if (!strcmp(name, ".bss")
				|| !strcmp(name, ".tbss"))
			srt[i] = SRT_BSS;
		else if (!strcmp(name, ".debug"))
			srt[i] = SRT_DEBUG;
	}
}

int elf64(FILE *file)
{
	int ret = 1;
	Elf64_Ehdr edhr;
	fread(&edhr, sizeof(edhr), 1, file);
	fseek(file, edhr.e_shoff, SEEK_SET);
	Elf64_Shdr *sht = malloc(sizeof(*sht) * edhr.e_shnum);
	if (!sht)
		goto fail_64;
	fread(sht, sizeof(*sht), edhr.e_shoff, file);
	Elf64_Shdr *strtab_section = sht + edhr.e_shstrndx;
	if (strtab_section->sh_type != SHT_STRTAB)
		goto free_sht_64;
	char *strtab = malloc(sizeof(*strtab) * strtab_section->sh_size);
	if (!strtab)
		goto free_sht_64;
	fseek(file, strtab_section->sh_offset, SEEK_SET);
	fread(strtab, sizeof(*strtab), strtab_section->sh_size, file);
	
	enum section_type *st = calloc(sizeof(*st), edhr.e_shnum);
	if (!st)
		goto free_strtab_64;
	fill_section_res_table64(st, strtab, sht, edhr.e_shnum);

	for (unsigned int i = 0; i < edhr.e_shnum; i++) {
		if (sht[i].sh_type != SHT_SYMTAB)
			continue ;

		strtab_section = sht + sht[i].sh_link;
		if (strtab_section->sh_type != SHT_STRTAB) {
			fprintf(stderr, "ft_nm: misformatted ELF file: No stringtable for symboltable found.\nSkipping...\n");
			continue ;
		}
		char *section_strtab = malloc(strtab_section->sh_size);
		if (!section_strtab)
			break ;
		fseek(file, strtab_section->sh_offset, SEEK_SET);
		fread(section_strtab, sizeof(*section_strtab), strtab_section->sh_size, file);

		unsigned int symnum = sht[i].sh_size / sht[i].sh_entsize;
		Elf64_Sym *symt = malloc(sizeof(*symt) * symnum);
		if (!symt)
			goto free_section_strtab_64;
		fseek(file, sht[i].sh_offset, SEEK_SET);
		fread(symt, sizeof(*symt), symnum, file);
		for (unsigned int j = 0; j < symnum; j++) {
			if (!symt[j].st_name)
				continue ;
			char sym = '?';

			switch (symt[j].st_shndx) {
				case SHN_ABS: sym = 'a'; break ;
				case SHN_COMMON: sym = 'c'; break ;
				case SHN_UNDEF: sym = 'u'; break ;
				default: switch (st[symt[j].st_shndx]) {
									 case SRT_TEXT: sym = 't'; break ;
									 case SRT_DATA: sym = 'd'; break ;
									 case SRT_RODATA: sym = 'r'; break ;
									 case SRT_BSS: sym = 'b'; break ;
									 case SRT_DEBUG: sym = 'N'; break ;
								 } break ;
			}

			switch ELF64_ST_TYPE(symt[j].st_info) {
				case STT_COMMON: sym = 'c'; break ;
				case STT_FUNC: sym = 't'; break ;
				case STT_FILE: continue ;
			}

			int local = !(!(ELF64_ST_BIND(symt[j].st_info) & STB_LOCAL));
			sym = local ? sym : toupper(sym);

			if (symt[j].st_shndx != SHN_UNDEF)
				printf("%.16lx %c %s\n", symt[j].st_value, sym, section_strtab + symt[j].st_name); 
			else
				printf("                 %c %s\n", sym, section_strtab + symt[j].st_name); 
		}
free_section_strtab_64:
		free(section_strtab);
	}
	ret = 1;
	free(st);
free_strtab_64:
	free(strtab);
free_sht_64:
	free(sht);
fail_64:
	return ret;
}

int main(int ac, char *av[])
{
	char *filename = "a.out";
	int ret = 1;

	if (ac > 1)
		filename = av[1];

	char ident[EI_NIDENT];
	FILE *file = fopen(filename, "r");
	if (!file || ferror(file))
		goto exit;
	if (fread(ident, sizeof(char), EI_NIDENT, file) != EI_NIDENT
			||ferror(file))
		goto exit_close;
	fseek(file, 0, SEEK_SET);
	if (!validate_elf_magic(ident))
		goto exit_close;
	if (ident[EI_CLASS] == ELFCLASS32)
		elf32(file);
	else if (ident[EI_CLASS] == ELFCLASS64)
		elf64(file);
	else
		goto exit_close;
	ret = 0;
exit_close:
	fclose(file);
exit:
	return ret;
}
