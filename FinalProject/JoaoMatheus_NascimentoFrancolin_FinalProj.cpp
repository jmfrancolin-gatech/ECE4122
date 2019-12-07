/*
Author: First name: Joao Matheus
        Last Name: Nascimento Francolin
Class: ECE 4122
Date: December 4, 2019
This is an OpenGL applicaiton that utilizes openMP and Glut in order to create
a simulation of flying Torus arond performiing a (random ) choreografy around
a virtual sphere suspeded from the grounds of a football field.
*/

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"
#include "iomanip"
#include <cmath>
#include <math.h>
#include <cstdlib>
#include <GL/glut.h>
#include <chrono>
#include <thread>
#include "JoaoMatheus_NascimentoFrancolin_FinalProj.h"

using namespace std;

void displayFootballField();
void drawUAVs();

// Send location, velocity, and acceleration vector in each direction
const int numElements = 6; // x, y, z, vx, vy, vz, ax, ay, az
const int rcvSize = 16 * numElements; // (Main task + 15 UAVs) * numElements
double* rcvBuffer = new double[rcvSize];
double sendBuffer[numElements];

//----------------------------------------------------------------------
// Reshape callback
//
// Window size has been set/changed to w by h pixels. Set the camera
// perspective to 45 degree vertical field of view, a window aspect
// ratio of w/h, a near clipping plane at depth 1, and a far clipping
// plane at depth 100. The viewport is the entire window.
//
//----------------------------------------------------------------------
void changeSize(int w, int h)
{
    float ratio = ((float)w) / ((float)h); // window aspect ratio
    glMatrixMode(GL_PROJECTION); // projection matrix is active
    glLoadIdentity(); // reset the projection
    gluPerspective(60.0, ratio, 0.1, 1000.0); // perspective transformation
    glMatrixMode(GL_MODELVIEW); // return to modelview mode
    glViewport(0, 0, w, h); // set viewport (drawing area) to entire window
}
//----------------------------------------------------------------------
// Draw the entire scene
//
// We first update the camera location based on its distance from the
// origin and its direction.
//----------------------------------------------------------------------
void renderScene()
{
    // Clear color and depth buffers
    glClearColor(0.0, 1.0, 0.0, 1.0); // background color to green??
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Reset transformations
    glLoadIdentity();

    // Eye location and direction
    float eye_x = 0.0;
    float eye_y = -90.0;
    float eye_z = 75.0;
    float center_x = 0.0;
    float center_y = 0.0;
    float center_z = 25.0;

    gluLookAt(eye_x, eye_y, eye_z,center_x, center_y, center_z, 0.0, 0.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
    displayFootballField();
    drawUAVs();
    glutSwapBuffers(); // Make it all visible
}
//----------------------------------------------------------------------
// timerFunction  - called whenever the timer fires
//----------------------------------------------------------------------
void timerFunction(int id)
{
    glutPostRedisplay();
    glutTimerFunc(100, timerFunction, 0);
    MPI_Allgather(sendBuffer, numElements, MPI_DOUBLE,
    rcvBuffer, numElements, MPI_DOUBLE,
    MPI_COMM_WORLD);
}
//----------------------------------------------------------------------
// mainOpenGL  - standard GLUT initializations and callbacks
//----------------------------------------------------------------------
void mainOpenGL(int argc, char**argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(400, 400);

    glutCreateWindow(argv[0]);
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_NORMALIZE);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    glutReshapeFunc(changeSize);
    glutDisplayFunc(renderScene);
    glutTimerFunc(100, timerFunction, 0);
    glutMainLoop();
}
//----------------------------------------------------------------------
// initializeAUVs  - Initialize AUV initail position
//----------------------------------------------------------------------
void initializeAUVs()
{
   // Loop through all AUVs
    for (int i = 0; i < 15; i++)
    {
        rcvBuffer[(i + 1)*numElements] = 25.0 * (i % 5) - 50.0;
        rcvBuffer[(i + 1)*numElements + 1] = -25.0 * floor(i / 5) + 25.0;
        rcvBuffer[(i + 1)*numElements + 2] = 0.0;
    }
}
//----------------------------------------------------------------------
// displayFootballField  - cretates a footbal fild from a bitmap image
//----------------------------------------------------------------------
void displayFootballField()
{
    glEnable(GL_TEXTURE_2D);
    glClearColor(0.5, 0.5, 0.0, 0.0);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    // Instanciate textures
    BMP inBitmap;
    GLuint texture[1];
    inBitmap.read("JoaoMatheus_NascimentoFrancolin_FinalProj.bmp");

    // Create Textures
    glPixelStorei(GL_UNPACK_ALIGNMENT,1);
    glGenTextures(1, texture);

    // Setup first texture
    glBindTexture(GL_TEXTURE_2D, texture[0]);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, 3, inBitmap.bmp_info_header.width, 
    inBitmap.bmp_info_header.height, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, &inBitmap.data[0]);

    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
    glEnable(GL_TEXTURE_2D);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glPushMatrix();
    glBindTexture(GL_TEXTURE_2D, texture[0]);

    // Draw football field 
    glBegin(GL_QUADS);
    glTexCoord2f(1, 1);
    glVertex3f(60.0f, 30.0f, 0.0f);
    glTexCoord2f(1, 0);
    glVertex3f(60.0f, -30.0f, 0.0f);
    glTexCoord2f(0, 0);
    glVertex3f(-60.0f, -30.0f, 0.0f);
    glTexCoord2f(0, 1);
    glVertex3f(-60.0f, 30.0f, 0.0f);
    glEnd();

    glPopMatrix();
    glutSwapBuffers();
}
//----------------------------------------------------------------------
// drawUAVs  - Cretates UAVs and a sphere for reference
//----------------------------------------------------------------------
void drawUAVs()
{
    glDisable(GL_TEXTURE_2D);

    for (int i = 0; i < 15; i++)
    {
        glColor3f(0, 0, 255);
        // Get xyz location
        float xPosition = rcvBuffer[(i+1)*numElements];
        float yPosition = rcvBuffer[(i+1)*numElements + 1];
        float zPosition = rcvBuffer[(i+1)*numElements + 2];

        // Draw a yellow teapot at specified location
        glPushMatrix();
        glTranslatef(xPosition, yPosition, zPosition);
        glRotatef(90.0, 1.0, 0.0, 0.0);
        glScalef(1.0, 1.0, 1.0);
        glutSolidTorus(0.3, 0.5, 10.0, 20.0);
        glPopMatrix();
        glutSwapBuffers();
    }

    //glEnable(GL_BLEND);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glColor4f(1.5, 0, 0, 255);
    // Draw virtual sphere
    glColor4f(1.0, 0.0, 0.0, 0.0);
    glPushMatrix();
    glTranslatef(0.0, 0.0, 50.0);
    glutSolidSphere(10.0, 100, 100);
    glPopMatrix();
    glutSwapBuffers();
}

