#include "solution.h"

using namespace std;

Solution::Solution(){};

Solution::~Solution(){};

Solution::Solution(vector< vector<int> > &distanceMatrix, vector<int> &clientDemand, int dimension, int vehicles, int capacity){
    this->distanceMatrix = distanceMatrix;
    this->clientDemand = clientDemand;
    this->dimension = dimension;
    this->vehicles = vehicles;
    this->capacity = capacity;
}

void Solution::initialSolution(double alpha){
    vector<int> route, candidateList; 
    vector<insertInfo> verticeList;
    initialInsertInfo initialInsertInfo;
    insertInfo insertInfoVar;
    int sizeSol, candidateListSize, currentCost, vertice, alphaVertices;
    //float alpha = 0.5;
    struct { 
        inline bool operator() (insertInfo &vertice1, insertInfo &vertice2){
            return vertice1.cost < vertice2.cost;
        }
    } compare;


    //Init variables
    distanceList.clear();
    capacityList.clear();
    solution.clear();
    objectiveFunction = 0;
    penality = 0;

    sizeSol = solution.size();

    //Create sol
    for(int i = 0; i < 2; i++) route.push_back(0);
    for(int i = 0; i < vehicles; i++){
        solution.push_back(route);
        distanceList.push_back(0);
        capacityList.push_back(clientDemand[0]);
    } 
    for(int i = 1; i < dimension; i++) candidateList.push_back(i);
    candidateListSize = candidateList.size();

    //Insert initial vertices
    for(int v = 0; v < vehicles; v++){
        initialInsertInfo.cost = INT_MIN;
        if(candidateListSize < 2) break;

        for(int i = 1; i < candidateListSize; i++){
            for(int j = 0; j < candidateListSize; j++){
                if(i == j) continue;
                currentCost = distanceMatrix[0][candidateList[i]] + 
                              distanceMatrix[candidateList[i]][candidateList[j]] + 
                              distanceMatrix[candidateList[j]][0];

                if(currentCost < initialInsertInfo.cost) continue;  

                initialInsertInfo.vertice1 = candidateList[i];
                initialInsertInfo.posRemoveV1 = i;
                initialInsertInfo.vertice2 = candidateList[j];
                initialInsertInfo.posRemoveV2 = j;
                initialInsertInfo.cost = currentCost;
            }
        }
    
        solution[v].emplace(solution[v].begin()+1, initialInsertInfo.vertice1);
        candidateList.erase(candidateList.begin() + initialInsertInfo.posRemoveV1);
        
        if(initialInsertInfo.posRemoveV1 < initialInsertInfo.posRemoveV2) initialInsertInfo.posRemoveV2--;
        
        solution[v].emplace(solution[v].begin()+1, initialInsertInfo.vertice2);
        candidateList.erase(candidateList.begin() + initialInsertInfo.posRemoveV2);

        capacityList[v] += clientDemand[initialInsertInfo.vertice1] + clientDemand[initialInsertInfo.vertice2];
        distanceList[v] += initialInsertInfo.cost;
        candidateListSize = candidateListSize - 2;
    }

    //Inserte other vertices
    while(1){
        for(int i = 0; i < vehicles; i++){
            sizeSol = solution[i].size();
            for(int j = 1; j < sizeSol; j++){
                for(int k = 0; k < candidateListSize; k++){
                    insertInfoVar.vertice = candidateList[k];
                    insertInfoVar.vehicle = i;
                    insertInfoVar.posInsert = j;
                    insertInfoVar.posRemove = k;
                    insertInfoVar.cost = (distanceMatrix[solution[i][j-1]][candidateList[k]] + 
                                          distanceMatrix[candidateList[k]][solution[i][j]]) -  
                                         (distanceMatrix[solution[i][j-1]][solution[i][j]]);
                                        
                    verticeList.push_back(insertInfoVar);
                }
            }
        }

        sort(verticeList.begin(), verticeList.end(), compare);

        alphaVertices = (int)(ceil(verticeList.size() * alpha));
        if(alphaVertices <= 0) alphaVertices = 1;

        vertice = rand() % alphaVertices;
        solution[verticeList[vertice].vehicle].emplace(solution[verticeList[vertice].vehicle].begin() + verticeList[vertice].posInsert, verticeList[vertice].vertice);
        capacityList[verticeList[vertice].vehicle] += clientDemand[verticeList[vertice].vertice];
        distanceList[verticeList[vertice].vehicle] += verticeList[vertice].cost;
        candidateList.erase(candidateList.begin() + verticeList[vertice].posRemove);
        candidateListSize--;
        verticeList.clear();

        if(candidateListSize == 0) break;
    }

    objectiveFunction = penality = 0;
    for(int i = 0; i < vehicles; i++){
        objectiveFunction += distanceList[i];
        if(capacityList[i] > capacity){
            penality += capacityList[i] - capacity;
        }
    } 
            
    #if defined DEBUG
        int totalSize = 0;

        cout << "Vertice list: " << endl;
        for(int i = 0; i < verticeList.size(); i++) cout << verticeList[i].cost << " ";
        cout << endl;

        cout << "Distance list: " << endl;
        for(int i = 0; i < distanceList.size(); i++) cout << distanceList[i] << " ";
        cout << endl;

        cout << "Total distance: " << endl << objectiveFunction << endl;

        cout << "Capacity list: " << endl;
        for(int i = 0; i < capacityList.size(); i++) cout << capacityList[i] << " ";
        cout << endl;

        cout << "Candidate list: " << endl;
        for(int i = 0; i < candidateListSize; i++) cout << candidateList[i] << " ";
        cout << endl;

        cout << "Penality: " << endl;
        cout << penality << endl;

        cout << "Solution: " << endl;
        for(int i = 0; i < vehicles; i++){
            for(int j = 0; j < solution[i].size(); j++){
                cout << solution[i][j] << " ";
            }  
            cout << endl;
        }
        cout << endl;
    #endif
}

