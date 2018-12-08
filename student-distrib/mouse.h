#ifndef _MOUSE_H
#define _MOUSE_H

#include "i8259.h"
#include "lib.h"
#include "vc.h"
#include "term_sched.h"
#include "types.h"

static uint32_t mouse_x;
static uint32_t mouse_y;
static int32_t  mouse_vx;
static int32_t  mouse_vy;

void write_to_mouse(int8_t command, int m_flag);
void mouse_init(void);
void mouse_interrupt_handler(void);

#endif /* _MOUSE_H */