//----------------------------------------------------------------------
// calcualteUAVsLocation - Uses openGL to comoute the configuration
//                         of the UAVs (position, velocity, and accel  
//----------------------------------------------------------------------
void calcualteUAVsLocation(int rank, bool sphereFlag)
{
    // Local variables
    double spring[3], dirCorr[3], force[3], drag[3], accNext[3];
    double gravity[3] = { 0.0, 0.0, -9.8 };
    double speedUAV = 8.0;

    // Get UAV position
    double xx = rcvBuffer[rank*numElements];
    double yy = rcvBuffer[rank*numElements + 1];
    double zz = rcvBuffer[rank*numElements + 2];
    
    // Get UAV velocities
    double v_x = rcvBuffer[rank*numElements + 3];
    double v_y = rcvBuffer[rank*numElements + 4];
    double v_z = rcvBuffer[rank*numElements + 5];

    // Compute UAV distance from sphere | sphere = (0, 0, 50)
    double distSphereX = rcvBuffer[rank * numElements];
    double distSphereY = rcvBuffer[rank * numElements + 1];
    double distSphereZ = rcvBuffer[rank * numElements + 2] - 50.0;
    double magDistSphere = sqrt(pow(distSphereX, 2) + pow(distSphereY, 2) + pow(distSphereZ, 2));
               

    // Check for collisions
    for (int i=numElements; i<rcvSize; i+=numElements)
    {
        // Compute distance to all other UAVs
        double distUavX = rcvBuffer[i] - rcvBuffer[rank * numElements];
        double distUavY = rcvBuffer[i + 1] - rcvBuffer[rank * numElements + 1];
        double distUavZ = rcvBuffer[i + 2] - rcvBuffer[rank * numElements + 2];
        double distanceUAV = sqrt(pow(distUavX, 2) + pow(distUavY, 2) + pow(distUavZ, 2));

        // Threshold condition
        if (distanceUAV <= 1.1)
        {
            rcvBuffer[rank * numElements] = rcvBuffer[i];
            rcvBuffer[rank * numElements + 1] = rcvBuffer[i + 1];
            rcvBuffer[rank * numElements + 2] = rcvBuffer[i + 2];
        }
    }
    
    // Check if UAV has reach the shell of the sphere
    if (sphereFlag)
    {
        // Compute closest point tothe shell
        double sphereX = distSphereX / magDistSphere * 10;
        double sphereY = distSphereY / magDistSphere * 10;
        double sphereZ = distSphereZ / magDistSphere * 10 + 50.0;

        // Hooks Law | F = k * s
        spring[0] = 80 * (sphereX - xx);
        spring[1] = 80 * (sphereY - yy);
        spring[2] = 80 * (sphereZ - zz);

        // Generate random number
        double randNum = (double)rand() / RAND_MAX;

        // Apply correction oposed to velocity vector
        dirCorr[0] = (v_x > 0.0) ? randNum : (-1) * randNum;
        dirCorr[1] = (v_y > 0.0) ? randNum : (-1) * randNum;
        dirCorr[2] = (v_z > 0.0) ? randNum : (-1) * randNum;
    }

    // Initial behavior | move towards the center of the sphere at 2 m/s
    else
    {
        // UAV initial speed
        speedUAV = 2.0;

        // Desired velocity
        double v = 2.0;

        // Compute forceelaration for t = 0.1 | v = v_0 + a * t | v = 2.0
        double a_x = pow((2.0 - v_x) / 0.1, 2);
        double a_y = pow((2.0 - v_y) / 0.1, 2);
        double a_z = pow((2.0 - v_z) / 0.1, 2);

        // Normalization vector
        double normVector = sqrt(a_x + a_y + a_z);   
             
        // Force vector
        force[0] = -distSphereX / magDistSphere * normVector;
        force[1] = -distSphereY / magDistSphere * normVector;
        force[2] = -distSphereZ / magDistSphere * normVector;
    }

    // Apply drag in case of excess speed
    if (abs(v_x) > speedUAV) force[0] = 0.0;
    if (abs(v_y) > speedUAV) force[1] = 0.0;
    if (abs(v_z) > speedUAV) force[2] = 0.0;

    drag[0] = (abs(v_x) > speedUAV) ? (speedUAV - v_x) / 0.1 : 0;
    drag[1] = (abs(v_y) > speedUAV) ? (speedUAV - v_y) / 0.1 : 0;
    drag[2] = (abs(v_z) > speedUAV) ? (speedUAV - v_z) / 0.1 : 0;

    // Compute acceleration
    accNext[0] = force[0] - gravity[0] + drag[0] + spring[0] + dirCorr[0];
    accNext[1] = force[1] - gravity[1] + drag[1] + spring[1] + dirCorr[1];
    accNext[2] = force[2] - gravity[2] + drag[2] + spring[2] + dirCorr[2];
    
    // Apply constrains
    accNext[0] = max(min(accNext[0], 20.0),-20.0);
    accNext[1] = max(min(accNext[1], 20.0),-20.0);
    accNext[2] = max(min(accNext[2], 20.0),-20.0);

    // Compute speed | v = v_0 + a * t
    sendBuffer[3] = v_x + accNext[0] * 0.1;
    sendBuffer[4] = v_y + accNext[1] * 0.1;
    sendBuffer[5] = v_z + accNext[2] * 0.1;
    // Compute position | x = x_0 + v_0 * t + a * t^2 / 2
    sendBuffer[0] = xx + v_x / 10.0 + accNext[0] / 200.0;
    sendBuffer[1] = yy + v_y / 10.0 + accNext[1] / 200.0;
    sendBuffer[2] = zz + v_z / 10.0 + accNext[2] / 200.0;
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
// Main entry point determines rank of the process and follows the
// correct program path
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
int main(int argc, char**argv)

{
    int numTasks, rank;
    int rc = MPI_Init(&argc, &argv);
    bool sphereFlag = false;

    if (rc != MPI_SUCCESS)
    {
        printf("Error starting MPI program. Terminating.\n");
        MPI_Abort(MPI_COMM_WORLD, rc);
    }

    MPI_Comm_size(MPI_COMM_WORLD, &numTasks);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    
    int gsize = 0;
    MPI_Comm_size(MPI_COMM_WORLD, &gsize);
    
    srand (rank);
    initializeAUVs();
 

    if (rank == 0)
    {
        mainOpenGL(argc, argv);
    }
    else
    { 
        // Sleep for 5 seconds
        std::this_thread::sleep_for(std::chrono::seconds(5));
        for (int ii = 0; ii < 600 ; ii++)
        {
            if (sphereFlag == false)
            {
                //  Check if the UAV has reached the sphere 
                double x = rcvBuffer[rank*numElements];
                double y = rcvBuffer[rank*numElements + 1];
                double z = rcvBuffer[rank*numElements + 2] - 50.0;
                double dist = sqrt( pow(x, 2) + pow(y, 2) + pow(z, 2) );
                sphereFlag = dist < 10.0;
            }
  
            calcualteUAVsLocation(rank, sphereFlag);
            MPI_Allgather(sendBuffer, numElements, MPI_DOUBLE, rcvBuffer, numElements, MPI_DOUBLE, MPI_COMM_WORLD);
        }
    }
    return 0;
}
