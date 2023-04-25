#pragma once
#include <time.h>
#include <stdlib.h>

srand(time(NULL));

int CreateRandomInt(void){
return rand();
}