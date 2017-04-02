#include <iostream>
#include <cassert>
#include <fstream>
#include <cstdio>
#include <thread>

#include "AntColony.h"

//constructor initializes the ant and city arrays and populated the distances matrix 
AntColony::AntColony(std::vector< std::vector<int> >* cities)
{
    //allocate cities and ants  
    maxCities = cities->size();
    if (maxCities < MAX_ANTS)
        maxAnts = maxCities;
    else
        maxAnts = MAX_ANTS;
    cityArray.resize(maxCities);
    antArray.resize(maxAnts);
    
    //fill in some of the other variables
    maxTime = MAX_TOURS * maxCities; 
    initPhero = 1.0 / maxCities;
    distances.resize(maxCities, std::vector<double>(maxCities, 0));
    pheroConcentration.resize(maxCities, std::vector<double>(maxCities, initPhero));
    
    //assign city data
    for(int i = 0; i < maxCities; i++)
    {
        cityArray[i].id = (*cities)[i][0];
        cityArray[i].x = (*cities)[i][1];
        cityArray[i].y = (*cities)[i][2];
    }

    std::vector<std::thread> antThreads(maxAnts); 
    
    //create threads to help initialize all the values    
    for(int i = 1; i < maxAnts; i++)
        antThreads[i] = std::thread(&AntColony::constructorThreads, this, i, 
            std::ref(maxCities), std::ref(cities));
        
    antThreads[0] = std::thread(&AntColony::constructorThreads, this, 0, 
            std::ref(maxCities), std::ref(cities));
   
    for(int i = 0; i < maxAnts; i++)
        antThreads[i].join(); 

    //fill in the distances matrix
    double maxDistance = 0;
    for(int m = 0; m < maxCities; m++)
    { 
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
        }
    }

    //set initial "best" as maximum possible tour
    best = maxCities * maxDistance;
} 

void AntColony::constructorThreads(int i, int maxCities, std::vector< std::vector<int> >* cities)
{
    //antArray[i].curr = (*cities)[i][0];
    antArray[i].citiesVisited = new int[maxCities];
    antArray[i].path = new int[maxCities];
    antArray[i].pathIndex = 1;
    antArray[i].next = -1;
    antArray[i].tourLength = 0.0;

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
}


//run the simulation maxTime times, which is a function of the number of cities 
//and MAX_TOURS
void AntColony::run()
{
    maxCities = distances.size();
    int currTic = 0;
    srand(time(NULL));

    while(currTic < maxTime)
    {
        currTic++;

        if(antSim() == 0)
        {
            updateTrails();
            if(currTic != maxTime)
                resetAnts();
            printf("currTic: %d Best: %f\n", currTic, best); 
        }
    }

    std::cout << "Best tour: " << best << std::endl;
}

//used to reset the ants after each tour
//TODO: multithread this ?
void AntColony::resetAnts()
{
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

        antArray[i].curr = cityArray[i].id;
        antArray[i].pathIndex = 1;
        antArray[i].path[0] = antArray[i].curr;
        antArray[i].citiesVisited[antArray[i].curr] = 1;        
    }
}

//function for randomly determining the next city to visit, using probability
//and the equation provided by the antProd(int, int) function above.
int AntColony::nextCity(int ant)
{
    int destination;
    int source = antArray[ant].curr;
    double denom = 0.0; //sum of heuristic of edges not traveled yet
    for(destination = 0; destination < maxCities; destination++)
    {
        //if the city hasn't been visited yet
        if (antArray[ant].citiesVisited[destination] == 0)
        {
                denom += antProd(source, destination); //summing values from edges
        }
    }
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
            
            double x = ( (double)rand()/ RAND_MAX );
            if(x < p || p == 0)
                break;
        }
    } while(true); //keep doing this until a break is generated
    return destination;
}

