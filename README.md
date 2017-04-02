#AntColonySalesman

An implementation of a heuristic to solve the Traveling Salesman Problem using "ant colony optimization".

Code adapted from the _A Positronic Brain_ blog entry "The Traveling Salesman Problem - Using Ant Algorithms"
https://ameyajoshi005.wordpress.com/2012/11/10/the-traveling-salesman-problem-using-ant-algorithms/

The idea is that various AI "ants" travel the paths between cities, leaving a pheromone trail. At the end, the tour with the strongest perhomone concentration is indirectly accepted as the solution to the problem.

This version uses the C++ 11 threads library so that each "ant" its own thread during execution of the tours.
