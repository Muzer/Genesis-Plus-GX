#include <3ds.h>
#include <stdio.h>
#include <stdarg.h>

#include "shared.h"
#include "osd.h"

#define SOUND_FREQUENCY 48000
#define SOUND_SAMPLES_SIZE  2048
u8 framebuf[4*320*240];

console_t top;
console_t bot;

/* Text */
#include "font_bin.h"
const u8 *font = font_bin;
#define CHAR_SIZE_X (8)
#define CHAR_SIZE_Y (8)

static void
drawCharacter(u8   *fb,
              char c,
              u16  x,
              u16  y,
              u16  w,
              u16  h)
{
  if(c < ' ')
    return;

  if(x < 0 || y < 0 || x+CHAR_SIZE_X >= w || y+CHAR_SIZE_Y >= h)
    return;

  c -= ' ';

  u8 *charData=(u8*)&font_bin[CHAR_SIZE_X*CHAR_SIZE_Y*c];

  fb += (x*h+y)*3;

  int i, j;
  for(i = 0; i < CHAR_SIZE_X; ++i)
  {
    for(j = 0; j < CHAR_SIZE_Y; ++j)
    {
      u8 v = *(charData++);
      if(v)
        fb[0] = fb[1] = fb[2] = (v==1) ? 0xFF : 0x00;
      fb += 3;
    }

    fb += (h-CHAR_SIZE_Y)*3;
  }
}
void
drawString(u8         *fb,
           const char *str,
           u16        x,
           u16        y,
           u16        w,
           u16        h)
{
  if(!fb || !str)
    return;

  int k, dx = 0, dy = 0;
  for(k = 0; k < strlen(str); ++k)
  {
    if(str[k] >= ' ' && str[k] <= '~')
      drawCharacter(fb, str[k], x+dx, y+dy, w, h);

    dx += 8;
    if(str[k]=='\n')
    {
      dx  = 0;
      dy -= 8;
    }
  }
}

/* GFX */
void
gfxDrawText(gfxScreen_t screen,
            gfx3dSide_t side,
            const char  *str,
            u16         x,
            u16         y)
{
  if(!str)
    return;

  u16 fbWidth, fbHeight;
  u8  *fbAdr = gfxGetFramebuffer(screen, side, &fbWidth, &fbHeight);

  drawString(fbAdr, str, y, x-CHAR_SIZE_Y, fbHeight, fbWidth);
}

void
gfxFillColor(gfxScreen_t screen,
             gfx3dSide_t side,
             u8          rgbColor[3])
{
  u16 fbWidth, fbHeight;
  u8  *fbAdr = gfxGetFramebuffer(screen, side, &fbWidth, &fbHeight);

  //TODO : optimize; use GX command ?
  int i;
  for(i = 0; i < fbWidth*fbHeight; ++i)
  {
    *(fbAdr++) = rgbColor[2];
    *(fbAdr++) = rgbColor[1];
    *(fbAdr++) = rgbColor[0];
  }
}

/* Console */
#define MAX_LINES ((240-8)/8)

static void
consoleClear(console_t *console)
{
    memset(console->console, 0, sizeof(console->console));
    console->lines = 0;
}
static void
renderFrame()
{
    u8 bluish[] = { 0, 0, 127 };

    gfxFillColor(GFX_BOTTOM, GFX_LEFT, bluish);
    gfxDrawText(GFX_BOTTOM, GFX_LEFT, bot.console, 240-8, 0);

    u8* fb = gfxGetFramebuffer(GFX_TOP, GFX_LEFT, NULL, NULL);

    size_t x, y;
    for(x=0; x<320; x++) {
        for(y=0; y<240; y++) {
            fb[3*240*x + 3*y    ] = framebuf[4*320*y + 4*x + 0];
            fb[3*240*x + 3*y + 1] = framebuf[4*320*y + 4*x + 1];
            fb[3*240*x + 3*y + 2] = framebuf[4*320*y + 4*x + 2];
        }
    }

    gfxFlushBuffers();
    gspWaitForVBlank();
    gfxSwapBuffers();
}
__attribute__((format(printf,2,3)))
void
print(console_t  *console,
      const char *fmt, ...)
{
    static char buffer[256];
    va_list ap;
    va_start(ap, fmt);
    vsiprintf(buffer, fmt, ap);
    va_end(ap);

    size_t num_lines = 0;
    const char *p = buffer;
    while((p = strchr(p, '\n')) != NULL)
    {
        ++num_lines;
        ++p;
    }

    if(console->lines + num_lines > MAX_LINES)
    {
        p = console->console;
        while(console->lines + num_lines > MAX_LINES)
        {
            p = strchr(p, '\n');
            ++p;
            --console->lines;
        }

        memmove(console->console, p, strlen(p)+1);
    }

    strcat(console->console, buffer);
    console->lines = console->lines + num_lines;
}

