//
// Created by Daniel on 2024-05-01.
//

#include "ythreader.h"

void YThreader::_notification(int p_what) {
    switch (p_what) {
        case NOTIFICATION_EXIT_TREE: {
            if (!Engine::get_singleton()->is_editor_hint()) {
                run_mutex.lock();
                _running = false;
                run_mutex.unlock();

                for (int i = 0; i < workers.size(); i++) {
                    available.post();
                }
                for (const auto& worker: workers) {
                    if (worker != nullptr)
                        worker->wait_to_finish();
                }
                break;
            }
        }
        case NOTIFICATION_READY: {
            if (!Engine::get_singleton()->is_editor_hint()) {
                _use_thread_count = thread_count > 0 ? thread_count : OS::get_singleton()->get_processor_count();
                for (int i = 0; i < _use_thread_count; ++i) {
                    auto worker = (memnew(Thread));
                    worker->start(_worker_thread,this);
                    workers.push_back(worker);
                }
            }
        } break;
        default:
            break;
    }
}

void YThreader::_worker_thread(void *p_threader) {
    auto *preview = static_cast<YThreader *>(p_threader);
    while(true) {
        preview->available.wait();

        preview->run_mutex.lock();
        bool should_exit = !preview->_running;
        preview->run_mutex.unlock();

        if (should_exit)
            break;

        preview->queue_mutex.lock();
        bool is_thread_empty = preview->thread_queue.is_empty();
        preview->queue_mutex.unlock();

        if (is_thread_empty) {
            continue;
        }

        preview->queue_mutex.lock();
        const auto& callback = preview->thread_queue[0];
        preview->queue_mutex.unlock();

        if (callback.is_valid()) {            
            callback.call();
        }

        preview->queue_mutex.lock();
        preview->thread_queue.remove_at(0);
        preview->queue_mutex.unlock();
    }
}

void YThreader::_bind_methods() {
    ClassDB::bind_method(D_METHOD("queue_job","p_callable"), &YThreader::queue_job);
    ClassDB::bind_method(D_METHOD("lock_mutex"), &YThreader::lock_mutex);
    ClassDB::bind_method(D_METHOD("unlock_mutex"), &YThreader::unlock_mutex);
    ClassDB::bind_method(D_METHOD("set_thread_count", "thread_count"), &YThreader::set_thread_count);
    ClassDB::bind_method(D_METHOD("get_thread_count"), &YThreader::get_thread_count);
    ClassDB::bind_static_method("YThreader",D_METHOD("is_main_thread"), &YThreader::is_main_thread);
    ADD_PROPERTY(PropertyInfo(Variant::INT, "thread_count"), "set_thread_count", "get_thread_count");
}

void YThreader::queue_job(const Callable &p_callable) {
    queue_mutex.lock();
    thread_queue.push_back(p_callable);
    queue_mutex.unlock();
    available.post();
}

void YThreader::lock_mutex() {
    custom_mutex.lock();
}

void YThreader::unlock_mutex() {
    custom_mutex.unlock();
}

bool YThreader::is_main_thread() {
    return Thread::is_main_thread();
}

void YThreaderInterrupter::_bind_methods() {
    ClassDB::bind_method(D_METHOD("cancel"), &YThreaderInterrupter::cancel);
    ClassDB::bind_method(D_METHOD("is_cancelled"), &YThreaderInterrupter::is_cancelled);
}

void YThreaderInterrupter::cancel() {
    mutex.lock();
    cancelled=true;
    mutex.unlock();
}

bool YThreaderInterrupter::is_cancelled() {
    mutex.lock();
    bool is_cancelled = cancelled;
    mutex.unlock();

    return is_cancelled;
}
