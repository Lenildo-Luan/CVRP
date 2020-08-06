#include "cvrp.h"

using namespace std;

CVRP::CVRP(char *archivePath){
    ifstream archive;
    vector<int> line;
    string archiveContent = {}, instanceName = {};
     
    int column = 0, matrixLine = 0, value = 0;

    dimension = INT_MAX;
    archive.open(archivePath);

    if(archive.is_open() && archive.good()){
        for(int i = 0; !archive.fail(); i++){
            archive >> archiveContent;

            if(i >= (dimension * 2 + 10)){
                //Adiciona elemento na matriz
                stringstream input(archiveContent);
                input >> value;
                
                if (input.fail()) {
                    exit(1);
                }
                
                distanceMatrix[matrixLine].push_back(value);
                column++;
                if(column == dimension){
                    matrixLine++;
                    column = 0;
                } 
                if(matrixLine == dimension) break;
            } else if(i >= 9 && i < (dimension * 2 + 9)){
                stringstream input(archiveContent); 
                input >> value;
                if((i%2) == 0) clientDemand.push_back(value);
            } else if(i == 1){
                instanceName = archiveContent;
            } else if(i == 3){
                stringstream input(archiveContent); 
                input >> value;
                dimension = value;
                for(int j = 0; j < dimension; j++) distanceMatrix.push_back(line); 
            } else if(i == 5){
                stringstream input(archiveContent);  
                input >> value;
                vehicles = value;
            } else if(i == 7){
                stringstream input(archiveContent);  
                input >> value;
                capacity = value;
            }
        }

        currentSol = Solution(distanceMatrix, clientDemand, dimension, vehicles, capacity);

        #if defined DEBUG
            cout << "Instance: " << instanceName << endl;
            cout << "Dimension: " << dimension << endl;
            cout << "Vehicles: " << vehicles << endl;
            cout << "Capacity: " << capacity << endl;
            cout << "Demand: " << endl;
            for(int i = 0; i < dimension; i++) cout << clientDemand[i] << " ";
            cout << endl << "Distance: " << endl;
            for(int i = 0; i < dimension; i++){
                for(int j = 0; j < dimension; j++){
                    cout << distanceMatrix[i][j];
                    if(distanceMatrix[i][j] < 10) cout << "  ";
                    else cout << " ";
                }
                cout << endl;
            }
            cout << endl;
        #endif
          
        archive.close();
    }
}

CVRP::~CVRP(){
}

void CVRP::GILS_RVND(){
    double alpha = 0.9;
    int pertubFactor = 0; 
    currentSol.initialSolution(alpha);
    finalSol = currentSol;

    cout <<"Alpha: " << alpha << endl;
    
    for(int i = 0; i < 50; i++){
        pertubFactor = 4 - ceil(i/10);
        if(pertubFactor <= 0) pertubFactor = 1;
        
        ILS(pertubFactor);

        cout <<"Perturb factor: " << pertubFactor << endl;
        cout <<"Best    Sol:" << bestSol.getObjectiveFunction() << " || " << bestSol.getPenality() << endl;
        cout <<"Current Sol:" << finalSol.getObjectiveFunction() << " || " << finalSol.getPenality() << endl;
        cout << i*2 << "% complete..." << endl << endl;

        if(bestSol.getObjectiveFunction() < finalSol.getObjectiveFunction() &&
           bestSol.getPenality() <= finalSol.getPenality()){
            finalSol = bestSol;
        }

        alpha = 1 - ((double)(i*2)/100);
        cout <<"Alpha: " << alpha << endl;

        currentSol.initialSolution(alpha);
    }

    finalSol.showLog();
}

void CVRP::ILS(int perturbFactor){
    int iterILS = 0;
    int maxIlsIter = (dimension > 150) ? dimension/2 : dimension;

    bestSol = currentSol;

    while(iterILS <= dimension){
        RVND_C();
        RVND();

        if(currentSol.getObjectiveFunction() < bestSol.getObjectiveFunction() &&
           currentSol.getPenality() <= bestSol.getPenality()){
            bestSol = currentSol;
            iterILS = 0;
        }
        
        currentSol = bestSol;
        currentSol.pertub(perturbFactor);

        iterILS++;
    }
}

void CVRP::RVND_C(){
    vector<int> neighborhood;
    int neighbor;

    lsSol = currentSol;

    for(int i = 0; i < 2; i++) neighborhood.push_back(i);

    while(1){
        neighbor = rand() % neighborhood.size();

        switch (neighbor){
            case 0:
                lsSol.reinsertionCapacity();
                break;

            case 1:
                lsSol.swapCapacity();
                break;
            
            default:
                break;
        }

        if(lsSol.getPenality() < currentSol.getPenality()){
            currentSol = lsSol;
            if(currentSol.getPenality() == 0) break;

            neighborhood.clear();
            for(int i = 0; i < 2; i++) neighborhood.push_back(i);
        } else {
            neighborhood.erase(neighborhood.begin() + neighbor);
            if(!neighborhood.size()) break; 
        }
    }
}

void CVRP::RVND(){
    vector<int> neighborhood;
    int neighbor;

    lsSol = currentSol;

    for(int i = 0; i < 4; i++) neighborhood.push_back(i);

    while(1){
        neighbor = rand() % neighborhood.size();

        switch (neighbor){
            case 0:
                lsSol.reinsertion(1, false);
                break;

            case 1:
                lsSol.reinsertion(2, false);
                break;

            case 2:
                lsSol.reinsertion(3, false);
                break;
            
            case 3:
                lsSol.swap(false);
                break;
            
            default:
                break;
        }

        if(lsSol.getObjectiveFunction() < currentSol.getObjectiveFunction()){
            currentSol = lsSol;
            neighborhood.clear();
            for(int i = 0; i < 4; i++) neighborhood.push_back(i);
        } else {
            neighborhood.erase(neighborhood.begin() + neighbor);
            if(!neighborhood.size()) break; 
        }
    }
}  