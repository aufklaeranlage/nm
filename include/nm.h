#ifndef NM_H
# define NM_H

# include <stdio.h>
# include <elf.h>

/*!
 * ELF (Executable and Linking Format) Files are the object file format.
 * They come in three main types:
 * - A *relocatable file* that holds code and data suitable for linking with
 *   other objec files to create an executable or a shared object file.
 * - A *executable file* that holds a program suitable for execution; the file
 *   specifices how exec(BA_OS) creates a program's process image.
 * - A *shared object file* that holds code and data suitable for linking in two
 *   contexts. First, the link editor [see ld(BA_OS)] processes the shared
 *   object file with other relocatable and shared object files to create
 *   another object file. Second, the dynamic linked combines it with an
 *   executable file and other shared object to create a process image.
 * (sorce)[https://refspecs.linuxbase.org/elf/gabi4+/ch4.intro.html]
 *
 * Object files participate in program linking (building a program) and program
 * execution (running a program). Therefor they provide a parallel view of a
 * file's content, reflecting the differing needs of those activities.
 *
 * | Linking View | Execution View |
 * | --- | --- |
 * | ELF Header | ELF Header |
 * | Program header table (optional) | Program header table (required) |
 * | Section 1 | Segment 1 |
 * | ... | Segment 2 |
 * | Section n | Segment 3 |
 * | ... | Segment 4 |
 * | ... | ... |
 * | Section header table (required) | Section header table (optional) |
 *
 * An *ELF Header* resides at the beginning and holds 'road map' describing the
 * file's organization.
 *
 * *Sections* hold the bulk of object file information for the linking view:
 * instructions, data, symblo table, relocation information, and so on.
 *
 * A *program heade table* tells the system how to create a process image. Files
 * used to build a process image (execute a program) must have a program header
 * table.
 *
 * A *section header table* contains information describing the file's sections.
 * Every section has an entry in the table; each entry gives information such as
 * the seciont name, the section size, and so on.
 */

/*!
 * **Data Representation**
 *
 * The object file format supports various processors with 8-bit bytes and
 * either 32-bit or 64-bit architectures. Therefore they represent some control
 * data with a machine-independent format, making it possible to identify
 * object files and interpret their contents in a common way. Remaining data in
 * an object file use the encoding of the target processor, regardless of the
 * machine on which the file was created.
 *
 * **32-bit Data Types**
 *
 * | Name | Size | Alignment | Purpose |
 * | --- | --- | --- | --- |
 * | Elf32_Addr | 4 | 4 | unsigned program address |
 * | Elf32_Off | 4 | 4 | unsigned file offset | 
 * | Elf32_Half | 2 | 2 | unsigned medium integer |
 * | Elf32_Word | 4 | 4 | unsigned integer |
 * | Elf32_Sword | 4 | 4 | singed integer |
 * | unsigned char | 1 | 1 | unsigend small integer |
 *
 * **64-bit Data Types**
 *
 * | Name | Size | Alignment | Purpose |
 * | --- | --- | --- | --- |
 * | Elf64_Addr | 8 | 8 | unsigned program address |
 * | Elf64_Off | 8 | 8 | unsigned file offset | 
 * | Elf64_Half | 2 | 2 | unsigned medium integer |
 * | Elf64_Word | 4 | 4 | unsigned integer |
 * | Elf64_Sword | 4 | 4 | singed integer |
 * | Elf64_Xword | 8 | 8 | unsigned long integer |
 * | Elf64_Sxword | 8 | 8 | signed long integer |
 * | unsigned char | 1 | 1 | unsigend small integer |
 *
 */

struct nm_args {
	unsigned int debug : 1;
	unsigned int only_extern : 1;
	unsigned int only_undefined : 1;
	unsigned int reverse : 1;
	unsigned int no_sort : 1;
};

int elf64(FILE *file, struct nm_args *args);
int elf32(FILE *file, struct nm_args *args);

#endif
