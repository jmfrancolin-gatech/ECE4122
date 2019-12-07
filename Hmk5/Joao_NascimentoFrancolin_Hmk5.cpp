/*
Author: Joao Matheus Nascimento Francolin
Class: ECE 4122
Date: November 13, 2019
Description: This is a MPI program that guides yellowJackets back to their main ship.
YellowJackets are flying machines, and their configuration is defined on the struct
yellowJacket. There are 7 yellowJackets and 1 main ship. Each rank of the MPI porgram is
responssable for updating their respective configuration. At each timestep the yellowJackets
uses therie thrusters to apply forces and change their velocity, position and accelation,
untiil finaly reaching the ship. The masters rank (zero) performs I/O as weel.
*/

#include <iostream>
#include <stdio.h>
#include <fstream>
#include <stddef.h>
#include <iomanip>
#include <sstream>
#include <math.h>

#include "mpi.h"
//#include <stdlib.h>
//#include <vector>
//#include <limits>
//#include <cmath>
//#include <cstdlib>

using namespace std;

// Function declarions
void LoadInputFile();
void CalculateBuzzyConfig();
void CalculateYellowJacketConfig();
void OutputStatusAndLocation();
double getRandomNumber(double a, double b);

// Define yellowJacket
typedef struct {
    int rankId;
    int status;
    double pos[3];
    double vel[3];
    double acc[3];
} yellowJacket;

// Global variables
int nTimeSteps;
double maxThrust;
yellowJacket swarm[8];

int main(int argc, char**argv)
{
    
    int  numtasks, rank, rc;

    rc = MPI_Init(&argc, &argv);

    if (rc != MPI_SUCCESS)
    {
        printf("Error starting MPI program. Terminating.\n");
        MPI_Abort(MPI_COMM_WORLD, rc);
    }
    
    // open file
    string line;
    ifstream inputFile ("in.dat");
    
    // line 1
    getline(inputFile, line);
    nTimeSteps = stoi(line);
    
    // line 2
    getline(inputFile, line);
    maxThrust = stof(line);
    
    // close file
    inputFile.close();

    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    
    // Define a yellowJacket MPI struct
    int numItems = 5;
    int length[5] = {1, 1, 3, 3, 3};
    MPI_Datatype type[5] = {MPI_INT, MPI_INT, MPI_DOUBLE, MPI_DOUBLE, MPI_DOUBLE};
    MPI_Datatype MPI_YJ_TYPE;
    MPI_Aint offset[5];
    offset[0] = offsetof(yellowJacket, rankId);
    offset[1] = offsetof(yellowJacket, status);
    offset[2] = offsetof(yellowJacket, pos);
    offset[3] = offsetof(yellowJacket, vel);
    offset[4] = offsetof(yellowJacket, acc);
    MPI_Type_struct(numItems, length, offset, type, &MPI_YJ_TYPE);
    MPI_Type_commit(&MPI_YJ_TYPE);
    
    // status
    MPI_Status status;

    // Seed the random number generator to get different results each time
    srand(rank);
    if (rank == 0)
    {
        // Load in.dat inputFile
        LoadInputFile();
    }
    
    // Broadcast to yellowjackets
    MPI_Bcast(&swarm, 8, MPI_YJ_TYPE, 0, MPI_COMM_WORLD);

    // Loop through the number of time steps
    for (int round = 0; round < nTimeSteps; round++)
    {
        if (rank == 0)
        {
            // Calculate Buzzy new location
            CalculateBuzzyConfig();
            // Gather all yellowJackets
            MPI_Allgather(MPI_IN_PLACE, 1, MPI_YJ_TYPE, &swarm, 1, MPI_YJ_TYPE, MPI_COMM_WORLD);
            // Output yellowJackets configuration
            OutputStatusAndLocation();
        }
        else
        {
            // Calculate yellow jacket new location
            CalculateYellowJacketConfig();
            // Gather all yellowJackets
            MPI_Allgather(MPI_IN_PLACE, 1, MPI_YJ_TYPE, &swarm, 1, MPI_YJ_TYPE, MPI_COMM_WORLD);
        }
    }

    MPI_Finalize();
}

