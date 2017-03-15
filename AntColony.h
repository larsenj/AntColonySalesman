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
        void run(bool);
        ~AntColony();

    private:
        int maxCities;                  //# of cities
        int maxAnts;                    //# of ants
        ant* antArray;                  //store the ant structs
        city* cityArray;                //store the city structs
        //TODO: change distances to double
        int** distances;             //distances b/w cities
        double** pheroConcentration;    //pheromone concentrations b/w cities
        double best;
        int bestIndex;
        void resetAnts();
        double antResult(int, int);
        int nextCity(int);
        int antSim();
        void updateTrails();
        void writeData(int);
        void writeDataForPython(int);
};


#endif
