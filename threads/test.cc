#include "test.h"
#include "thread.h"
#include "kernel.h"
#include "utility.h"
#include "copyright.h"
#include "switch.h"
#include "synch.h"


void
Lab1Exercise3Thread(int which)
{
    int num;

    for (num = 0; num < 5; num++) {
    printf("*** thread %d (uid=%d, tid=%d) looped %d times\n", which, kernel->currentThread->getUserId(), kernel->currentThread->getThreadID(), num);
        kernel->currentThread->Yield();
    }
}

void testTid(){

    DEBUG('t', "Entering test tid : ");
    const int max_Thread = 5 ; 
    const int uid = 87 ; 

     for( int i = 0 ; i < max_Thread; i++){
        Thread *t = new Thread("test Thread");
        t-> setUserID(uid);
        t-> Fork ((VoidFunctionPtr)Lab1Exercise3Thread, (void*)t->getThreadID());
     }
     Lab1Exercise3Thread(0);
}