#include "SignalCtrlC.hpp"
#include <cstring>

SignalCtrlC::SignalCtrlC() : signalfd_(-1) {
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGINT);

    // Bloque SIGINT pour que seul signalfd le reçoive
    if (sigprocmask(SIG_BLOCK, &mask, nullptr) == -1) {
        perror("sigprocmask");
        return;
    }

    signalfd_ = signalfd(-1, &mask, SFD_NONBLOCK | SFD_CLOEXEC);
    if (signalfd_ < 0) {
        perror("signalfd");
    }
}

SignalCtrlC::~SignalCtrlC() {
    if (signalfd_ >= 0) close(signalfd_);
}

CtrlCOrigin SignalCtrlC::pollOnce(pid_t &senderPid) {
    struct signalfd_siginfo si {};
    ssize_t n = read(signalfd_, &si, sizeof(si));

    if (n == -1) {
        if (errno == EAGAIN || errno == EWOULDBLOCK)
            return CtrlCOrigin::None;
        perror("read(signalfd)");
        return CtrlCOrigin::None;
    }
    if (n != sizeof(si) || si.ssi_signo != SIGINT)
        return CtrlCOrigin::None;

    // Cas 1 : Ctrl-C depuis le terminal du serveur
    if (si.ssi_pid == 0 || si.ssi_code == SI_KERNEL) {
        return CtrlCOrigin::ServerTTY;
    }

    // Cas 2 : Signal d’un autre processus (kill/sigqueue)
    senderPid = static_cast<pid_t>(si.ssi_pid);
    return CtrlCOrigin::FromClient;
}