void Solution::reinsertionCapacity(){
    int sizeV1 = 0, insertCost = 0, removeCost = 0;
    bool flag = false;
    insertInfo insertInfo;

    for(int v1 = 0; v1 < vehicles; v1++){
        if(capacityList[v1] <= capacity) continue;
        sizeV1 = solution[v1].size() - 1;

        for(int i = 1; i < sizeV1; i++){

            for(int v2 = 0; v2 < vehicles; v2++){
                if(v1 == v2) continue;

                if(capacityList[v2] + clientDemand[solution[v1][i]] <= capacity){
                     if(capacityList[v1] - clientDemand[solution[v1][i]] <= capacity){
                        //Recalcula custo de rota
                        removeCost =  distanceMatrix[solution[v1][i-1]][solution[v1][i+1]] - 
                                     (distanceMatrix[solution[v1][i-1]][solution[v1][i]] + distanceMatrix[solution[v1][i]][solution[v1][i+1]]);
                        
                        insertCost = (distanceMatrix[solution[v2][0]][solution[v1][i]] + distanceMatrix[solution[v1][i]][solution[v2][1]]) -
                                      distanceMatrix[solution[v2][0]][solution[v2][1]];

                        distanceList[v1] += removeCost;
                        distanceList[v2] += insertCost;
                        objectiveFunction += insertCost + removeCost;

                        //Recalcula custo de capacidade
                        int penalityRemove = capacityList[v1] - capacity;
                        int penalityInsert = capacityList[v2] - capacity;
                        
                        if(penalityRemove < 0) penalityRemove = 0;
                        if(penalityInsert < 0) penalityInsert = 0;

                        int deltaPenalityRemove = capacityList[v1] - clientDemand[solution[v1][i]] - capacity;
                        int deltaPenalityInsert = capacityList[v2] + clientDemand[solution[v1][i]] - capacity;

                        if(deltaPenalityRemove < 0) deltaPenalityRemove = 0;
                        if(deltaPenalityInsert < 0) deltaPenalityInsert = 0;

                        penality += (deltaPenalityRemove + deltaPenalityInsert) - (penalityRemove + penalityInsert);

                        capacityList[v1] -= clientDemand[solution[v1][i]];
                        capacityList[v2] += clientDemand[solution[v1][i]];

                        //Reinsere vertice
                        solution[v2].emplace(solution[v2].begin() + 1, solution[v1][i]);
                        solution[v1].erase(solution[v1].begin() + i);

                        return;
                     } else {
                        //Recalcula custo de rota
                        removeCost =  distanceMatrix[solution[v1][i-1]][solution[v1][i+1]] - 
                                     (distanceMatrix[solution[v1][i-1]][solution[v1][i]] + distanceMatrix[solution[v1][i]][solution[v1][i+1]]);
                        
                        insertCost = (distanceMatrix[solution[v2][0]][solution[v1][i]] + distanceMatrix[solution[v1][i]][solution[v2][1]]) -
                                      distanceMatrix[solution[v2][0]][solution[v2][1]];

                        insertInfo.vertice = solution[v1][i];
                        insertInfo.vehicleInsert = v2;
                        insertInfo.vehicleRemove = v1;
                        insertInfo.posRemove = i;
                        insertInfo.costInsert = insertCost;
                        insertInfo.costRemove = removeCost;

                        flag = true;
                     }
                }        
            }
        }
    } 

    if(flag){
        distanceList[insertInfo.vehicleRemove] += removeCost;
        distanceList[insertInfo.vehicleInsert] += insertCost;
        objectiveFunction += insertInfo.costInsert + insertInfo.costRemove;   

        int penalityRemove = capacityList[insertInfo.vehicleRemove] - capacity;
        int penalityInsert = capacityList[insertInfo.vehicleInsert] - capacity;
        
        if(penalityRemove < 0) penalityRemove = 0;
        if(penalityInsert < 0) penalityInsert = 0;

        int deltaPenalityRemove = capacityList[insertInfo.vehicleRemove] - clientDemand[solution[insertInfo.vehicleRemove][insertInfo.posRemove]] - capacity;
        int deltaPenalityInsert = capacityList[insertInfo.vehicleInsert] + clientDemand[solution[insertInfo.vehicleRemove][insertInfo.posRemove]] - capacity;

        if(deltaPenalityRemove < 0) deltaPenalityRemove = 0;
        if(deltaPenalityInsert < 0) deltaPenalityInsert = 0;

        penality += (deltaPenalityRemove + deltaPenalityInsert) - (penalityRemove + penalityInsert);

        capacityList[insertInfo.vehicleRemove] -= clientDemand[solution[insertInfo.vehicleRemove][insertInfo.posRemove]];
        capacityList[insertInfo.vehicleInsert] += clientDemand[solution[insertInfo.vehicleRemove][insertInfo.posRemove]];

        solution[insertInfo.vehicleInsert].emplace(solution[insertInfo.vehicleInsert].begin() + 1, solution[insertInfo.vehicleRemove][insertInfo.posRemove]);
        solution[insertInfo.vehicleRemove].erase(solution[insertInfo.vehicleRemove].begin() + insertInfo.posRemove); 
    }
}

