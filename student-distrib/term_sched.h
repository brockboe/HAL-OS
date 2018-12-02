#ifndef _TERM_SCHED_H
#define _TERM_SCHED_H

extern int current_display;
extern int current_pid[3];

void setup_shells();
void task_switch(int PID);

#endif
