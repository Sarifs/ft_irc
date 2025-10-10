#pragma once
#include <csignal>
#include <sys/signalfd.h>
#include <unistd.h>
#include <cerrno>
#include <cstdint>
#include <iostream>

enum class CtrlCOrigin {
    None = 0,
    ServerTTY,    
    FromClient 
};

class SignalCtrlC {
public:
    SignalCtrlC();
    ~SignalCtrlC();

    SignalCtrlC(const SignalCtrlC&) = delete;
    SignalCtrlC& operator=(const SignalCtrlC&) = delete;

    CtrlCOrigin pollOnce(pid_t &senderPid);

    inline int fd() const { return signalfd_; }
    inline bool isValid() const { return signalfd_ >= 0; }

private:
    int signalfd_;
};
