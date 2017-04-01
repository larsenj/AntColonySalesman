#include <iostream>
#include <vector>
#include <fstream>
#include <ctime>
#include <cstring>
#include <chrono>

#include "AntColony.h"

std::vector< std::vector<int> > getCities(std::string);

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

    //check is the program should track the running time
    bool timed = false;
    if (argc == 3)
        if( std::strcmp(argv[2], "-time") == 0 )
            timed = true;
    
    bool python = false;

    //if tracking the running time, get the start time
    std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();
    //clock_t begin, end;
    //if(timed)
        //begin = clock();

    //save the name of the input file into a string
    std::string fileName = std::string(argv[1]);
    //create a vector of vectors using the cmd line arg file name
    std::vector< std::vector<int> > inputData = getCities(fileName);
    AntColony AC(&inputData);
    AC.run();

    //if tracking the running time, get the ending time and output the result
    if(timed)
    {
        std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(end - start);
        std::cout << "Time taken: " << time_span.count() << " seconds" << std::endl;
        //end = clock();
        //double seconds = double(end - begin) / CLOCKS_PER_SEC;
        //std::cout << "Time taken: " << seconds << " seconds." << std::endl;
    }
    AC.writeData(fileName);
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
std::vector< std::vector<int> > getCities(std::string fileName)
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

    std::vector< std::vector<int> > inputData;  //main vector

    //input each line in the file
    int city, x, y;
    while(inFile >> city >> x >> y)
    {
        std::vector<int> readIn; //temporary vector
        readIn.push_back(city);
        readIn.push_back(x);
        readIn.push_back(y);

        //add the temporary vector to the main one
        inputData.push_back(readIn);
    }
    inFile.close();
    return inputData;
}

