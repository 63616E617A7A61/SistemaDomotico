// @author Tommaso Rovoletto

#include "../include/Id.h"

/**
 * @brief method that returns a unique id
 * @return a unique id
 */
int Id::getId(){
    static int id = 0;
    return ++id;
}
