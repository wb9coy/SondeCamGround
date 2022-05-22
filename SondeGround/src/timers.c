#include <signal.h>
#include <stdio.h>
#include <time.h>
#include "statusThread.h"

static timer_t statusTimerID;


static void timerHandler(int sig,siginfo_t*si,void*uc )
{
    timer_t*tidp;

    tidp = si->si_value.sival_ptr;

    if(*tidp == statusTimerID )
    {
    	signalStatusThread();
    }
}

static int makeTimer(timer_t*timerID,int time)
{
    struct sigevent te;
    struct itimerspec its;
    struct sigaction sa;
    int sigNo = SIGRTMIN;
    int result = 1;

    // Set up signal handler.
    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = timerHandler;//Action when singal is triggered
    sigemptyset(&sa.sa_mask);
    if(sigaction(sigNo,&sa, NULL)==-1)
    {
        printf("Faltal Error creating Timer sigaction");
        result = -1;
    }

    // Set and enable alarm
    te.sigev_notify = SIGEV_SIGNAL;//Gnerate alarm upon expiration
    te.sigev_signo = sigNo;//SIGALRM
    te.sigev_value.sival_ptr = timerID;//Timer ID
    //Create a per_process timer using the timer ID
    timer_create(CLOCK_REALTIME,&te, timerID);

    //Interval for starting again
    its.it_interval.tv_sec =time;
    its.it_interval.tv_nsec =0;
    //Timer time
    its.it_value.tv_sec = time;
    its.it_value.tv_nsec =0;
    //Arm/disarmer a per process time
    timer_settime(*timerID,0,&its, NULL);

    return result;
}

int setupTimers()
{
	printf("Setup Timers \n");

	makeTimer(&statusTimerID,10);

	return 1;

}
