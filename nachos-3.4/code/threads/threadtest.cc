// threadtest.cc 
//	Simple test case for the threads assignment.
//
//	Create two threads, and have them context switch
//	back and forth between themselves by calling Thread::Yield, 
//	to illustratethe inner workings of the thread system.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "elevatortest.h"
#include "synch.h"

// testnum is set in main.cc
int testnum = 1;

//----------------------------------------------------------------------
// TestPriority
// 	Fork some Thread with different ways to initial the priority
//----------------------------------------------------------------------

void
TestPriority()
{

    DEBUG('t', "Entering TestPriority");

    Thread *t1 = new Thread("with p", 87);

    Thread *t2 = new Thread("set p");
    t2->setPriority(100);

    Thread *t3 = new Thread("no p");

    t1->Fork(CustomThreadFunc, (void*)0);
    t2->Fork(CustomThreadFunc, (void*)0);
    t3->Fork(CustomThreadFunc, (void*)0);

    CustomThreadFunc(0); // Yield the current thread

    printf("--- Calling TS command ---\n");
    TS();
    printf("--- End of TS command ---\n");
    
}

//----------------------------------------------------------------------
// TS command
// Showing current threads' status (like ps in Linux)
//----------------------------------------------------------------------
void
TS()
{
    DEBUG('t', "Entering TS");

    const char* TStoString[] = {"JUST_CREATED", "RUNNING", "READY", "BLOCKED"};

    printf("UID\tTID\tNAME\tPRI\tSTATUS\n");
    for (int i = 0; i < MAX_THREAD_NUM; i++) { 
        if (tid_pointer[i]) {
          
           printf("%d\t%d\t%s\t%d\t%s\n", tid_pointer[i]->getUserId(), tid_pointer[i]->getThreadId(), tid_pointer[i]->getName(), tid_pointer[i]->getPriority(),TStoString[tid_pointer[i]->getThreadStatus()]);
        }
    }
}

//----------------------------------------------------------------------
// CustomThreadFunc
//
// "which" is simply a number identifying the operation to do on current thread
//----------------------------------------------------------------------

void
CustomThreadFunc(int which)
{
    DEBUG('t', "Entering CustomThreadFunc");
    printf("*** current thread (uid=%d, tid=%d, priority= %d,  name=%s) => ", currentThread->getUserId(), currentThread->getThreadId(), currentThread->getPriority(), currentThread->getName());
    IntStatus oldLevel;
    switch (which)
    {
        case 0:
            printf("Yield\n");
            scheduler->Print();
            printf("\n\n");
            currentThread->Yield();
            break;
        case 1:
            printf("Sleep\n");
            oldLevel = interrupt->SetLevel(IntOff);	// disable interrupts
            currentThread->Sleep();
            (void) interrupt->SetLevel(oldLevel);	// re-enable interrupts
            break;
        case 2:
            printf("Finish\n");
            currentThread->Finish();
            break;
        default:
            printf("Yield (default)\n");
            currentThread->Yield();
            break;
    }
} 


//----------------------------------------------------------------------
// SimpleThread
// 	Loop 5 times, yielding the CPU to another ready thread 
//	each iteration.
//
//	"which" is simply a number identifying the thread, for debugging
//	purposes.
//----------------------------------------------------------------------

void
SimpleThread(int which)
{
    int num;
    
    for (num = 0; num < 5; num++) {
	printf("*** thread %d looped %d times\n", which, num);
        currentThread->Yield();
    }
}

//----------------------------------------------------------------------
// ThreadTest1
// 	Set up a ping-pong between two threads, by forking a thread 
//	to call SimpleThread, and then calling SimpleThread ourselves.
//----------------------------------------------------------------------

void
ThreadTest1()
{
    DEBUG('t', "Entering ThreadTest1");

    Thread *t = new Thread("forked thread");

    t->Fork(SimpleThread, (void*)1);
    SimpleThread(0);
}

//----------------------------------------------------------------------
///Functions to test Lock and Condition Variable implementation 
//----------------------------------------------------------------------

Condition * cond_Consumer = new Condition ("Consumer Condition");
Condition * cond_Producer = new Condition ("Producer Condition");

Lock *resourceLock = new Lock ("Lock");