void LoadInputFile()
{
    // open file
    string line;
    ifstream inputFile ("in.dat");
    
    // skip first 2 lines
    getline(inputFile, line);
    getline(inputFile, line);
   
    // parse inputeFile and populate yellowJacket struct
    double values[7];
    for (int i = 0; i < 8; i++)
    {        
        getline(inputFile, line);
        stringstream iss(line);
        for (int j = 0; j < 7; j++)
        {
            iss >> values[j];
        }
        // rankId and status
        swarm[i].rankId = i;
        swarm[i].status = (int) 1;
        // postion
        swarm[i].pos[0] = values[0];
        swarm[i].pos[1] = values[1];
        swarm[i].pos[2] = values[2];
        // velocity
        swarm[i].vel[0] = values[3] * values[6];
        swarm[i].vel[1] = values[4] * values[6];
        swarm[i].vel[2] = values[5] * values[6];
        // acceleration
        swarm[i].acc[0] = 0;
        swarm[i].acc[0] = 0;
        swarm[i].acc[0] = 0;
    }
    // close file
    inputFile.close();
}

// Update Buzz configuration
void CalculateBuzzyConfig()
{
    // Update postion of Buzz (const v, delta_t = 1)
    swarm[0].pos[0] += swarm[0].vel[0];
    swarm[0].pos[1] += swarm[0].vel[1];
    swarm[0].pos[2] += swarm[0].vel[2];
}

// Update yellowJacket position
void CalculateYellowJacketConfig()
{
    // local variable to compute docking condition
    double dockingDistance;
    
    for (int i = 1; i < 8; i++)
    {
        // Update acceleration
        // x = x_0 + v_x0 * t + 0.5 * a_x * t^2
        // Solve for a_x when t = nTimeSteps
        // multiply by a factor n to account for thrusters slowdown
        swarm[i].acc[0] = 2 * (2.0 / pow(nTimeSteps, 2)) * (swarm[0].pos[0] - swarm[i].pos[0] - swarm[i].vel[0]);
        swarm[i].acc[1] = 2 * (2.0 / pow(nTimeSteps, 2)) * (swarm[0].pos[1] - swarm[i].pos[1] - swarm[i].vel[1]);
        swarm[i].acc[2] = 2 * (2.0 / pow(nTimeSteps, 2)) * (swarm[0].pos[2] - swarm[i].pos[2] - swarm[i].vel[2]);
        // account for limitationsof the thrusters
        swarm[i].acc[0] *= min(getRandomNumber(0.8, 1.2), maxThrust);
        swarm[i].acc[1] *= min(getRandomNumber(0.8, 1.2), maxThrust);
        swarm[i].acc[2] *= min(getRandomNumber(0.8, 1.2), maxThrust);
       
        // Update position
        // x = x_0 + v_x0 * t + 0.5 * a_x * t^2
        // Solve for x when t = 1 TimeStep
        swarm[i].pos[0] += swarm[i].vel[0] + 0.5 * swarm[i].acc[0];
        swarm[i].pos[1] += swarm[i].vel[1] + 0.5 * swarm[i].acc[1];
        swarm[i].pos[2] += swarm[i].vel[2] + 0.5 * swarm[i].acc[2];
         
        // Update velocity
        // v_x = v_x0 + a_x * t
        // Solve for v_x when t = 1 TimeStep
        swarm[i].vel[0] += swarm[i].acc[0];
        swarm[i].vel[1] += swarm[i].acc[1];
        swarm[i].vel[2] += swarm[i].acc[2];
        
        // Check for docking condition
        dockingDistance = sqrt(pow(swarm[i].pos[0], 2) + pow(swarm[i].pos[1], 2) + pow(swarm[i].pos[2], 2));
        if (dockingDistance < 50)
        {
            swarm[i].status = 2;
        }
    }
}

// Handles I/O
// Format: rankId, status, x, y, z, Fx, Fy, Fz
void OutputStatusAndLocation()
{
    for (int i = 0; i < 8; i++)
    {
        // Rank and status
        cout << swarm[i].rankId << ", ";
        cout << dec << swarm[i].status;

        cout << scientific << setprecision(3);
        // Position
        cout << ", " << swarm[i].pos[0];
        cout << ", " << swarm[i].pos[1];
        cout << ", " << swarm[i].pos[2];
        // Force (F = m*a)
        cout << ", " << swarm[i].acc[0] * 10000;
        cout << ", " << swarm[i].acc[1] * 10000;
        cout << ", " << swarm[i].acc[2] * 10000;

        cout << endl;
    }
    cout << endl;
}

// Generates a random number between a and b
double getRandomNumber(double a, double b)
{
    double random = ((double) rand()) / (double) RAND_MAX;
    double diff = b - a;
    double r = random * diff;
    return a + r;
}
