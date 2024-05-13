//
// Created by Daniel on 2024-05-01.
//

#ifndef YTHREADER_H
#define YTHREADER_H
#include "scene/main/node.h"
#include "core/os/thread.h"


class YThreader : public Node {
    GDCLASS(YThreader,Node);

protected:
    void _notification(int p_what);

    static void _worker_thread(void *p_threader);

    static void _bind_methods();

public:
    void queue_job(const Callable &p_callable);
    int thread_count = 1;
    int get_thread_count() const {return thread_count;}
    void set_thread_count(int p_tcount) {thread_count = p_tcount;}
    void lock_mutex();
    void unlock_mutex();
    static bool is_main_thread();
    int _use_thread_count = 0;

    Vector<Callable> thread_queue;

    // A mutex to synchronize access to the task queue by different threads.
    Semaphore available;

    // A mutex used to notify worker that a new task has become available.
    Mutex run_mutex;

    // Queue mutex to prevent manipulate with task queue on same time.
    Mutex queue_mutex;

    Vector<Thread*> workers;

    // Custom mutex
    Mutex custom_mutex;

    // An atomic (in mind) variable indicating to the workers to keep running.
    bool _running = true;

    YThreader() {

    }
};

class YThreaderInterrupter : public RefCounted {
    GDCLASS(YThreaderInterrupter, RefCounted);

protected:
    bool cancelled = false;
    Mutex mutex;

    static void _bind_methods();

public:
    void cancel();

    bool is_cancelled();

};
#endif //YTHREADER_H
