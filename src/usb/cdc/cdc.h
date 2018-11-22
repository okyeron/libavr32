#ifndef _USB_CDC_H_
#define _USB_CDC_H_

#include "uhc.h"

extern bool callback_cdc_change(uhc_device_t* dev, bool b_plug);

extern void callback_cdc_rx_notify(void);

extern void my_task(void);
extern void my_task_rx(void);

#endif
