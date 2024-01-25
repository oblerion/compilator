#include "compilator.h"
int main(void)
{
    struct compilator cr = compilator();
    compilator_comp(&cr);
    compilator_link(cr);
    return 0;
}

