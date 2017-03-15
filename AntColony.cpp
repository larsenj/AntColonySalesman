#include <iostream>
#include <cmath>

#include "AntColony.h"

using namespace std;

AntColony::AntColony(std::vector< std::vector<int> > cities)
{
    //allocate cities, ants and part of distances arrays 
    int maxCities = cities.size();
    int maxAnts = maxCities;
    cityArray = new city[maxCities];
    antArray = new ant[maxAnts];
    distances = new int*[maxCities];
    
    for(int i = 0; i < maxCities; i++)
    {
        //assign city and ant data
        cityArray[i].id = cities[i][0];
        cityArray[i].x = cities[i][1];
        cityArray[i].y = cities[i][2];
        antArray[i].curr = cities[i][0];
        antArray[i].citiesVisited = new int[maxCities];
        antArray[i].path = new int[maxCities];
        antArray[i].pathIndex = 1;
        antArray[i].next = -1;
        antArray[i].tourLength = 0;

        //init values for cities visited and path arrays
        for(int k = 0; k < maxCities; k++)
        {
            antArray[i].citiesVisited[k] = 0;
            antArray[i].path[k] = -1;
        }

        //first path item is the current city;
        antArray[i].path[0] = antArray[i].curr;
        

        //finish allocating the distances while we're here
        distances[i] = new int[maxCities];
    }
/*
    cout << "Testing" << endl;
    for (int j = 0; j < maxCities; j++)
        cout << cityArray[j].id << endl;    
*/
    //fill in the distances matrix
    for(int m = 0; m < maxCities; m++)
        for(int n = 0; n < maxCities; n++)
        {
            if (m != n)
            {
                //use distance formula to get the distances b/w cities
                double xDist = pow( (cityArray[m].x - cityArray[n].x), 2);
                double yDist = pow( (cityArray[m].y - cityArray[n].y), 2);
                double sum = xDist + yDist;
                distances[m][n] = sqrt(sum);
            }
            else  
                distances[m][n] = 0;  //if same city, distance is zero
        }
}

void AntColony::run(bool writePython)
{

}

void AntColony::resetAnts()
{

}

double AntColony::antResult(int, int)
{

}

int AntColony::nextCity(int)
{

}

int AntColony::antSim()
{

}

void AntColony::updateTrails()
{

}

//write the data
void AntColony::writeData(int)
{

}

//write the data to a file that can be read by the python script descriped in the
//"A Positronic Brain" blog
void AntColony::writeDataForPython(int)
{

}

//TODO: change EVERYTHING to vectors to get rid of this
AntColony::~AntColony()
{
    for(int i = 0; i < maxCities; i++)
    {
        delete [] antArray[i].citiesVisited;
        delete [] antArray[i].path;
        delete [] distances[i];
    }
    delete [] antArray;
    delete [] cityArray;
    delete [] distances;
}