void Solution::swapCapacity(){
    int sizeV1 = 0, sizeV2 = 0, insertCost = 0, removeCost = 0;
    int penalityV1 = 0, penalityV2 = 0, deltaPenalityV1 = 0, deltaPenalityV2 = 0, deltatotalPenality = 0, deltaPenality = 0;
    int swapV1Cost = 0, swapV2Cost = 0, deltaCusto = 0;
    bool flag = false;
    swapInfo swapInfo;

    for(int v1 = 0; v1 < vehicles; v1++){
        if(capacityList[v1] <= capacity) continue;

        (capacityList[v1] - capacity < 0) ? penalityV1 = 0 : penalityV1 = capacityList[v1] - capacity;
        sizeV1 = solution[v1].size() - 1;

        for(int i = 1; i < sizeV1; i++){

            for(int v2 = 0; v2 < vehicles; v2++){
                if(v1 == v2 || capacityList[v2] > capacity) continue;

                (capacityList[v2] - capacity < 0) ? penalityV2 = 0 : penalityV2 = capacityList[v2] - capacity;
                sizeV2 = solution[v2].size() - 1;

                for(int y = 1; y < sizeV2; y++){
                    swapV1Cost = (distanceMatrix[solution[v1][i-1]][solution[v2][y]] + distanceMatrix[solution[v2][y]][solution[v1][i+1]]) -
                                 (distanceMatrix[solution[v1][i-1]][solution[v1][i]] + distanceMatrix[solution[v1][i]][solution[v1][i+1]]);
                                 

                    swapV2Cost = (distanceMatrix[solution[v2][y-1]][solution[v1][i]] + distanceMatrix[solution[v1][i]][solution[v2][y+1]]) -
                                 (distanceMatrix[solution[v2][y-1]][solution[v2][y]] + distanceMatrix[solution[v2][y]][solution[v2][y+1]]);

                    deltaPenalityV1 = capacityList[v1] + clientDemand[solution[v2][y]] - clientDemand[solution[v1][i]] - capacity;
                    deltaPenalityV2 = capacityList[v2] - clientDemand[solution[v2][y]] + clientDemand[solution[v1][i]] - capacity;
                    
                    if(deltaPenalityV1 < 0) deltaPenalityV1 = 0;
                    if(deltaPenalityV2 < 0) deltaPenalityV2 = 0;

                    deltaPenality = (deltaPenalityV1 + deltaPenalityV2) - (penalityV1 + penalityV2);

                    if(deltaPenality < deltatotalPenality){
                        flag = true; 

                        deltaCusto = swapV1Cost + swapV2Cost;
                        swapInfo.totalPenality = deltatotalPenality = deltaPenality;

                        swapInfo.cost1 = swapV1Cost;
                        swapInfo.pos1 = i;
                        swapInfo.vertice1 = solution[v1][i];
                        swapInfo.vehicle1 = v1;

                        swapInfo.cost2 = swapV2Cost;
                        swapInfo.pos2 = y;
                        swapInfo.vertice2 = solution[v2][y];
                        swapInfo.vehicle2 = v2;
                    }
                }     
            }
        }
    }     

  if(flag){
    objectiveFunction = objectiveFunction + deltaCusto;
    penality += swapInfo.totalPenality;

    distanceList[swapInfo.vehicle1] += swapInfo.cost1;
    distanceList[swapInfo.vehicle2] += swapInfo.cost2;

    capacityList[swapInfo.vehicle1] += clientDemand[swapInfo.vertice2] - clientDemand[swapInfo.vertice1];
    capacityList[swapInfo.vehicle2] += clientDemand[swapInfo.vertice1] - clientDemand[swapInfo.vertice2];

    solution[swapInfo.vehicle1].erase(solution[swapInfo.vehicle1].begin() + swapInfo.pos1);
    solution[swapInfo.vehicle1].emplace(solution[swapInfo.vehicle1].begin() +  swapInfo.pos1,  swapInfo.vertice2);

    solution[swapInfo.vehicle2].erase(solution[swapInfo.vehicle2].begin() + swapInfo.pos2);
    solution[swapInfo.vehicle2].emplace(solution[swapInfo.vehicle2].begin() +  swapInfo.pos2,  swapInfo.vertice1);

    flag = false;
    deltaCusto = 0;
  }
}

