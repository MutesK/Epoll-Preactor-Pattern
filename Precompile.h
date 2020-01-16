#pragma once

#include <iostream>
#include <cstring>

#include <cstdlib>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/epoll.h>
#include <netinet/in.h>


#include <thread>
#include <memory>
#include <queue>
#include <mutex>
#include <functional>
#include <atomic>

#ifdef WIN32
using Descriptor = HANDLE;
#else

#define abstract
#define __interface

#define INFINITE -1
using Descriptor = int32_t;


template<typename T, typename... Args>
std::unique_ptr<T> make_unique(Args&&... args)
{
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}
#endif