//simulate the ants moving to the next city and recording the distances
int AntColony::antSim()
{   
    //make moving thread-safe
    std::atomic<int> moving;// 
    moving = 0;

    //create the thread vector
    std::vector<std::thread> antThreads(maxAnts); 

    for(int i = 1; i < maxAnts; i++)
        //using a lambda function for the thread constructor, passing into it i,
        //a reference to the atomic int to increment, and the class object.
        //Downside is code gets repeated for the main thread.
        antThreads[i] = std::thread([this, i, &moving]()
        {
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
        }); //end lambda for std::thread

    //main thread does work too
    //verify of more cities left to visit
    if(antArray[0].pathIndex < maxCities)
    {
        antArray[0].next = nextCity(0); //get a new next city
        antArray[0].citiesVisited[antArray[0].next] = 1; //label it as visited
        antArray[0].path[antArray[0].pathIndex++] = antArray[0].next; //make it the nex one on the path
        antArray[0].tourLength += distances[antArray[0].curr][antArray[0].next];

        //handle last case: going back to starting point
        if(antArray[0].curr == maxCities)
            antArray[0].tourLength += distances[antArray[0].path[maxCities - 1]]
                [antArray[0].path[0]];

        antArray[0].curr = antArray[0].next;
        moving++;
    }

    for(int i = 1; i < maxAnts; i++)
        antThreads[i].join();//sometimes freezes here

    return moving;
}

//multithreaded version of the ant sim - moves ants along path, tracking cities
//visited and total length traveled 
void AntColony::antSimThreaded(int i, std::atomic<int> moving)
{
            if(antArray[i].pathIndex < maxCities)
            {
                antArray[i].next = nextCity(i); //get a new next city
                antArray[i].citiesVisited[antArray[i].next] = 1; //label it as visited
                antArray[i].path[antArray[i].pathIndex++] = antArray[i].next; //make it the next one on the path
                antArray[i].tourLength += distances[antArray[i].curr][antArray[i].next];

                //handle last case: going back to starting point
                if(antArray[i].curr == maxCities)
                    antArray[i].tourLength += distances[antArray[i].path[maxCities - 1]]
                        [antArray[i].path[0]];

                antArray[i].curr = antArray[i].next;
                    moving++;
            }
}

//updates the laying and the evaporation of pheromones along trails
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

            //larger tour length = less pheromones
            pheroConcentration[source][dest] += (QVAL / antArray[i].tourLength);
            pheroConcentration[dest][source] = pheroConcentration[source][dest];
        }
    }

    //evaporate again to account for new trail evaporation
    for(int m = 0; m < maxCities; m++)
        for(int n = 0; n < maxCities; n++)
            pheroConcentration[m][n] *= RHO;
}

//write the data
void AntColony::writeData(std::string fileName)
{
    fileName = fileName + ".tour";
    std::cout << "fileName: " << fileName << std::endl;
    //check if file can be opened
    std::ofstream outFile(fileName.c_str());
    std::string s = "Cannot open " + fileName;
    //if the file doesn't exist, throw an exception
    if (!outFile)
    {
        std::cout << s << std::endl;
        return;
    }

    //ant bestAnt = antArray[bestIndex];
    ant bestAnt = antArray[bestIndex];
    //output tour length
    outFile << bestAnt.tourLength << "\n";
    for(int i = 0; i < maxCities; i++)
        outFile << bestAnt.path[i] << "\n";
    outFile.close();
}

//write the data to a file that can be read by the python script described in the
//"A Positronic Brain" blog
void AntColony::writeDataForPython()
{
    std::ofstream f1;
    f1.open("Data.txt");
    ant bestAnt = antArray[bestIndex];
    
    for(int i = 0; i < maxCities; i++)
        f1 << bestAnt.path[i] << " ";
    f1.close();

    f1.open("CityData.txt");
    for(int j = 0; j < maxCities; j++)
        f1 << cityArray[j].x << " " << cityArray[j].y << "\n";
    f1.close();
}

AntColony::~AntColony()
{
    for(int i = 0; i < maxAnts; i++)
    {
        delete [] antArray[i].citiesVisited;
        delete [] antArray[i].path;
    }
}

