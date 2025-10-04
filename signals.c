// signals_ctrlc.c
#define _GNU_SOURCE
#include "signals_ctrlc.h"
#include <signal.h>
#include <sys/signalfd.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>

int ctrlc_init_signalfd(void) {
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGINT);

    if (sigprocmask(SIG_BLOCK, &mask, NULL) == -1) return -1;

    int sfd = signalfd(-1, &mask, SFD_NONBLOCK | SFD_CLOEXEC);
    return sfd; 
}

ctrlc_origin_t ctrlc_poll(int sfd, pid_t *out_sender_pid) {
    struct signalfd_siginfo si;
    ssize_t n = read(sfd, &si, sizeof(si));
    if (n == -1) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) return CTRL_C_NONE;
        return CTRL_C_NONE;
    }
    if (n != sizeof(si)) return CTRL_C_NONE;

    if (si.ssi_signo != SIGINT) return CTRL_C_NONE;

    if (si.ssi_pid == 0 || si.ssi_code == SI_KERNEL) {
        return CTRL_C_SERVER_TTY;
    }

    if (out_sender_pid) *out_sender_pid = (pid_t)si.ssi_pid;
    return CTRL_C_FROM_CLIENT;
}

void ctrlc_close(int sfd) {
    if (sfd >= 0) close(sfd);
}
