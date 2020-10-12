#ifndef _ELF32_H_
#define _ELF32_H_

#include "x86playground/type.h"

#define EI_NIDENT 16

typedef struct {
	u8_t e_ident[EI_NIDENT];
	u16_t e_type;
	u16_t e_machine;
	u32_t e_version;
	u32_t e_entry;
	u32_t e_phoff;
	u32_t e_shoff;
	u32_t e_flags;
	u16_t e_ehsize;
	u16_t e_phentsize;
	u16_t e_phnum;
	u16_t e_shentsize;
	u16_t e_shnum;
	u16_t e_shstrndx;
} Elf32_Ehdr;

typedef struct {
	u32_t p_type;
	u32_t p_offset;
	u32_t p_vaddr;
	u32_t p_paddr;
	u32_t p_filesz;
	u32_t p_memsz;
	u32_t p_flags;
	u32_t p_align;
} Elf32_Phdr;

#endif
