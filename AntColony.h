#ifndef ANTCOLONY_H
#define ANTCOLONY_H

#include <vector>

struct ant
{
    int curr, next, pathIndex;
    int* citiesVisited;
    int* path;
    double tourLength;
};

struct city
{
    int id, x, y;
};

class AntColony
{
    public:
        //constructor
        AntColony(std::vector< std::vector<int> >);
        void run();
        void writeData(std::string);
        void writeDataForPython();
        int maxCities;                  //# of cities
        ~AntColony();

    private:
        const float ALPHA = 1.0;        //importance of trail
        const float BETA = 5.0;         //relative importance of visibility
        const float RHO = 0.5;          //evaporation rate for pheromones 
        const float QVAL = 100;         //constant used in formula
        const int MAX_TOURS = 20;        //each ant does at most 20 tours


        int maxAnts;                    //# of ants
        int maxTime;                    //to stop ants if they're taking too long
        double initPhero;                  //initial weight of edges

        ant* antArray;                  //store the ant structs
        city* cityArray;                //store the city structs
        //TODO: change distances to double
        std::vector< std::vector<int> > distances;  //distances b/w cities
        //int** distances;                            //distances b/w cities
        std::vector< std::vector<double> >  pheroConcentration;    //pheromone concentrations b/w cities
        //double** pheroConcentration;    //pheromone concentrations b/w cities
        double best;
        int bestIndex;
        
        void resetAnts();
        double antProd(int, int);
        int nextCity(int);
        int antSim();
        void updateTrails();
};


#endif
