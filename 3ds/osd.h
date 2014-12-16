
#ifndef _OSD_H_
#define _OSD_H_

#include <stdio.h>
#include <string.h>
#include <3ds.h>

#include "shared.h"
#include "config.h"

#define osd_input_update input_update

#define GG_ROM      "sdmc:/genplus/bios/ggenie.bin"
#define AR_ROM      "sdmc:/genplus/bios/areplay.bin"
#define SK_ROM      "sdmc:/genplus/bios/sk.bin"
#define SK_UPMEM    "sdmc:/genplus/bios/sk2chip.bin"
#define CD_BIOS_US  "sdmc:/genplus/bios/bios_CD_U.bin"
#define CD_BIOS_EU  "sdmc:/genplus/bios/bios_CD_E.bin"
#define CD_BIOS_JP  "sdmc:/genplus/bios/bios_CD_J.bin"
#define MD_BIOS     "sdmc:/genplus/bios/bios_MD.bin"
#define MS_BIOS_US  "sdmc:/genplus/bios/bios_U.sms"
#define MS_BIOS_EU  "sdmc:/genplus/bios/bios_E.sms"
#define MS_BIOS_JP  "sdmc:/genplus/bios/bios_J.sms"
#define GG_BIOS     "sdmc:/genplus/bios/bios.gg"

typedef struct {
    char   console[2048];
    size_t lines;
} console_t;

extern console_t top;
extern console_t bot;

#if 1
#define DBG(x ...) do {                                 \
        char buf[1024];                                 \
        sprintf(buf, x);                                \
        svcOutputDebugString(buf, strlen(buf)+1);       \
    } while (0)
#else
#define DBG(x ...) do {                         \
        (void) 0;                               \
    } while(0)
#endif

#define CUSTOM_BLITTER(line, width, pixel, src) do {                    \
        u16* fb = ((u16*)bitmap.data) + (bitmap.viewport.h - 1 - line); \
        do {                                                            \
            *fb = pixel[*src++];                                        \
            fb += 240;                                                  \
        } while (--width);                                              \
    } while(0);

#define error(args ...) do {                    \
        DBG(args);                              \
    } while(0)

__attribute__((format(printf,2,3)))
void
print(console_t  *console,
      const char *fmt, ...);

#endif
