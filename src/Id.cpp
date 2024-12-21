#include "../include/Id.h"

int Id::getId(){
    static int id = 0;
    return ++id;
}