/* Glue */
void input_update()
{
    input.pad[0] = 0;
    print(&bot, "input_update()\n");
}

int load_archive(char *filename, unsigned char *buffer, int maxsize, char *extension)
{
    int size = 0;

    FILE* fd = fopen(filename, "rb");
    if(fd == NULL)
        return 0;

    /* Read file data */
    size = fread(buffer, 1, maxsize, fd);

    /* filename extension */
    if (extension)
    {
        strncpy(extension, &filename[strlen(filename) - 3], 3);
        extension[3] = 0;
    }

    fclose(fd);
    return size;
}


int genplus_init()
{
    /* set default config */
    set_config_defaults();

    /* mark all BIOS as unloaded */
    system_bios = 0;

    /* Genesis BOOT ROM support (2KB max) */
    memset(boot_rom, 0xFF, 0x800);
    FILE* fp = fopen(MD_BIOS, "rb");
    if (fp != NULL)
    {
        int i;

        /* read BOOT ROM */
        fread(boot_rom, 1, 0x800, fp);
        fclose(fp);

        /* check BOOT ROM */
        if (!memcmp((char *)(boot_rom + 0x120),"GENESIS OS", 10))
        {
            /* mark Genesis BIOS as loaded */
            system_bios = SYSTEM_MD;
        }

        /* Byteswap ROM */
        for (i=0; i<0x800; i+=2)
        {
            uint8 temp = boot_rom[i];
            boot_rom[i] = boot_rom[i+1];
            boot_rom[i+1] = temp;
        }
    }

    /* initialize Genesis virtual system */
    memset(&bitmap, 0, sizeof(t_bitmap));
    bitmap.width        = 400;
    bitmap.height       = 240;
    bitmap.pitch        = (bitmap.width * 4);
    bitmap.data         = framebuf;
    bitmap.viewport.changed = 3;

    /* Load game file */
    if(!load_rom("sdmc:/Contra.smd"))
    {
        print(&bot, "Failed to load rom.\n");
        return 1;
    }

    return 0;
}

/* 3DS */
int main(int argc, char* argv[]) {
    srvInit();
    aptInit(APPID_APPLICATION);
    gfxInit();
    hidInit(NULL);
    fsInit();
    sdmcInit();

    consoleClear(&top);
    consoleClear(&bot);

    int ret = genplus_init();
    if(!ret) {
        /* initialize system hardware */
        audio_init(SOUND_FREQUENCY, 0);
        system_init();

        // load internal backup RAM
        // load cartridge backup RAM
        // load SRAM

        //system_reset();

    }

    if(system_hw == SYSTEM_MCD) {
        print(&bot, "hw: mcd\n");
    }
    else if((system_hw & SYSTEM_PBC) == SYSTEM_MD) {
        print(&bot, "hw: md\n");
    }
    else{
        print(&bot, "hw: sms\n");
    }

    APP_STATUS status;
    while((status=aptGetStatus())!=APP_EXITING)
    {
        hidScanInput();

        if (ret) {
            print(&bot, "err\n");
        }
        else {
            if (system_hw == SYSTEM_MCD) {
                system_frame_scd(0);
            }
            else if ((system_hw & SYSTEM_PBC) == SYSTEM_MD) {
                system_frame_gen(0);
            }
            else {
                system_frame_sms(0);
            }
        }

        u32 keys = hidKeysUp();
        if(keys & KEY_B)
            break;

        renderFrame();
    }

    if(!ret) {
        audio_shutdown();
    }

    sdmcExit();
    fsExit();
    hidExit();
    gfxExit();
    aptExit();
    srvExit();
    return 0;
}
