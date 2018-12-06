#ifndef _TERM_SCHED_H
#define _TERM_SCHED_H

extern volatile int current_display;
extern volatile int running_display;
extern volatile int current_pid[3];
extern volatile int flag_for_term_change;

void setup_shells();
void task_switch(int PID);
void vidchange(int from, int to);
void init_terms();

void asynchronous_task_switch(int new_display);

#endif
