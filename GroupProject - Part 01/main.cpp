/*  Title: Computer Graphics Group Project - Part 01
 *  Name: Ryan Bracewell, Brett Smith, Mason Wolfrum
 *  Date: 5/01/2022
 */
#include<windows.h>
#ifdef __APPLE_CC__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <math.h>
#include <stdlib.h>
#include <bits/stdc++.h>
#include "RGBf.h"
#include "Vector3.h"
#include <vector>
#include <iostream>
using namespace std;

GLuint regPoly, regPoints;
static GLfloat rotTheta = 0.0;
int G_VertCount = 0;
std::vector<Vector3> vContainer; //Dynamic array that holds all vertex coordinates.
Vector3 centroid = Vector3(0, 0, 0.0), MousePos = Vector3(0, 0, 0.0);
bool vertsExist = false, polyExists = false, RotClock = false, activeShift = false, polyGrabbed = false;
RGBf bgk = RGBf(44,47,51); //Define background color in RGB.
RGBf geoColor = RGBf(86,99,247); //Define geometry color in RGB

//Draws the vertex with data sent from mouse function.
void drawVertex(int x, int y)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //Sets glClear to use Color Buffer and Depth Buffer (for polygon detection).
    G_VertCount++;
    Vector3 Point = Vector3(x, glutGet(GLUT_WINDOW_HEIGHT) - y, 1.0);
    vContainer.push_back(Point); //Pushes new vertex into the vertex array.

    regPoints = glGenLists(1); //Generates list of empty vertex display-list. Need to use a display list and call in main draw function in order to persist across window resize.
    glNewList(regPoints, GL_COMPILE); //Specifies the display-list name.
    glColor3f(geoColor.red, geoColor.green, geoColor.blue);
    glBegin( GL_POINTS );
        for (int i = 0; i < G_VertCount; i++){  //Must redraw every point due to use of double buffer.
            glVertex2i (vContainer[i].x, vContainer[i].y);
        }
    glEnd();
    glEndList();
    glutSwapBuffers();
    glFlush();
    glutPostRedisplay();
}

//Initializes Polygon from list of user-defined vertices.
void initPolygon (void)
{
    regPoly = glGenLists(1); //Generates list of empty polygon display-list.
    glNewList(regPoly, GL_COMPILE); //Specifies the display-list name.
    glColor3f(geoColor.red, geoColor.green, geoColor.blue);
    glBegin(GL_POLYGON);
        for (int i = 0; i < G_VertCount; i++){ //Loops through list of user-defined vertices and draws them as a polygon.
            glVertex2i (vContainer[i].x, vContainer[i].y);
        }
    glEnd();
    glEndList();
    polyExists = true;
}

//Draws Polygon every frame.
void drawPolygon (void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glPushMatrix();  //Pushes current matrix.
    glTranslatef(centroid.x,centroid.y,0.0); //Translates coordinates to center of polygon, rotates, and translates back.
    glRotatef (rotTheta, 0.0, 0.0, 1.0);
    glTranslatef(-centroid.x,-centroid.y,0.0);
    glCallList (regPoly); //Executes display list.
    if(!polyExists){
        glCallList(regPoints); // Displays vertices if and only if a full polygon doesn't exist.
    }
    glPopMatrix(); //Pops matrix from stack.
    glutSwapBuffers();
    glFlush();
}

//Calculates center of Polygon.
void cenCalc()
{
    Vector3 lMost = vContainer[0]; Vector3 rMost = vContainer[0];
    Vector3 vLowest = vContainer[0]; Vector3 vHighest = vContainer[0];

    //Determines rough bounding box of object.
    for (int i = 0; i < G_VertCount; i++){
        if(lMost.x > vContainer[i].x){
            lMost = vContainer[i];
        }
        if(rMost.x < vContainer[i].x){
            rMost = vContainer[i];
        }
        if(vLowest.y > vContainer[i].y){
            vLowest = vContainer[i];
        }
        if(vHighest.y < vContainer[i].y){
            vHighest = vContainer[i];
        }
    }
    centroid = Vector3((lMost.x + rMost.x)/2, (vHighest.y + vLowest.y)/2, 1.0 ); //Sets centroid to center of generated bounding box.
}

//Rotation Function.
void rotatePolygon (void)
{
    switch(RotClock){ //Switches between Clockwise and Counterclockwise rotation based on global RotClock variable.
        case true:  {
            rotTheta -= 1.0; //Clockwise
            break;
        }
        default:    {
            rotTheta += 1.0; //CounterClockwise
            break;
        }
    }
    if (rotTheta > 360.0){ //If rotation goes over 360 degrees, use complimentary angle to maintain rotational direction.
       rotTheta -= 360.0;
    }

    glutPostRedisplay(); //Calls Screen redisplay.
}

//Uniformly scales polygon according to a scale factor. Wraps scaling in centroid-relative translation to maintain position.
void scalePolygon(float sFactor)
{
    glTranslatef(centroid.x,centroid.y,0.0);
    glScalef (sFactor, sFactor, 0.0);
    glTranslatef(-centroid.x,-centroid.y,0.0);
    glutPostRedisplay();
}

