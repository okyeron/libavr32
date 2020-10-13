/*
  cdc.c
  libavr32

  usb host routines for cdc driver
 */

// std
#include <stdint.h>

// asf
#include "uhc.h"
#include <string.h>
#include "print_funcs.h"

// libavr32
#include "conf_usb_host.h"
#include "events.h"
#include "cdc.h"
#include "monome.h"
#include "ftdi.h"

static bool cdc_available = false;

static u8 rxBuf [CDC_RX_BUF_SIZE];
static u8 rxBytes = 0;

// This callback is called when a USB device CDC is plugged or unplugged. 
// The communication port can be opened and configured here.
bool callback_cdc_change(uhc_device_t* dev, bool b_plug) {
   if (b_plug) {
      // USB Device CDC connected
      cdc_available = true;
      // Open and configure USB CDC ports
      usb_cdc_line_coding_t cfg = {
         .dwDTERate   = CPU_TO_LE32(115200),
         .bCharFormat = CDC_STOP_BITS_1,
         .bParityType = CDC_PAR_NONE,
         .bDataBits   = 8,
      };
      uhi_cdc_open(0, &cfg);
      print_dbg("\r\n CDC OPEN");
   } else {
      cdc_available = false;
   }
   return 0;
}

// This callback is called when a new data are received. 
// This can be used to manage data reception through interrupt and avoid pooling.
void callback_cdc_rx_notify(void) {
   // Wakeup my_task_rx() task
}

#define MESSAGE "Hello"
void my_task(void)
{
   static bool startup = true;
   if (!cdc_available) {
      startup = true;
      return;
   }
   if (startup) {
      startup = false;
      // Send data on CDC communication port
      print_dbg("\r\n SEND MESSAGE");
      uhi_cdc_write_buf(0, MESSAGE, sizeof(MESSAGE)-1);
      uhi_cdc_putc(0,'\n');
      return;
   }
}
void my_task_rx(void)
{
    rxBytes = 0;
    while (uhi_cdc_is_rx_ready(0)) {
        int value = uhi_cdc_getc(0);
        rxBuf[rxBytes++] = (u8)value;
        if (value != '\0') {
            print_dbg("\r\n REC MESSAGE: ");
            print_dbg_hex(value);
        }
   }
   ///  ...[A]... call monomeserial parser somehow ????
   ftdi_write_rx_buf(rxBuf, rxBytes);
   (*monome_read_serial)();
}