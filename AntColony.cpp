#include <iostream>
#include <cmath>
#include <cassert>
#include <atomic>
#include <fstream>

#include "AntColony.h"

using namespace std;

//constructor initializes the ant and city arrays and populated the distances matrix 
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

        //load first city into the citiesVisited list
        antArray[i].citiesVisited[antArray[i].curr] = 1;        

        //finish allocating the distances while we're here
        distances[i] = new int[maxCities];

        //fill in some of the other variables
        initPhero = 1 / maxCities;
        maxTime = MAX_TOURS * maxCities; 
    }
/*
    cout << "Testing" << endl;
    for (int j = 0; j < maxCities; j++)
        cout << cityArray[j].id << endl;    
*/
    //fill in the distances matrix
    int maxDistance = 0;
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
                //check if highest distance value
                if (distances[m][n] > maxDistance)
                    maxDistance = distances[m][n];
            }
            else  
                distances[m][n] = 0;  //if same city, distance is zero

        }

    //set initial "best" as maximum possible tour
    best = maxCities * maxDistance;
} 

void AntColony::run(bool writePython)
{

}

//used to reset the ants after each tour
void AntColony::resetAnts()
{
    //int nextCity = 0;

    for(int i = 0; i < maxAnts; i++)
    {
        //check if the ant has the best path so far
        if (antArray[i].tourLength < best)
        {
            //if so, adjust best length and the index of the best ant
            best = antArray[i].tourLength;
            bestIndex = i;
        }

        antArray[i].next = -1;
        antArray[i].tourLength = 0.0;

        for(int j = 0; j < maxCities; j++)
        {
            antArray[i].citiesVisited[j] = 0;
            antArray[i].path[j] = -1;
        }

        //if (nextCity == maxCities)
        //    nextCity = 0;
        //antArray[i].curr = nextCity++;

        antArray[i].curr = cityArray[i].id;
        antArray[i].pathIndex = 1;
        antArray[i].path[0] = antArray[i].curr;
        antArray[i].citiesVisited[antArray[i].curr] = 1;        
    }
}

//gives distance as a function of concentration of pheromones and visibility
double AntColony::antProd(int source, int destination)
{
    return( pow(pheroConcentration[source][destination], ALPHA) * 
        (pow( 1.0/distances[source][destination], BETA)) );
}


//function for randomly determining the next city to visit, using probability
//and the equation provided by the antProd(int, int) function above.
int AntColony::nextCity(int ant)
{
    int destination;
    int source = antArray[ant].curr;
    double denom = 0.0; //sum of heuristic of edges not traveled yet

    for(destination = 0; destination < maxCities; destination++)
        //if the city hasn't been visited yet
        if (antArray[ant].citiesVisited[destination] == 0)
                denom += antProd(source, destination); //summing values from edges
    assert( denom != 0.0);
    
    do
    {
        double p;
        destination++;
        if (destination >= maxCities)
            destination = 0;
        if(antArray[ant].citiesVisited[destination] == 0)
        {
            //intensity of pheromone along edge divided by heuristic sum from 
            //edges not yet traversed
            p = antProd(source, destination) / denom;
            
            double x = ((double)rand()/RAND_MAX);
            if(x < p)
                break;
        }
    } while(true); //keep doing this until a break is generated

    return destination;
}

//sumilate the ants moving to the next city and recording the distances
int AntColony::antSim()
{   
    //make moving thread-safe
    std::atomic<int> moving;// 
    moving = 0;

    for(int i = 0; i < maxAnts; i++)
    {
        //verify of more cities left to visit
        if(antArray[i].pathIndex < maxCities)
        {
            antArray[i].next = nextCity(i); //get a new next city
            antArray[i].citiesVisited[antArray[i].next] = 1; //label it as visited
            antArray[i].path[antArray[i].pathIndex++] = antArray[i].next; //make it the nex one on the path
            antArray[i].tourLength += distances[antArray[i].curr][antArray[i].next];

            //handle last case: going back to starting point
            if(antArray[i].curr == maxCities)
                antArray[i].tourLength += distances[antArray[i].path[maxCities - 1]]
                    [antArray[i].path[0]];

            antArray[i].curr = antArray[i].next;
            moving++;
        }
    }
    
    return moving;
}

void AntColony::updateTrails()
{
    int source, dest;

    //handle pheromone evaporation
    for(source = 0; source < maxCities; source++)
        for(dest = 0; dest < maxCities; dest++)
            if (source != dest)
            {
                //evaporate pheromones
                pheroConcentration[source][dest] *= (1.0 - RHO);

               if (pheroConcentration[source][dest] < 0.0)
                  pheroConcentration[source][dest] = initPhero; 
            }

    //add new pheromones to the trails
    for(int i = 0; i < maxAnts; i++)
    {
        for (int j = 0; j < maxCities; j++)
        {
            if(j < (maxCities - 1))
            {
                source = antArray[i].path[j];
                dest = antArray[i].path[j+1];
            }
            else
            {
                source = antArray[i].path[j];
                dest = antArray[i].path[0];
            }

            pheroConcentration[source][dest] += (QVAL / antArray[i].tourLength);
            pheroConcentration[dest][source] = pheroConcentration[source][dest];
        }
    }

    for(int m = 0; m < maxCities; m++)
        for(int n = 0; n < maxCities; n++)
            pheroConcentration[m][n] *= RHO;
}

//write the data
void AntColony::writeData(int)
{

}

//write the data to a file that can be read by the python script described in the
//"A Positronic Brain" blog
void AntColony::writeDataForPython()
{
    ofstream f1;
    f1.open("Data.txt");
    ant bestAnt;
    bestAnt = antArray[bestIndex];
    
    for(int i = 0; i < maxCities; i++)
        f1 << bestAnt.path[i] << " ";
    f1.close();

    f1.open("CityData.txt");
    for(int j = 0; j < maxCities; j++)
        f1 << cityArray[j].x << " " << cityArray[j].y << "\n";
    f1.close();

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