//Moves polygon to wherever the user drags their mouse.
void movePolygon(int x, int y)
{
    glTranslatef((x - MousePos.x),(glutGet(GLUT_WINDOW_HEIGHT) - y) - MousePos.y,0.0); //Moves polygon by factor of however far the mouse has traveled.
    MousePos = Vector3(x, glutGet(GLUT_WINDOW_HEIGHT) - y, 0); //Sets new location to be the previous location.
    glutPostRedisplay();
}

//Mouse Function that handles 1x CC speed on LMB, and 2x CC speed on RMB.
void mouseFunc (GLint button, GLint action, GLint x, GLint y)
{
    switch (button)
        {
            case GLUT_LEFT_BUTTON:
                if (action == GLUT_DOWN) { //Processes LMB pressed events.

                    //Read GL Depth into variable for pixel-perfect polygon detection (no bounding box required!!)
                    float depth;
                    glReadPixels(x, glutGet(GLUT_WINDOW_HEIGHT) - y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &depth); //Reads pixels at mouse position and returns depth (0.5 if polygon, 1 if background).
                    MousePos = Vector3(x, glutGet(GLUT_WINDOW_HEIGHT) - y, 0); //Sets mouse position to location of click.

                    //Checks if vertices exist, but polygon does not.
                    if(vertsExist && !polyExists){
                        float distance = sqrt(pow(x - vContainer[0].x, 2) +
                                             pow((glutGet(GLUT_WINDOW_HEIGHT) - y) - vContainer[0].y, 2) * 1.0); //Distance formula as derived from Pythagorean Theorem.

                        if(distance < 7){ //Checks if click is within 5 pixels of the 1st vertex. If so, convert points to polygon.
                            cenCalc(); //Calculate polygon center.
                            initPolygon(); //Draws polygon from vertices.
                            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                            glutIdleFunc (rotatePolygon); //Sets idle function to rotation animation.
                        } else {
                            drawVertex(x, y); //If not within 5 pixels of 1st vertex, draw a new one.
                        }

                    } else if(polyExists && depth != 1) { //Sets polyGrabbed to true if click is registered on polygon depth.
                        polyGrabbed = true;
                    } else {
                        if(!polyExists){ //Handles first time instance where no polygons or vertices exist. Exists for first point placement.
                            drawVertex(x, y);
                            vertsExist = true;
                        }
                    }

                    //Modifiers (Alt, Shift, etc.)
                    switch(glutGetModifiers())
                    {
                        case GLUT_ACTIVE_ALT:
                            if(polyGrabbed){ //Checks to ensure the user has grabbed the polygon.
                                if(RotClock){ //Flips RotClock between true and false.
                                    RotClock = false;
                                } else {
                                    RotClock = true;
                                }
                            }
                            break;
                        case GLUT_ACTIVE_SHIFT:
                            if(polyGrabbed){
                                activeShift = true; //Sets Shift Held to true for reference in mouse motion function.
                            }
                            break;
                        default: //Default Case. Occurs in event of invalid input.
                            break;
                    }
                } else { //Processes LMB release events.
                        activeShift = false;
                        polyGrabbed = false;
                    }
            default: //Default Case for button switch statement.
                break;
        }
}

//Motion Function that occurs when mouse is moved and a button is held.
void motionFunc (GLint x, GLint y)
{
    if(activeShift){ //Scales while mouse is moving and shift is held.
        scalePolygon(1.05);
    }
    if(polyGrabbed && !activeShift){ //Moves polygon while mouse is moving and shift is not held.
        movePolygon(x, y);
    }
}

//Window Reshape function.
void winReshapeFunc (GLint newWidth, GLint newHeight)
{
    glViewport(0, 0, (GLsizei) newWidth, (GLsizei) newHeight);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, (float)newWidth, 0.0, (float)newHeight, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW); glLoadIdentity();
    glPointSize(10); //Set Vertex Size.
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

//Main
int main (int argc, char** argv)
{
    glutInit (&argc, argv);
    glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB);
    int winDim[2] = {720, 720}; //Defines Window Dimensions for later usage.
    glutInitWindowSize(winDim[0], winDim[1]); //Uses X and Y coordinates from previously defined array to set Window Size.
    glutInitWindowPosition( (glutGet(GLUT_SCREEN_WIDTH)-winDim[0])/2, (glutGet(GLUT_SCREEN_HEIGHT)-winDim[1])/2 ); //Centers Window on the Screen.
    glutCreateWindow ("Group Project Part 01");

    glClearColor( bgk.red, bgk.green, bgk.blue, 0.0 );	//Set background color from RGB setup.
    glEnable(GL_DEPTH_TEST); //Enables Depth test for polygon detection.
    glutDisplayFunc(drawPolygon); //Initialize Hexagon
    glutReshapeFunc(winReshapeFunc);
    glutMouseFunc (mouseFunc); //Mouse Function
    glutMotionFunc(motionFunc); //Mouse motion function.
    glutMainLoop();
}
