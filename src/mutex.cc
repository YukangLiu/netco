//@author Liu Yukang
#include "../include/mutex.h"
#include "../include/scheduler.h"
#include "../include/spinlock_guard.h"

using namespace netco;

void RWMutex::rlock(){

    {
        SpinlockGuard l(lock_);
        if(state_ == MU_FREE || state_ == MU_READING){
            readingNum_.fetch_add(1);
            state_ = MU_READING;
            return;
        }

        waitingCo_.push(Scheduler::getScheduler()->getProcessor(threadIdx)->getCurRunningCo());
    
    }

    Scheduler::getScheduler()->getProcessor(threadIdx)->yield();
    rlock();

}

void RWMutex::runlock(){
    SpinlockGuard l(lock_);
    auto cur = readingNum_.fetch_add(-1);
    if(cur == 1){
        freeLock();
    }
}

void RWMutex::wlock(){
    
    {
        SpinlockGuard l(lock_);
        if(state_ == MU_FREE){
            state_ = MU_WRITING;
            return;
        }
        
        waitingCo_.push(Scheduler::getScheduler()->getProcessor(threadIdx)->getCurRunningCo());
        
    }
    
    Scheduler::getScheduler()->getProcessor(threadIdx)->yield();
    wlock();
}
    
void RWMutex::wunlock(){
    SpinlockGuard l(lock_);
    freeLock();
}

void RWMutex::freeLock(){
    state_ = MU_FREE;
    while(!waitingCo_.empty()){
        auto wakeCo = waitingCo_.front();
        waitingCo_.pop();
        wakeCo->getMyProcessor()->goCo(wakeCo);
    }
}