#include "StreamTask.h"
#include "LogUtility.h"


#if MAX_TASKS_IN_POOL > 0

//static ftl::FixedSizePool poolStreamTask(sizeof(StreamTask), MAX_TASKS_IN_POOL);

//void* StreamTask::operator new(size_t)
//{
//    return poolStreamTask.allocate();
//}
//void StreamTask::operator delete(void* p, size_t)
//{
//    poolStreamTask.deallocate(p);
//}
#endif  // MAX_TASKS_IN_POOL
