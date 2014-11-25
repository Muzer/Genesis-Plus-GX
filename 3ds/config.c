#include "osd.h"

t_config config;


void set_config_defaults(void)
{
   int i;

   /* sound options */
   config.psg_preamp     = 150;
   config.fm_preamp      = 100;
   config.hq_fm          = 1; /* high-quality resampling */
   config.psgBoostNoise  = 1;
   config.filter         = 0; /* no filter */
   config.lp_range       = 0x9999; /* 0.6 in 16.16 fixed point */
   config.low_freq       = 880;
   config.high_freq      = 5000;
   config.lg             = 1.0;
   config.mg             = 1.0;
   config.hg             = 1.0;
   config.dac_bits 	     = 14; /* MAX DEPTH */ 
   config.ym2413         = 2; /* AUTO */
   config.mono           = 0; /* STEREO output */

   /* system options */
   config.system         = 0; /* AUTO */
   config.region_detect  = 0; /* AUTO */
   config.vdp_mode       = 0; /* AUTO */
   config.master_clock   = 0; /* AUTO */
   config.force_dtack    = 0;
   config.addr_error     = 1;
   config.bios           = 0;
   config.lock_on        = 0;

   /* video options */
   config.overscan = 0;
   config.gg_extra = 0;
   config.ntsc     = 0;
   config.render   = 0;

   /* input options */
   input.system[0] = SYSTEM_GAMEPAD;
   input.system[1] = SYSTEM_GAMEPAD;
   for (i=0; i<MAX_INPUTS; i++)
   {
     config.input[i].padtype = DEVICE_PAD2B | DEVICE_PAD3B | DEVICE_PAD6B;
   }
}

