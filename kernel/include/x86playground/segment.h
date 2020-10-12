#ifndef _SEGMENT_H_
#define _SEGMENT_H_

#include "x86playground/type.h"

/* GDT */
typedef struct {
	u16_t limit1;
	u16_t base1;
	u8_t base2;
	u8_t attr1;
	u8_t attr2_limit2;
	u8_t base3;
} gdesc_t;

#define GDT_SIZE 128

#define SEL_TEXT_G 0x08
#define SEL_DATA_G 0x10
#define SEL_TSS_G 0x18
#define SEL_INIT_G 0x20

#define IDX_TEXT_G 0x01
#define IDX_DATA_G 0x02
#define IDX_TSS_G 0x03
#define IDX_INIT_G 0x04

/* G 0 | D/B 1 | AVL 0 | P 1 | DPL 0 | S 0 system | TYPE 2 LDT */
#define GDT_ATTR_LDT 0x4082

/* G 0 | D/B 1 | AVL 0 | P 1 | DPL 0 | S 0 system | TYPE 9 386 TSS */
#define GDT_ATTR_TSS 0x4089

void init_gdesc (gdesc_t *p_desc, u32_t base, u32_t limit, u16_t attr);
void init_gdt ();

/* IDT */
typedef struct {
	u16_t offset1;
	u16_t selector;
	u16_t attr;
	u16_t offset2;
} gate_t;

#define IDT_SIZE 256

/* p 1
 * DPL 3
 * S 0
 * TYPE E
 */
#define IDT_ATTR_INT_GATE 0xEE00

void init_gate (gate_t *p_gate, u16_t selector, u32_t offset, u16_t attr);
void init_idt ();

/* LDT */
typedef struct {
	u16_t limit1;
	u16_t base1;
	u8_t base2;
	u8_t attr1;
	u8_t attr2_limit2;
	u8_t base3;
} ldesc_t;

#define LDT_SIZE 2

/* TI 1 RPL 3 */
#define SEL_TEXT_L 0x07
#define SEL_DATA_L 0x0F

#define LDT_ATTR_TEXT 0xC0F8
#define LDT_ATTR_DATA 0xC0F2

void init_ldesc (ldesc_t *p_desc, u32_t base, u32_t limit, u16_t attr);

/* TSS */

#endif
