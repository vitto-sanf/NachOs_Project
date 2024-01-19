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
// TS command
// Showing current threads' status (like ps in Linux)
//----------------------------------------------------------------------
void
TS()
{
    DEBUG('t', "Entering TS");

    const char* TStoString[] = {"JUST_CREATED", "RUNNING", "READY", "BLOCKED"};

    printf("UID\tTID\tNAME\tSTATUS\n");
    for (int i = 0; i < MAX_THREAD_NUM; i++) { 
        if (tid_pointer[i]) {
          
           printf("%d\t%d\t%s\t%s\n", tid_pointer[i]->getUserId(), tid_pointer[i]->getThreadId(), tid_pointer[i]->getName(), TStoString[tid_pointer[i]->getThreadStatus()]);
        }
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
    case 4 : 
    SyncTest();
    default:
	printf("No test specified.\n");
	break;
    }
}

