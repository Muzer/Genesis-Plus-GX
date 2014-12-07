/*
  drz80 -> z80.
*/

#include <3ds.h>
#include "osd.h"
#include "z80.h"
#include "drz80.h"


unsigned char *z80_readmap[64];
unsigned char *z80_writemap[64];
void (*z80_writemem)(unsigned int address, unsigned char data);
unsigned char (*z80_readmem)(unsigned int address);
void (*z80_writeport)(unsigned int port, unsigned char data);
unsigned char (*z80_readport)(unsigned int port);
Z80_Regs Z80;

struct DrZ80 z80_state;


/** STUBS **/
void z80_write8_stub(unsigned char d, unsigned short a) {
    z80_writemem(a, d);
}
void z80_write16_stub(unsigned short d, unsigned short a) {
    z80_writemem(a, d);
    z80_writemem(a+1, d>>8);
}
unsigned char z80_in_stub(unsigned short p) {
    DBG("z80_readport %x", p+0);
    return z80_readport(p);
}
void z80_out_stub(unsigned short p,unsigned char d) {
    DBG("z80_writeport %x", p+0);
    z80_writeport(p, d);
}
unsigned char z80_read8_stub(unsigned short a) {
    return z80_readmem(a);
}
unsigned short z80_read16_stub(unsigned short a) {
    return z80_readmem(a) || (z80_readmem(a+1)<<8);
}
void irq_callback_stub() {
    (void) 0;
}
unsigned int z80_rebaseSP_stub(unsigned short new_sp) {
    DBG("z80_rebasesp %x, base0=%x", new_sp, z80_readmap[0]);
    z80_state.Z80SP_BASE = (unsigned int) z80_readmap[new_sp>>10];

    return z80_state.Z80SP_BASE;
}
unsigned int z80_rebasePC_stub(unsigned short new_pc) {
    DBG("z80_rebasepc %x", new_pc);
    z80_state.Z80PC_BASE = (unsigned int) z80_readmap[new_pc>>10];

    return z80_state.Z80PC_BASE;
}


/** IMPLEMENTATION **/
void z80_init(const void *config, int (*irqcallback)(int)) {
    (void) config;
    (void) irqcallback;

    DBG("z80_init");

    memset(&z80_state, 0, sizeof(z80_state));

    z80_state.z80_write8       = z80_write8_stub;
    z80_state.z80_write16      = z80_write16_stub;
    z80_state.z80_in           = z80_in_stub;
    z80_state.z80_out          = z80_out_stub;
    z80_state.z80_irq_callback = &irq_callback_stub;
    z80_state.z80_read8        = z80_read8_stub;
    z80_state.z80_read16       = z80_read16_stub;
    z80_state.z80_rebaseSP     = z80_rebaseSP_stub;
    z80_state.z80_rebasePC     = z80_rebasePC_stub;

    z80_reset();
}

void z80_reset() {
    DBG("z80_reset");

    z80_state.Z80I = 0;
    z80_state.Z80IM = 0;
    z80_state.Z80IF = 0;

    z80_state.z80irqvector = 0xff0000; // RST 38h
    z80_state.Z80PC_BASE = z80_readmap[0];
    z80_state.Z80PC = z80_readmap[0];

    z80_state.Z80SP_BASE = z80_readmap[0];

    /*
    drz80_sp_base = (PicoAHW & PAHW_SMS) ? 0xc000 : 0x0000;
    drZ80.Z80SP_BASE = z80_read_map[drz80_sp_base >> Z80_MEM_SHIFT] << 1;
    if (PicoAHW & PAHW_SMS)
        drZ80.Z80SP = drZ80.Z80SP_BASE + 0xdff0; // simulate BIOS
    */
}

void z80_run(unsigned int cycles) {
    DBG("z80_run: %x, %x", cycles, cycles - Z80.cycles);

    if(cycles > Z80.cycles) {
        unsigned int min = (cycles - Z80.cycles) / 15;

        if(min > 0) {
            unsigned int extra = DrZ80Run(&z80_state, min);
            DBG("extra: %x", extra);

            Z80.cycles = (min + extra) * 15;
        }
    }
}

void z80_get_context (void *dst) {
    (void) 0;
}

void z80_set_context (void *src) {
    (void) 0;
}

void z80_set_irq_line(unsigned int state) {
    DBG("z80_irq: %x", state);
    z80_state.Z80_IRQ = !!state;
}

void z80_set_nmi_line(unsigned int state) {
    DBG("z80_nmi: %x", state);
    if(state)
        z80_state.Z80IF |= 8;
    else
        z80_state.Z80IF &= ~8;
}
