#pragma once

#include <mutex>
#include <condition_variable>


typedef std::recursive_mutex RecursiveMutex;
typedef std::lock_guard<RecursiveMutex> RecursiveLock;


typedef std::mutex Mutex;
typedef std::unique_lock<Mutex> AutoLock;


typedef std::condition_variable Condition;