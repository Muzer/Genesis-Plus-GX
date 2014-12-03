/*
  drz80 -> z80.
*/

#include <3ds.h>
#include "z80.h"
#include "drz80.h"


unsigned char *z80_readmap[64];
unsigned char *z80_writemap[64];
void (*z80_writemem)(unsigned int address, unsigned char data);
unsigned char (*z80_readmem)(unsigned int address);
void (*z80_writeport)(unsigned int port, unsigned char data);
unsigned char (*z80_readport)(unsigned int port);
//Z80_Regs Z80;


struct DrZ80 z80_state;
#if 0
#define DBG(x ...) do {                                 \
        char buf[1024];                                 \
        sprintf(buf, x);                                \
        svcOutputDebugString(buf, strlen(buf)+1);       \
    } while (0)
#else
#define DBG(x ...) \
    (void) 0
#endif

/** STUBS **/
void z80_write8_stub(unsigned char d, unsigned short a) {
    z80_writemem(a, d);
}
void z80_write16_stub(unsigned short d, unsigned short a) {
    z80_writemem(a, d);
    z80_writemem(a+1, d>>8);
}
unsigned char z80_in_stub(unsigned short p) {
    return z80_readport(p);
}
void z80_out_stub(unsigned short p,unsigned char d) {
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
}
unsigned int z80_rebasePC_stub(unsigned short new_pc) {
    DBG("z80_rebasepc %x", new_pc);
    z80_state.Z80PC_BASE = (unsigned int) z80_readmap[new_pc>>10];
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
    DBG("z80_run");

    DrZ80Run(&z80_state, cycles);
}

void z80_get_context (void *dst) {
    (void) 0;
}

void z80_set_context (void *src) {
    (void) 0;
}

void z80_set_irq_line(unsigned int state) {
    DBG("z80_set_irq %x", state);
    // TODO
}
void z80_set_nmi_line(unsigned int state) {
    DBG("z80_set_nmi %x", state);
    // TODO
}
