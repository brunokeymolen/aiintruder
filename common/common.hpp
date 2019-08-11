/*
 * Copyright 2019 Bruno Keymolen (bruno.keymolen@gmail.com)
 *
 * This is unpublished proprietary source code of Bruno Keymolen (bruno.keymolen@gmail.com).
 * The copyright notice above does not evidence any actual or
 * intended publication of such source code.
 *
 * Redistribution of this material is strictly prohibited.
 */
#ifndef KEY_COMMON_HPP
#define KEY_COMMON_HPP

#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>

#include <iostream>
#include <iomanip>
#include <vector>
#include <cstdint>

#include <sys/types.h>
#include <sys/syscall.h>

#define LOG_PTR(P) std::hex << (void*)P << std::dec
#define LOG_PRE __FILE__ << " " << __FUNCTION__<< ":" << __LINE__ << " "
#define LOG_DBG(X) std::cout << LOG_PRE << X << std::endl;
#define LOG_WARN LOG_DBG
#define LOG_ERR LOG_DBG


namespace keymolen {

    inline pid_t gettid()
    {
        return syscall(__NR_gettid);
    }

}

#endif
