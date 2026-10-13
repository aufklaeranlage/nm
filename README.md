# nm

## Symbols

    "A" symbol is absolute

    st_shndx == SHN_ABS

    "B"
    "b" The symbol is in the BSS data section

    section name is '.bss'

    "C"
    "c" The symbol is common. The lower case is used when the symbol is in a
        special section for small commons.

    "D"
    "d" The symbol is in the initialized data section.

    "G"
    "g" The symbol is in the initialized data section for small objects.

    "i" For PE format filest this indicates that the symbol is in a section
        specific to the implementation of DLLs.

    GNU specific extension to the standard set of ELF symbol tabels.

    "I" The symbol is an indirect reference ot another symbol.

    "N" The symbol is a debuggging symbol.

    section name starts with '.debug', '.stab' or '.stabstr'

    "n" The symbol is in the read-only data section.

    "R"
    "r" The symbol is in a read only data section.

    "S"
    "s" The symbol is in an uninitialized or zero-initialized data section for
        for small objects.

    "T"
    "t" The symbol is in the text (code) section.

        section name is '.text'

    "U" The symbol is undefined.

        symbol table entry `st_shndx` is SHN_UNDEF

    "V"
    "v" The symbol is a weak object.

        symbol table entry `st_info` with ELFN_ST_BIND is STB_WEAK

    "W"
    "w" The symbol is a weak symbol that has not been specifically tagged as a
        weak object symbol.

        On some system, uppercase inidcates that a default value has been
        specified.

    "-" The symbol is a stabs symbol in an a.out object file. In this case the,
        next values printed are the stabs other field, the stabs desc field,
        and the stab type. Stabs symbols are used to hold debugging information.

    "?" The symbol type is unkown, or object file format specific.

Section name based symbols:

    'b', 't', 