void Solution::reinsertion(int verticesQtd, bool pertub){
    // Inicia variáveis
    vector<int> aux;
    int deltaCost = 0, withdrawalCost = 0, insertionCost = 0, solutionSize = 0;
    int iterSize = 0, deltaCapacity = 0, capacityCost = 0;
    bool flag = false, viabilitFlag = false;
    insertInfo insertion;

    if(pertub){
        //Escolhe local randomicamente
        int i, j, k, l;

        while(1){
            i = rand() % vehicles;
            k = rand() % vehicles;

            iterSize = solution[i].size();
            if((iterSize - verticesQtd - 1) < 1) continue;
            
            j = rand() % (iterSize - verticesQtd - 1) + 1;
            l = rand() % (solution[k].size() - 1) + 1;

            if(i == k && l >= j && l <= j+verticesQtd) continue;
            break;
        }

        int costR = distanceMatrix[solution[i][j-1]][solution[i][j+verticesQtd]] - (
                    distanceMatrix[solution[i][j-1]][solution[i][j]] + 
                    distanceMatrix[solution[i][j+verticesQtd-1]][solution[i][j+verticesQtd]]);

        int costI = (distanceMatrix[solution[k][l-1]][solution[i][j]] +
                    distanceMatrix[solution[i][j+verticesQtd-1]][solution[k][l]]) -
                    distanceMatrix[solution[k][l-1]][solution[k][l]];
        
        int v = solution[i][j];

        insertion.vertice = v;

        insertion.posRemove = j;
        insertion.vehicleRemove = i;
        insertion.costRemove = costR;

        insertion.posInsert = l;
        insertion.vehicleInsert = k;
        insertion.costInsert = costI;

        flag = true;
    } else {
        //Procura local de melhor reinserção
        for(int i = 0; i < vehicles; i++){
            iterSize = solution[i].size() - verticesQtd;

            for(int j = 1; j < iterSize; j++){
                withdrawalCost = distanceMatrix[solution[i][j-1]][solution[i][j+verticesQtd]] - (
                                 distanceMatrix[solution[i][j-1]][solution[i][j]] + 
                                 distanceMatrix[solution[i][j+verticesQtd-1]][solution[i][j+verticesQtd]]
                                );

                for(int k = 0; k < vehicles; k++){
                    solutionSize = solution[k].size();

                    if(i != k){
                        int costTest = capacityList[k], costTest2 = capacityList[i];
                        bool capacityFlag = false;

                        for(int l = 0; l < verticesQtd; l++){
                            costTest2 -= clientDemand[solution[i][j+l]];
                            costTest += clientDemand[solution[i][j+l]];
                            if(costTest > capacity){
                                capacityFlag = true;
                                break;
                            }
                        }

                        if(capacityFlag) continue;
                    }

                    for(int l = 1; l < solutionSize; l++){
                        if(i == k && l >= j && l <= j+verticesQtd) continue;

                        insertionCost = (distanceMatrix[solution[k][l-1]][solution[i][j]] +
                                         distanceMatrix[solution[i][j+verticesQtd-1]][solution[k][l]]) -
                                         distanceMatrix[solution[k][l-1]][solution[k][l]];

                        if((insertionCost + withdrawalCost) < deltaCost){
                            flag = true;
                            deltaCost = insertionCost + withdrawalCost;
                            //cout << insertionCost << " " << withdrawalCost << endl;
                            insertion.vertice = solution[k][i];
                            //cout << deltaCost << endl;

                            insertion.posRemove = j;
                            insertion.vehicleRemove = i;
                            insertion.costRemove = withdrawalCost;

                            insertion.posInsert = l;
                            insertion.vehicleInsert = k;
                            insertion.costInsert = insertionCost;
                        }
                    }
                }
            }
        }
    }    

    //Aplica reinserção
    if(flag){ 
        int clientsCapacityReinsert = 0;
        int deltaPenalityRemove = 0, deltaPenalityInsert = 0;
        int penalityRemove = capacityList[insertion.vehicleRemove] - capacity;
        int penalityInsert = capacityList[insertion.vehicleInsert] - capacity;
        
        if(penalityRemove < 0) penalityRemove = 0;
        if(penalityInsert < 0) penalityInsert = 0;

        viabilitFlag == false;
        for(int i = 0; i < verticesQtd; i++){
            clientsCapacityReinsert += clientDemand[solution[insertion.vehicleRemove][insertion.posRemove]];

            if(i < verticesQtd-1){
                distanceList[insertion.vehicleRemove] -= distanceMatrix[solution[insertion.vehicleRemove][insertion.posRemove]]
                                                                       [solution[insertion.vehicleRemove][insertion.posRemove+1]]; 
                distanceList[insertion.vehicleInsert] += distanceMatrix[solution[insertion.vehicleRemove][insertion.posRemove]]
                                                                       [solution[insertion.vehicleRemove][insertion.posRemove+1]];
            }

            aux.push_back(solution[insertion.vehicleRemove][insertion.posRemove]);
            solution[insertion.vehicleRemove].erase(solution[insertion.vehicleRemove].begin() + insertion.posRemove);
        }

        capacityList[insertion.vehicleRemove] -= clientsCapacityReinsert;
        capacityList[insertion.vehicleInsert] += clientsCapacityReinsert;

        deltaPenalityRemove = capacityList[insertion.vehicleRemove] - capacity;
        deltaPenalityInsert = capacityList[insertion.vehicleInsert] - capacity;

        if(deltaPenalityRemove < 0) deltaPenalityRemove = 0;
        if(deltaPenalityInsert < 0) deltaPenalityInsert = 0;

        penality += (deltaPenalityRemove + deltaPenalityInsert) - (penalityRemove + penalityInsert);

        if(insertion.vehicleRemove == insertion.vehicleInsert && 
           insertion.posInsert > insertion.posRemove) insertion.posInsert -= verticesQtd; 

        solution[insertion.vehicleInsert].insert(solution[insertion.vehicleInsert].begin() + insertion.posInsert, aux.begin(), aux.end());
        distanceList[insertion.vehicleRemove] += insertion.costRemove;
        distanceList[insertion.vehicleInsert] += insertion.costInsert;
        objectiveFunction += (insertion.costRemove + insertion.costInsert);

        deltaCost = 0;
        flag = true; // ???
    }   

    #if defined LSDEBUG
        int totalSize = 0;

        cout << endl << "REINSERTION" << endl;
        cout << "Distance list: " << endl;
        for(int i = 0; i < distanceList.size(); i++){
            cout << distanceList[i] << " ";
            totalSize += distanceList[i];
        }
        cout << endl;

        cout << "Total distance: " << endl << totalSize << endl;

        cout << "Capacity list: " << endl;
        for(int i = 0; i < capacityList.size(); i++) cout << capacityList[i] << " ";
        cout << endl;

        cout << "Solution: " << endl;
        for(int i = 0; i < vehicles; i++){
            for(int j = 0; j < solution[i].size(); j++){
                cout << solution[i][j] << " ";
            }  
            cout << endl;
        }
    #endif

}