int resource = 0 ; 
int limit = 10 ; 

//----------------------------------------------------------------------
///Producer
//The producer thread will try to access the resource, 
//if it has reached the limit it will go to sleep,
// otherwise it will increment the resource and send a signal to the consumer thread
//----------------------------------------------------------------------

void Producer (){
    for(int i = 0; i < 30 ; i++){
        resourceLock->Acquire();

        while (resource>= limit){
            printf("Le risorse hanno raggiunto il limite, il Produttore %s va in sleep \n", currentThread->getName());
            cond_Producer->Wait(resourceLock);
        }

        ++resource;
        printf("Il produttore %s ha aggiunto una risorsa, tot %d\n", currentThread->getName(), resource);

        cond_Consumer->Signal(resourceLock);
        resourceLock->Release();
    }
}


//----------------------------------------------------------------------
///Consumer
//The consumer thread will try to access the resource,
// if it is 0 it will go to sleep, 
//otherwise it will decrement the resource and send a signal to the producer Thread//
//----------------------------------------------------------------------

void Consumer (){

    for(int i = 0; i < 30 ; i++){
        resourceLock->Acquire();

        while (resource<= 0){
            printf("Le risorse sono esaurite, il Consumatore %s va in sleep\n ", currentThread->getName());
            cond_Consumer->Wait(resourceLock);
        }

        --resource;
        printf("Il consumatore %s ha rimosso una risorsa, tot %d\n", currentThread->getName(), resource);

        cond_Producer->Signal(resourceLock);
        resourceLock->Release();
    }
}


void SyncTest (){

    DEBUG('t', "Entering in Sync test ");

    Thread *p1 = new Thread("Produttore 1 ");
    Thread *p2 = new Thread("Produttore 2 ");

    Thread *c1 = new Thread("Consumatore 1 ");
    Thread *c2 = new Thread("Consumatore 2 ");

    p1->Fork(Producer, nullptr);
    c1->Fork(Consumer, nullptr);
    c2->Fork(Consumer, nullptr);
    p2->Fork(Producer, nullptr);
    
}

//----------------------------------------------------------------------
// ThreadWithTicks
//  Re-enable the interrupt to invoke OnTick() make system time moving forward
//----------------------------------------------------------------------

void
ThreadWithTicks(int runningTime)
{
    int num;
    
    for (num = 0; num < runningTime * SystemTick; num++) {
        printf("*** thread with running time %d looped %d times (stats->totalTicks: %d)\n", runningTime, num+1, stats->totalTicks);
        interrupt->OneTick(); // make system time moving forward (advance simulated time)
    }
    currentThread->Finish();
}

//----------------------------------------------------------------------
//  RRTest
// 	Fork some Thread with different priority
//----------------------------------------------------------------------

void
RRTest()
{
    DEBUG('t',"Entering RRTest()");

    printf("\nSystem initial ticks:\tsystem=%d, user=%d, total=%d\n", stats->systemTicks, stats->userTicks, stats->totalTicks);

    Thread *t1 = new Thread("7");
    Thread *t2 = new Thread("2");
    Thread *t3 = new Thread("5");

    printf("\nAfter new Thread ticks:\tsystem=%d, user=%d, total=%d\n", stats->systemTicks, stats->userTicks, stats->totalTicks);

    t1->Fork(ThreadWithTicks, (void*)7);
    t2->Fork(ThreadWithTicks, (void*)2);
    t3->Fork(ThreadWithTicks, (void*)5);

    printf("\nAfter 3 fork() ticks:\tsystem=%d, user=%d, total=%d\n\n", stats->systemTicks, stats->userTicks, stats->totalTicks);

    
    scheduler->lastSwitchTick = stats->totalTicks;
    currentThread->Yield(); // Yield the main thread
}



//----------------------------------------------------------------------
// ThreadTest
// 	Invoke a test routine.
//----------------------------------------------------------------------

void
ThreadTest()
{
    switch (testnum) {
    case 1:
	ThreadTest1();
	break;
    case 2 : 
    TestPriority();
    break;
    case 3:
    RRTest();
    break;
    case 4 : 
    SyncTest();
    break;
    default:
	printf("No test specified.\n");
	break;
    }
}

