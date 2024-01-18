// synch.cc 
//	Routines for synchronizing threads.  Three kinds of
//	synchronization routines are defined here: semaphores, locks 
//   	and condition variables (the implementation of the last two
//	are left to the reader).
//
// Any implementation of a synchronization routine needs some
// primitive atomic operation.  We assume Nachos is running on
// a uniprocessor, and thus atomicity can be provided by
// turning off interrupts.  While interrupts are disabled, no
// context switch can occur, and thus the current thread is guaranteed
// to hold the CPU throughout, until interrupts are reenabled.
//
// Because some of these routines might be called with interrupts
// already disabled (Semaphore::V for one), instead of turning
// on interrupts at the end of the atomic operation, we always simply
// re-set the interrupt state back to its original value (whether
// that be disabled or enabled).
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "synch.h"
#include "system.h"

//----------------------------------------------------------------------
// Semaphore::Semaphore
// 	Initialize a semaphore, so that it can be used for synchronization.
//
//	"debugName" is an arbitrary name, useful for debugging.
//	"initialValue" is the initial value of the semaphore.
//----------------------------------------------------------------------

Semaphore::Semaphore(char* debugName, int initialValue)
{
    name = debugName;
    value = initialValue;
    queue = new List;
}

//----------------------------------------------------------------------
// Semaphore::Semaphore
// 	De-allocate semaphore, when no longer needed.  Assume no one
//	is still waiting on the semaphore!
//----------------------------------------------------------------------

Semaphore::~Semaphore()
{
    delete queue;
}

//----------------------------------------------------------------------
// Semaphore::P
// 	Wait until semaphore value > 0, then decrement.  Checking the
//	value and decrementing must be done atomically, so we
//	need to disable interrupts before checking the value.
//
//	Note that Thread::Sleep assumes that interrupts are disabled
//	when it is called.
//----------------------------------------------------------------------

void
Semaphore::P()
{
    IntStatus oldLevel = interrupt->SetLevel(IntOff);	// disable interrupts
    
    while (value == 0) { 			// semaphore not available
	queue->Append((void *)currentThread);	// so go to sleep
	currentThread->Sleep();
    } 
    value--; 					// semaphore available, 
						// consume its value
    
    (void) interrupt->SetLevel(oldLevel);	// re-enable interrupts
}

//----------------------------------------------------------------------
// Semaphore::V
// 	Increment semaphore value, waking up a waiter if necessary.
//	As with P(), this operation must be atomic, so we need to disable
//	interrupts.  Scheduler::ReadyToRun() assumes that threads
//	are disabled when it is called.
//----------------------------------------------------------------------

void
Semaphore::V()
{
    Thread *thread;
    IntStatus oldLevel = interrupt->SetLevel(IntOff);

    thread = (Thread *)queue->Remove();
    if (thread != NULL)	   // make thread ready, consuming the V immediately
	scheduler->ReadyToRun(thread);
    value++;
    (void) interrupt->SetLevel(oldLevel);
}

// Dummy functions -- so we can compile our later assignments 
// Note -- without a correct implementation of Condition::Wait(), 
// the test case in the network assignment won't work!
Lock::Lock(char* debugName) {
    name = debugName;
    semaphore= new Semaphore("Lock",1);
    lock_owner= NULL ; 
}
Lock::~Lock() {
   delete semaphore; 
}
void Lock::Acquire() {
    //controllo che il lock non sia già acquisito 
    ASSERT(! this -> isHeldByCurrentThread());

    //mascheriamo gli interrupt per evitare che il thread che sta acquisendo il lock venga tolto dalla cpu
    IntStatus oldLevel = interrupt->SetLevel(IntOff);

    DEBUG ('s', "Lock \%s\" Acquired by Thread \"%s\"\n", name , currentThread->getName());

    //utilizziamo il semaforo binario per gestire l'acquisizione o l'eventuale wait 
    semaphore->P();
    lock_owner= currentThread;

    //abilitiamo nuovamente gli interrupt 
    (void) interrupt -> SetLevel (oldLevel);
}
void Lock::Release() {
    //controllo che il lock che sta facendo la release sia effettivamente l' owner del lock 
    ASSERT( this -> isHeldByCurrentThread());

    //disabilito gli interrupt 
    IntStatus oldLevel = interrupt->SetLevel(IntOff);

    DEBUG ('s', "Lock \%s\" Released by Thread \"%s\"\n", name , currentThread->getName());
    lock_owner = NULL ; 

    semaphore->V();

    //abilitiamo gli interrupt 
     (void) interrupt -> SetLevel (oldLevel);
}

bool Lock::isHeldByCurrentThread(){

    return lock_owner==currentThread;
}

Condition::Condition(char* debugName) {
    name = debugName;
    queue= new List; 
 }

Condition::~Condition() { }

void Condition::Wait(Lock* conditionLock) { 
    //ASSERT(FALSE); 

    //disabilitiamo  gli interrupt 
    IntStatus oldLevel = interrupt->SetLevel(IntOff);

    //Verifichiamo che il Thread che sta andando in sleep sia l'owner del lock 
    ASSERT( conditionLock -> isHeldByCurrentThread());
    
    //Rilasciamo il lock prima di andare in sleep 
    conditionLock->Release();

    //inseriamo il thread nella coda di sleep 

    queue->Append(currentThread);
    currentThread->Sleep();

    //Dopo l'awake il Thread riacquista il lock 
    conditionLock->Acquire();

    
    (void) interrupt -> SetLevel (oldLevel); 
    }

void Condition::Signal(Lock* conditionLock) { 

    //disabilito gli interrupt 

    IntStatus oldLevel = interrupt->SetLevel(IntOff);
    
    ASSERT( conditionLock -> isHeldByCurrentThread());

    //sveglio il primo Thread in coda, se ce n'è qualcuno 

    if(!queue->IsEmpty()){
        Thread *nextThread = queue->Remove();

        scheduler->ReadyToRun(nextThread);
    }

    (void) interrupt -> SetLevel (oldLevel); 

}


void Condition::Broadcast(Lock* conditionLock) {

    //disabilito gli interrupt 

    IntStatus oldLevel = interrupt->SetLevel(IntOff);
    
    ASSERT( conditionLock -> isHeldByCurrentThread());

    //sveglio tuttii thread in coda

    while(!queue->IsEmpty()){
        Signal(conditionLock);
    }

    (void) interrupt -> SetLevel (oldLevel); 

 }