void Solution::swap(bool pertub){
    int solucaoSize = 0, initFor = 0, swapV1Cost = 0, swapV2Cost = 0, deltaCusto = 0;
    int sizeV2 = 0, sizeV1 = 0;
    bool flag = false;
    swapInfo swapInfo;
    
    if(pertub){
        int v1, v2, i, y;

        while(1){
            v1 = rand() % vehicles;
            if(solution[v1].size() - 2 < 1) continue;

            v2 = rand() % vehicles;
            if(solution[v2].size() - 2 < 1) continue;

            i = rand() % (solution[v1].size() - 2) + 1;
            y = rand() % (solution[v2].size() - 2) + 1;

            if(v1 == v2 && (i - 1 == y || i + 1 == y || i == y)) continue;
            break;
        }

        swapV1Cost = (distanceMatrix[solution[v1][i-1]][solution[v2][y]] + distanceMatrix[solution[v2][y]][solution[v1][i+1]]) -
                     (distanceMatrix[solution[v1][i-1]][solution[v1][i]] + distanceMatrix[solution[v1][i]][solution[v1][i+1]]);
        
        swapV2Cost = (distanceMatrix[solution[v2][y-1]][solution[v1][i]] + distanceMatrix[solution[v1][i]][solution[v2][y+1]]) -
                     (distanceMatrix[solution[v2][y-1]][solution[v2][y]] + distanceMatrix[solution[v2][y]][solution[v2][y+1]]);

        swapInfo.totalCost = deltaCusto = swapV1Cost + swapV2Cost;

        swapInfo.cost1 = swapV1Cost;
        swapInfo.pos1 = i;
        swapInfo.vertice1 = solution[v1][i];
        swapInfo.vehicle1 = v1;

        swapInfo.cost2 = swapV2Cost;
        swapInfo.pos2 = y;
        swapInfo.vertice2 = solution[v2][y];
        swapInfo.vehicle2 = v2;

        flag = true; 
    } else {
        for(int v1 = 0; v1 < vehicles; v1++){
            sizeV1 = solution[v1].size();

            for(int i = 1; i < sizeV1-1; i++){

                for(int v2 = 0; v2 < vehicles; v2++){
                    sizeV2 = solution[v2].size();
                    (v1 == v2) ? initFor = i + 2 : initFor = 1;

                    for(int y = initFor; y < sizeV2 - 1; y++){
                        if(capacityList[v1] + clientDemand[solution[v2][y]] - clientDemand[solution[v1][i]] > capacity ||
                           capacityList[v2] - clientDemand[solution[v2][y]] + clientDemand[solution[v1][i]] > capacity) continue;

                        swapV1Cost = (distanceMatrix[solution[v1][i-1]][solution[v2][y]] + distanceMatrix[solution[v2][y]][solution[v1][i+1]]) -
                                    (distanceMatrix[solution[v1][i-1]][solution[v1][i]] + distanceMatrix[solution[v1][i]][solution[v1][i+1]]);
                                    

                        swapV2Cost = (distanceMatrix[solution[v2][y-1]][solution[v1][i]] + distanceMatrix[solution[v1][i]][solution[v2][y+1]]) -
                                    (distanceMatrix[solution[v2][y-1]][solution[v2][y]] + distanceMatrix[solution[v2][y]][solution[v2][y+1]]);

                        if((swapV1Cost + swapV2Cost) < deltaCusto){
                            flag = true; 

                            deltaCusto = swapV1Cost + swapV2Cost;

                            swapInfo.cost1 = swapV1Cost;
                            swapInfo.pos1 = i;
                            swapInfo.vertice1 = solution[v1][i];
                            swapInfo.vehicle1 = v1;

                            swapInfo.cost2 = swapV2Cost;
                            swapInfo.pos2 = y;
                            swapInfo.vertice2 = solution[v2][y];
                            swapInfo.vehicle2 = v2;
                            
                            swapInfo.totalCost = deltaCusto;
                        }
                    }
                }
            }
        }
    }

    if(flag){
        int clientsCapacityReinsert = 0;
        int deltaPenalityV1 = 0, deltaPenalityV2 = 0;
        int penalityV1 = capacityList[swapInfo.vehicle1] - capacity;
        int penalityV2 = capacityList[swapInfo.vehicle2] - capacity;

        if(penalityV1 < 0) penalityV1 = 0;
        if(penalityV2 < 0) penalityV2 = 0;

        objectiveFunction = objectiveFunction + deltaCusto;

        distanceList[swapInfo.vehicle1] += swapInfo.cost1;
        distanceList[swapInfo.vehicle2] += swapInfo.cost2;

        capacityList[swapInfo.vehicle1] += clientDemand[swapInfo.vertice2] - clientDemand[swapInfo.vertice1];
        capacityList[swapInfo.vehicle2] += clientDemand[swapInfo.vertice1] - clientDemand[swapInfo.vertice2];

        deltaPenalityV1 = capacityList[swapInfo.vehicle1] - capacity;
        deltaPenalityV2 = capacityList[swapInfo.vehicle2] - capacity;

        if(deltaPenalityV1 < 0) deltaPenalityV1 = 0;
        if(deltaPenalityV2 < 0) deltaPenalityV2 = 0;

        penality += (deltaPenalityV1 + deltaPenalityV2) - (penalityV1 + penalityV2);
        
        solution[swapInfo.vehicle1].erase(solution[swapInfo.vehicle1].begin() + swapInfo.pos1);
        solution[swapInfo.vehicle1].emplace(solution[swapInfo.vehicle1].begin() + swapInfo.pos1,  swapInfo.vertice2);

        solution[swapInfo.vehicle2].erase(solution[swapInfo.vehicle2].begin() + swapInfo.pos2);
        solution[swapInfo.vehicle2].emplace(solution[swapInfo.vehicle2].begin() + swapInfo.pos2,  swapInfo.vertice1);

        flag = false;
        deltaCusto = 0;
    }
}

