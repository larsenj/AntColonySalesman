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
  //  cout << "main@1" << endl;
    //allocate cities, ants and part of distances arrays 
    int maxCities = cities.size();
    int maxAnts = maxCities;
    cityArray = new city[maxCities];
    antArray = new ant[maxAnts];
    //fill in some of the other variables
    maxTime = MAX_TOURS * maxCities; 
    initPhero = 1.0 / maxCities;
    //matrix.resize(no_of_rows, std::vector<int>(no_of_cols, initial_value));
    distances.resize(maxCities, std::vector<int>(maxCities, 0));
    pheroConcentration.resize(maxCities, std::vector<double>(maxCities, initPhero));
//    int** distances = new int*[maxCities];
//    double** pheroConcentration = new double*[maxCities];
  //  cout << "main@2" << endl;
    
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
  //  cout << "main@3" << endl;

        //init values for cities visited and path arrays
        for(int k = 0; k < maxCities; k++)
        {
            antArray[i].citiesVisited[k] = 0;
            antArray[i].path[k] = -1;
        }
//    cout << "main@4" << endl;

        //first path item is the current city;
        antArray[i].path[0] = antArray[i].curr;

        //load first city into the citiesVisited list
        antArray[i].citiesVisited[antArray[i].curr] = 1;        

        //finish allocating the distances while we're here
        //distances[i] = new int[maxCities];
        //pheroConcentration[i] = new double[maxCities];

//    cout << "main@5" << endl;
    }
/*
    cout << "Testing" << endl;
    for (int j = 0; j < maxCities; j++)
        cout << cityArray[j].id << endl;    
*/
    //fill in the distances matrix
//    cout << "main@6" << endl;
    int maxDistance = 0;
//    cout << "main@7" << endl;
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
                distances[m][n] = (int)sqrt(sum);
                //check if highest distance value
                if (distances[m][n] > maxDistance)
                    maxDistance = distances[m][n];
            }
        }
    }

    //set initial "best" as maximum possible tour
    best = maxCities * maxDistance;
} 

//run the simulation maxTime times, which is a function of the number of cities 
//and MAX_TOURS
void AntColony::run()
{
//    std::cout << "run@1" << std::endl;
    maxCities = distances.size();
    maxAnts = maxCities;
    int currTic = 0;
    srand(time(NULL));

//    std::cout << "run@2" << std::endl;
    while(currTic < maxTime)
    {
        currTic++;
//    cout << "run@2maxCities: " << maxCities << endl;

//    std::cout << "run@3" << std::endl;
        if(antSim() == 0)
        {
  //  std::cout << "run@4" << std::endl;
//    cout << "run@3maxCities: " << maxCities << endl;
            updateTrails();
//    cout << "run@4maxCities: " << maxCities << endl;
  //  std::cout << "run@5" << std::endl;
            if(currTic != maxTime)
                resetAnts();
//    cout << "run@5maxCities: " << maxCities << endl;
  //  std::cout << "run@6" << std::endl;

//            std::cout << "currTic: " << currTic << " Best: " << best << std::endl;
        }
    }

//    std::cout << "Best tour: " << best << std::endl;
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
//    std::cout << "antProd@1" << std::endl;
//    std::cout << "maxCities: " << maxCities << endl;
//    cout << "source: " << source << " destination: " << destination << endl;
//    cout << "pheroCon: " << pheroConcentration[source][destination] << endl;
    double x = pow(pheroConcentration[source][destination], ALPHA);
//    std::cout << "antProd@2" << std::endl;
    double y = pow( 1.0/distances[source][destination], BETA);
//    std::cout << "antProd@3" << std::endl;
    return x*y;

}


//function for randomly determining the next city to visit, using probability
//and the equation provided by the antProd(int, int) function above.
int AntColony::nextCity(int ant)
{
//    std::cout << "nextCity@1" << std::endl;
    int destination;
    int source = antArray[ant].curr;
    double denom = 0.0; //sum of heuristic of edges not traveled yet
//    std::cout << "nextCity@2" << std::endl;
    for(destination = 0; destination < maxCities; destination++)
    {
//        cout << "nextCity@2.1 - ant: " << ant << " destination: " << destination << endl;
        //if the city hasn't been visited yet
        if (antArray[ant].citiesVisited[destination] == 0)
        {
  //      cout << "nextCity@2.2" << endl;

                denom += antProd(source, destination); //summing values from edges
    //    cout << "nextCity@2.3" << endl;
        }
    }
    assert( denom != 0.0);
//    std::cout << "nextCity@3" << std::endl;
    
    
    do
    {
  //  std::cout << "nextCity@4" << std::endl;
        double p;
        destination++;
        if (destination >= maxCities)
            destination = 0;
   // std::cout << "nextCity@5" << std::endl;
        if(antArray[ant].citiesVisited[destination] == 0)
        {
            //intensity of pheromone along edge divided by heuristic sum from 
            //edges not yet traversed
            p = antProd(source, destination) / denom;
 //   std::cout << "nextCity@6" << std::endl;
            
            double x = ((double)rand()/RAND_MAX);
            if(x < p)
                break;
        }
    } while(true); //keep doing this until a break is generated
   // std::cout << "nextCity@7" << std::endl;

    return destination;
}

//sumilate the ants moving to the next city and recording the distances
int AntColony::antSim()
{    
//    std::cout << "antSim@1" << std::endl;
    //make moving thread-safe
    std::atomic<int> moving;// 
    moving = 0;

//    std::cout << "antSim@2" << std::endl;
    for(int i = 0; i < maxAnts; i++)
    {
        //verify of more cities left to visit
//    std::cout << "antSim@3" << std::endl;
        if(antArray[i].pathIndex < maxCities)
        {
//    std::cout << "antSim@4 - maxAnts: " << maxAnts << std::endl;
            antArray[i].next = nextCity(i); //get a new next city
//    std::cout << "antSim@5" << std::endl;
            antArray[i].citiesVisited[antArray[i].next] = 1; //label it as visited
//    std::cout << "antSim@6" << std::endl;
            antArray[i].path[antArray[i].pathIndex++] = antArray[i].next; //make it the nex one on the path
//    std::cout << "antSim@7" << std::endl;
            antArray[i].tourLength += distances[antArray[i].curr][antArray[i].next];
//    std::cout << "antSim@8" << std::endl;

            //handle last case: going back to starting point
            if(antArray[i].curr == maxCities)
                antArray[i].tourLength += distances[antArray[i].path[maxCities - 1]]
                    [antArray[i].path[0]];

//    std::cout << "antSim@9" << std::endl;
            antArray[i].curr = antArray[i].next;
//    std::cout << "antSim@10" << std::endl;
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
void AntColony::writeData(string fileName)
{
    fileName = fileName + ".tour";
    //check if file can be opened
    std::ofstream outFile(fileName.c_str());
    std::string s = "Cannot open " + fileName;

    //if the file doesn't exist, throw an exception
    if (!outFile)
    {
        std::cout << s << std::endl;
        return;
    }

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
    ofstream f1;
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

//TODO: change EVERYTHING to vectors to get rid of this
AntColony::~AntColony()
{
    for(int i = 0; i < maxCities; i++)
    {
        delete [] antArray[i].citiesVisited;
        delete [] antArray[i].path;
        //delete [] distances[i];
        //delete [] pheroConcentration[i];
    }
    delete [] antArray;
    delete [] cityArray;
    //delete [] distances;
    //delete [] pheroConcentration;
}

