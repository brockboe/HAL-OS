#ifndef _TERM_SCHED_H
#define _TERM_SCHED_H

extern volatile int current_display;
extern volatile int running_display;
extern volatile int current_pid[3];

void setup_shells();
void task_switch(int PID);
void prep_term_with_command(uint8_t * command, int term_number);
void vidchange(int from, int to);
void init_terms();

#endif