void Solution::orOpt2(){
}

void Solution::pertub(int pertubFactor){
    vector<int> neighborhood;
    int neighbor;

    for(int i = 0; i < 4; i++) neighborhood.push_back(i);

    for(int i = 0; i < pertubFactor; i++){
        neighbor = rand() % neighborhood.size();

        switch (neighbor){
            case 0:
                reinsertion(1, true);
                break;

            case 1:
                reinsertion(2, true);
                break;

            case 2:
                reinsertion(3, true);
                break;
            
            case 3:
                swap(true);
                break;
            
            default:
                break;
        }
    }
}

int Solution::getObjectiveFunction(){
    return objectiveFunction;
}

int Solution::getPenality(){
    return penality;
}

void Solution::showLog(){
    int totalSize = 0;

    cout << endl << "FINAL LOG" << endl;
    cout << "Cost per vehicle " << endl;
    for(int i = 0; i < distanceList.size(); i++){
        cout << distanceList[i] << " ";
        totalSize += distanceList[i];
    }
    cout << endl;

    cout << "Total cost: " << endl << totalSize << endl;

    cout << "Capacity per vehicle: " << endl;
    for(int i = 0; i < capacityList.size(); i++) cout << capacityList[i] << " ";
    cout << endl;

    cout << "Penality: " << endl;
    cout << penality << endl;

    cout << "Route per vehicle: " << endl;
    for(int i = 0; i < vehicles; i++){
        for(int j = 0; j < solution[i].size(); j++){
            cout << solution[i][j] << " ";
        }  
        cout << endl;
    }    
}

void Solution::debugTotalCost(){
    int cost = 0, subCost = 0;

    cout << endl << "DEBUG TOTAL COST: " << endl;

    for(int i = 0; i < solution.size(); i++){
        for(int j = 1; j < solution[i].size(); j++){
            cost += distanceMatrix[solution[i][j-1]][solution[i][j]];
            subCost += distanceMatrix[solution[i][j-1]][solution[i][j]];
        }
        cout << subCost << " ";
        subCost = 0;
    }

    cout << endl << cost << endl << endl;
}

void Solution::debugCapacity(){
    int cost = 0, penality = 0;

    cout << endl << "DEBUG TOTAL CAPACITY: " << endl;

    for(int i = 0; i < solution.size(); i++){
        for(int j = 0; j < solution[i].size(); j++){
            cost += clientDemand[solution[i][j]];
        }
        cout << cost << " ";
        penality += (cost - capacity < 0) ? 0 : cost - capacity;
        cost = 0;
    }

    cout << endl << penality << endl << endl;   
}