#include "ThreadLocal.h"

__thread int LThreadType = -1;
__thread Scheduler* LScheduler = nullptr;


