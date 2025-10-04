#pragma once
#include <sys/types.h>

typedef enum {
    CTRL_C_NONE = 0,        
    CTRL_C_SERVER_TTY,      
    CTRL_C_FROM_CLIENT      
} ctrlc_origin_t;

int ctrlc_init_signalfd(void);

ctrlc_origin_t ctrlc_poll(int sfd, pid_t *out_sender_pid);

void ctrlc_close(int sfd);
