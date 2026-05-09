//
// Created by Owner on 09/05/2026.
//

#include "debuggingTools.h"

#include <malloc.h>
#include <stdio.h>

void printHeapStatus() {
    const int heapstatus = _heapchk();
    switch( heapstatus )
    {
        case _HEAPOK:
            printf(" OK - heap is fine\n" );
        break;
        case _HEAPEMPTY:
            printf(" OK - heap is empty\n" );
        break;
        case _HEAPBADBEGIN:
            printf( "ERROR - bad start of heap\n" );
        break;
        case _HEAPBADNODE:
            printf( "ERROR - bad node in heap\n" );
        break;
        case _HEAPBADPTR:
            printf( "ERROR - bad pointer in heap\n" );
        break;
        default:
            break;
    }
}
