#include <3ds.h>

static char buf[2*1024*1024] = {1};

/* Glue */
void input_update()
{

}

int load_archive(char *filename, unsigned char *buffer, int maxsize, char *extension)
{
    return 0;
}

/* 3ds */
int main(int argc, char* argv[]) {
    srvInit();
    aptInit(APPID_APPLICATION);
    gfxInit();
    hidInit(NULL);
    fsInit();

    APP_STATUS status;

    while((status=aptGetStatus())!=APP_EXITING)
    {
        hidScanInput();

        gfxFlushBuffers();
        gspWaitForVBlank();
        gfxSwapBuffers();

        u32 keys = hidKeysUp();
        if(keys & KEY_B)
            break;
    }

    fsExit();
    hidExit();
    gfxExit();
    aptExit();
    srvExit();
    return 0;
}
