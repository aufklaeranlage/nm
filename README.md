# nm

## Symbols

    "A" symbol is absolute

    st_shndx == SHN_ABS

    "B"
    "b" The symbol is in the BSS data section

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

    "n" The symbol is in the read-only data section.

    "R"
    "r" The symbol is in a read only data section.

    "S"
    "s" The symbol is in an uninitialized or zero-initialized data section for
        for small objects.

    "T"
    "t" The symbol is in the text (code) section.

    "U" The symbol is undefined.

    "V"
    "v" The symbol is a weak object.

    "W"
    "w" The symbol si a weak symbol that has not been specifically tagged as a
        weak object symbol.

        On some system, uppercase inidcates that a default value has been
        specified.

    "-" The symbol is a stabs symbol in an a.out object file. In this case the,
        next values printed are the stabs other field, the stabs desc field,
        and the stab type. Stabs symbols are used to hold debugging information.

    "?" The symbol type is unkown, or object file format specific.
