#include "objects/thread/thread.h"
#include "thread/types/lock.h"

#include "common/results.h"

love::Thread::Thread(Threadable* t) : common::Thread(t)
{}

love::Thread::~Thread()
{
    thread::Lock lock(this->mutex);

    if (this->hasThread)
        threadClose(&this->thread);
}

void love::Thread::Wait()
{
    {
        thread::Lock lock(this->mutex);

        if (!this->hasThread)
            return;
    }

    threadWaitForExit(&this->thread);
    threadClose(&this->thread);

    thread::Lock(this->mutex);

    this->running   = false;
    this->hasThread = false;
}

bool love::Thread::Start()
{
    thread::Lock lock(this->mutex);

    if (this->running)
        return false;

    if (this->hasThread)
        threadWaitForExit(&this->thread);

    Result rc = threadCreate(&this->thread, Runner, this, NULL, Thread::STACK_SIZE, 0x3B, 0);

    if (R_SUCCEEDED(rc))
        rc = threadStart(&this->thread);

    this->running   = R_SUCCEEDED(rc);
    this->hasThread = this->running;

    return this->running;
}
