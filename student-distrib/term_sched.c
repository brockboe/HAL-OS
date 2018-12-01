#include "lib.h"
#include "types.h"
#include "structures.h"
#include "paging.h"
#include "video.h"

void task_switch(int PID, char * EIP){
      cli();

      

      sti();
      return;
}
