#ifndef CVRP_H
#define CVRP_H

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream> 
#include "solution.h"

using namespace std;

class CVRP {
    private:
        vector< vector<int> > distanceMatrix;
        vector<int> clientDemand;
        int dimension;
        int vehicles;
        int capacity;

        Solution finalSol;
        Solution bestSol;
        Solution currentSol;
        Solution lsSol;

    public:
        CVRP(char *archivePath);
        virtual ~CVRP();

        void GILS_RVND();
        void ILS(int perturbFactor);
        void RVND();
        void RVND_C();
};  

#endif