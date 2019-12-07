/*
Author: Joao Matheus Francolin
Class: ECE 4122
Last date modified: October 1, 2019
Description: Implements a modified version of a BSF algorithm. It computes all
optimum paths to transverse a grid from the top left  for the botton left.
*/

// Implemantation only works in C++ 11
// scp -r ECE4122/ jfrancolin3@coc-ice.pace.gatech.edu:~
// g++ Problem1.cpp -fopenmp -O3 -o Problem1

#include <iostream>
#include <vector>
#include <queue>
#include <utility>
#include <algorithm>
#include <cstdio>
#include <ctime>
#include <omp.h>

int modified_bfs(int vertices_dim1, int vertices_dim2);
std::vector<std::pair <int, int> > get_successors(int vertices_dim1, int vertices_dim2, std::pair <int, int> curr_node);
void print_path(std::vector<std::pair <int, int> > path);
bool already_in_path(std::vector<std::pair <int, int> > path, std::pair <int, int> next_node);


int modified_bfs(int vertices_dim1, int vertices_dim2)
{
    // node is defined as a pair of coordinates
    std::pair <int, int> curr_node;
    std::pair <int, int> next_node;
    std::pair <int, int> goal;

    // path is defined as a vector of nodes
    std::vector<std::pair <int, int> > curr_path;
    std::vector<std::pair <int, int> > next_path;
    std::vector<std::pair <int, int> > successors;

    // queue keeps record of all paths travelled
    std::queue <std::vector<std::pair <int, int> > > queue;

    // initialize search state
    curr_node = std::make_pair (0, 0);
    curr_path.push_back(curr_node);
    queue.push(curr_path);
    goal = std::make_pair(vertices_dim1 - 1, vertices_dim2 - 1);
    unsigned int num_paths = 0;

    // loop while the queue is not empty
    while (not queue.empty())
    {
        // pop front of the queue
        curr_path = queue.front();
        queue.pop();

        // the curr_node is the last node from the path that was popped
        curr_node = curr_path[curr_path.size() - 1];

        // check if we arrived at the goal
        if (curr_node == goal)
        {
            // incremment path counter
            num_paths++;
            // print path for debugging purposes
            // print_path(curr_path);
            continue;
        }

        // get possible successors of the node
        successors = get_successors(vertices_dim1, vertices_dim2, curr_node);

        #pragma omp parallel num_threads ( 1 )
        // #pragma omp parallel
        {
            // #pragma omp for
            // loop through the list of successors
            for (int ii = 0; ii < 4; ii++)
            {
                if (ii < successors.size())
                {
                    // next_node iterates through the list of successors
                    next_node = std::make_pair(successors[ii].first, successors[ii].second);

                    // check if successor was already visited
                    if (not already_in_path(curr_path, next_node))
                    {
                        // create a new path with the unvisited successor appended at the end
                        next_path = curr_path;
                        next_path.push_back(next_node);
                        // push this new path to the queue
                        queue.push(next_path);
                    }
                }
            }
        }
    }
    // return the number of paths found
    return num_paths;
}

bool already_in_path(std::vector<std::pair <int, int> > path, std::pair <int, int> next_node)
{
    // iterate through path to check for instances of next_node
    if (std::find(path.begin(), path.end(), next_node)!= path.end())
    {
        return true;
    }
    return false;
}

void print_path(std::vector<std::pair <int, int> > path)
{
    for (int ii = 0; ii < path.size(); ii++)
    {

        if (ii == path.size() - 1)
        {
            std::cout << "(" << path[ii].first << ", " << path[ii].second << ")" << std::endl;
        }
        else
        {
            std::cout << "(" << path[ii].first << ", " << path[ii].second << ") -> ";
        }

    }
}

std::vector<std::pair <int, int> > get_successors(int vertices_dim1, int vertices_dim2, std::pair <int, int> curr_node)
{
    // local variables
    std::vector<std::pair <int, int> > successors;
    std::pair <int, int> next_node;

    // In the implemantion, we are only allowed to move South or East
    // Logic for moving towards North or West has been commented out

    // South
    if (curr_node.first + 1 < vertices_dim1)
    {
        next_node = std::make_pair(curr_node.first + 1, curr_node.second);
        successors.push_back(next_node);
    }
    // East
    if (curr_node.second + 1 < vertices_dim2)
    {
        next_node = std::make_pair(curr_node.first, curr_node.second + 1);
        successors.push_back(next_node);
    }
    // // North
    // if (curr_node.first - 1 >= 0)
    // {
    //     next_node = std::make_pair(curr_node.first - 1, curr_node.second);
    //     successors.push_back(next_node);
    // }
    // // West
    // if (curr_node.second - 1 >= 0)
    // {
    //     next_node = std::make_pair(curr_node.first, curr_node.second - 1);
    //     successors.push_back(next_node);
    // }
    return successors;
}


int main(int argc, char *argv[])
{
    std::clock_t start;
    double duration;

    start = std::clock();

    // transfrom grid into graph
    // A 2 x 2 grid can be represented as a graph with 9 vertices and 10 edges
    int vertices_dim1 = atoi(argv[2]) + 1;
    int vertices_dim2 = atoi(argv[4]) + 1;

    // We are interested in the permutation of the edges
    int num_paths = modified_bfs(vertices_dim1, vertices_dim2);

    duration = ( std::clock() - start ) / (double) CLOCKS_PER_SEC;
    std::cout<<"Time elapsed: "<< duration <<" seconds\n";

    // program output
    std::cout << "Number of Routes: " << num_paths << std::endl;
    return 0;
}
