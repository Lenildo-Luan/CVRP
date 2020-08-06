#include "cvrp.h"

using namespace std;

int main(int argc, char *argv[]){
    srand(time(0));

    CVRP problem(argv[1]);
    problem.GILS_RVND();
}

