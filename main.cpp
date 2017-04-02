#include <iostream>
#include <vector>
#include <fstream>
#include <cstring>
#include <chrono>

#include "AntColony.h"

void getCities(std::string, std::vector< std::vector<int> >*);

int main(int argc, char* argv[])
{

    //check for correct number of args - the input file name and if an option 
    //is present
    if (argc < 2 || argc > 3)
    {
        std::cerr << "Usage: " << argv[0] << " [FILE]... [OPTION]...\n" 
            << "Options:\n"
            << "\t-time\t\tOutput the running time of the program." 
            << std::endl;
        return 1;
    } 

    //check if the program should track the running time
    bool timed = false;
    if (argc == 3)
        if( std::strcmp(argv[2], "-time") == 0 )
            timed = true;
    
    bool python = false;

    //if tracking the running time, get the start time. Using std::chrono
    //because clock_t will give bad results with multithreading
    std::chrono::high_resolution_clock::time_point start = 
        std::chrono::high_resolution_clock::now();

    //save the name of the input file into a string
    std::string fileName = std::string(argv[1]);
    //create a vector of vectors using the cmd line arg file name
    std::vector< std::vector<int> > inputData;
    getCities(fileName, &inputData);
    
    //instantiate the antcolony class and run the simulation
    AntColony AC(&inputData);
    AC.run();

    //if tracking the running time, get the ending time and output the result
    if(timed)
    {
        std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(end - start);
        std::cout << "Time taken: " << time_span.count() << " seconds" << std::endl;
    }

    AC.writeData(fileName);
    
    //TODO
    if(python)
        AC.writeDataForPython();
        


}//end main


/*
 * Input: a string that is the name of a file
 * Output: a vector or int vectors
 *
 * Creates a vector of vectors of the cities and data. If the input file is in
 * the format "[city] [x-coordinate] [y-coordinate]", with each being an int,
 * the result will be a vector where each vector within it contains those 3
 * elements, in order.
 */
void getCities(std::string fileName, std::vector< std::vector<int> >* inputData)
{

    //check if file can be opened
    std::ifstream inFile(fileName.c_str());
    std::string s = "Cannot open " + fileName;

    //if the file doesn't exist, throw an exception
    if (!inFile)
    {
        std::cout << s << std::endl;
        throw s;
    }

    //input each line in the file
    int city, x, y;
    while(inFile >> city >> x >> y)
    {
        std::vector<int> readIn; //temporary vector
        readIn.push_back(city);
        readIn.push_back(x);
        readIn.push_back(y);

        //add the temporary vector to the main one
        inputData->push_back(readIn);
    }
    inFile.close();
}
