#include "compilator.h"
#include "libmanager.h"
int main(void)
{
    struct libmanager lm = libmanager_init();
    struct compilator cr = compilator();
    compilator_comp(&cr);
    compilator_link(cr);
    return 0;
}

