#ifndef ANTCOLONY_H
#define ANTCOLONY_H

#include <vector>
#include <atomic>
#include <cmath>

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
        AntColony(std::vector< std::vector<int> >*);
        void run();
        void writeData(std::string);
        void writeDataForPython();
        ~AntColony();

    private:
        const float ALPHA = 1.0;        //importance of trail
        const float BETA = 5.0;         //relative importance of visibility
        const float RHO = 0.5;          //evaporation rate for pheromones 
        const float QVAL = 100;         //constant used in formula
        const int MAX_TOURS = 15;        //each ant does at most 20 tours
        const int MAX_ANTS = 10;

        int maxAnts;                    //# of ants
        int maxTime;                    //to stop ants if they're taking too long
        double initPhero;                  //initial weight of edges
        std::vector<city> cityArray;                //store the city structs
        std::vector<ant> antArray;                  //store the ant structs
        std::vector< std::vector<double> > distances;  //distances b/w cities
        std::vector< std::vector<double> > pheroConcentration;    //pheromone concentrations b/w cities
        double best;
        int bestIndex;
        int maxCities;                  //# of cities

        void resetAnts();
        int nextCity(int);
        int antSim();
        void updateTrails();
        void antSimThreaded(int, std::atomic<int>);
        void constructorThreads(int, int, std::vector< std::vector<int> >*); 
        
        inline double antProd(int source, int destination)
        {
            double x = pow(pheroConcentration[source][destination], ALPHA);
            double y = pow( 1.0/distances[source][destination], BETA);
            return x*y;
        };
};


#endif
