#include <iostream>
#include <vector>
#include <fstream>
#include <ctime>
#include <cstring>
#include <cstdio>

#include "AntColony.h"

std::vector< std::vector<int> > getCities(std::string);

int main(int argc, char* argv[])
{

    //check is the program should track the running time
    bool timed = true;
    bool python = false;

    //if tracking the running time, get the start time
    clock_t begin, end;
    if(timed)
        begin = clock();

    //save the name of the input file into a string
    std::string fileName = std::string(argv[1]);
    //create a vector of vectors using the cmd line arg file name
    std::vector< std::vector<int> > inputData = getCities(fileName);
    printf("main@1");
    AntColony AC(&inputData);

    srand(time(NULL));

//    std::cout << "Testing antProd()" << std::endl;
//    printf("%e ", AC.antProd(0,0));
    /*    for (int i = 0; i < inputData.size() ; i++)
    {
        int x = i;
        int y = inputData.size() - i;
        printf("%e ", AC.antProd(x,y));
//        std::cout << AC.antProd(x, y) << std::endl;

    }
*/

    printf("Testing next city\n");
    int aamax = AC.antArray.size();
    for(int i = 0; i < aamax; i++)
        AC.nextCity(i);



    //if tracking the running time, get the ending time and output the result
    if(timed)
    {
        end = clock();
        double seconds = double(end - begin) / CLOCKS_PER_SEC;
        std::cout << "Time taken: " << seconds << " seconds." << std::endl;
    }

/*
    AC.writeData(fileName);
    if(python)
        AC.writeDataForPython();
*/        


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
        printf("%i ",city);
        readIn.push_back(city);
        readIn.push_back(x);
        readIn.push_back(y);

        //add the temporary vector to the main one
        inputData.push_back(readIn);
    }
    printf("f@end\n");
    inFile.close();
    return inputData;
}

