#ifndef _MOUSE_H
#define _MOUSE_H

#include "i8259.h"
#include "lib.h"
#include "vc.h"
#include "term_sched.h"
#include "types.h"
#include "keyboard.h"
#define MK_COMMAND_PORT 0x64
#define AUX_DEVICE_CMD  0xA8
#define K_FLAG          0
#define M_FLAG          1
#define SET_STATUS_INIT 0x47
#define ENABLE_SGN_CMD  0xF4
#define SEND_MOUSE_CMD  0xD4
#define MOUSE_IRQ       12
#define MOUSE_CMD_CHECK 0x20



static uint32_t mouse_x;
static uint32_t mouse_y;
static int32_t  mouse_vx;
static int32_t  mouse_vy;


void write_to_mouse(int8_t command, int m_flag);
void mouse_init(void);
void mouse_interrupt_handler(void);

#endif /* _MOUSE_H */
