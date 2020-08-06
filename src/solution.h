#ifndef SOLUTION_H
#define SOLUTION_H

#include <iostream>
#include <vector>
#include <time.h>
#include <algorithm>
#include <limits.h>
#include <math.h>

using namespace std;

class Solution {
    private:
        struct insertInfo{
            int vertice;
            int vehicle;
            int cost;

            int posRemove;
            int vehicleRemove;
            int costRemove;
            int posInsert;
            int vehicleInsert;
            int costInsert;
        };

        struct swapInfo{
            int vertice1;
            int pos1;
            int vehicle1;
            int cost1;

            int vertice2;
            int pos2;
            int vehicle2;
            int cost2;

            int totalCost;
            int totalPenality;
        };

        struct initialInsertInfo{
            int vertice1;
            int posRemoveV1;
            int vertice2;
            int posRemoveV2;
            int cost;
        };

        vector<int> distanceList;
        vector<int> capacityList;
        vector< vector<int> > solution;

        vector< vector<int> > distanceMatrix;
        vector<int> clientDemand;

        int dimension;
        int vehicles;
        int capacity;
        int objectiveFunction;

        int penality;

    public:
        Solution();
        Solution(vector< vector<int> > &distanceMatrix, vector<int> &clientDemand, int dimension, int vehicles, int capacity);
        virtual ~Solution();

        void initialSolution(double alpha);
        void swapCapacity();
        void reinsertionCapacity();
        void reinsertion(int verticesQtd, bool pertub);
        void swap(bool pertub);
        void orOpt2();
        void pertub(int pertubFactor);

        void showLog();
        void debugTotalCost();
        void debugCapacity();

        int getObjectiveFunction();
        int getPenality();
};

#endif