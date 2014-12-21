#ifndef UBUNTU_FIX_H
#define UBUNTU_FIX_H

#include <pthread.h>


void ubuntu_fix()
{
    pthread_getconcurrency();
}


#endif // UBUNTU_FIX_H
