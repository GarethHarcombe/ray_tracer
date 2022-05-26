/*==================================================================================
* COSC 363  Computer Graphics (2022)
* Department of Computer Science and Software Engineering, University of Canterbury.
*
* A basic ray tracer
* See Lab07.pdf  for details.
*===================================================================================
*/
#include <iostream>
#include <cmath>
#include <vector>
#include <glm/glm.hpp>
#include "Sphere.h"
#include "SceneObject.h"
#include "Ray.h"
#include <GL/freeglut.h>
#include "Plane.h"
#include "Cylinder.h"
#include "Cone.h"
#include "Torus.h"
#include "TextureBMP.h"
using namespace std;

const float PI = 3.14159265;
const float EDIST = 40.0;
const int NUMDIV = 1000;
const int MAX_STEPS = 5;
const float XMIN = -10.0;
const float XMAX = 10.0;
const float YMIN = -10.0;
const float YMAX = 10.0;
const float E = 2.71828;

vector<SceneObject*> sceneObjects;

// g++ RayTracer.cpp SceneObject.cpp Ray.cpp TextureBMP.cpp Plane.cpp Sphere.cpp Cylinder.cpp Cone.cpp quartic.cpp Torus.cpp -o program  -lm -lGL -lGLU -lglut -lGLEW


/* TODO: Extra Marks
 * Cone/cylinder 					(2 Marks)
 * Torus							(2-3 Marks)
 * Refraction 						(1 Mark)
 * Proceedural Generated pattern 	(1-2 Marks)
 *  
 */


TextureBMP texture;


int cmodulus(int a, int b)
{
	return (b + (a % b)) % b;
}

float customSine(float t)
{
	while (t > 3 * PI) t = t - 3 * PI;
	if (t < 2 * PI) return (sin(t - PI / 2.0) + 1) / 2.0;
	else return 0.0f;
}

glm::vec3 shadowColor(glm::vec3 lightPos, Ray ray, glm::vec3 color)
{
	glm::vec3 lightVec = lightPos - ray.hit;
	Ray shadowRay(ray.hit, lightVec);
	
	shadowRay.closestPt(sceneObjects);
	
	float lightDist = glm::length(lightVec);
	
	if (shadowRay.index > -1 && shadowRay.dist < lightDist)
	{
		if (sceneObjects[shadowRay.index]->isTransparent())
		{
			return 0.5f * color + 0.5f * sceneObjects[shadowRay.index]->getColor();
		}
		else if (sceneObjects[shadowRay.index]->isRefractive())
		{
			return 0.5f * color;
		}
		else 
		{
			return 0.2f * color; //0.2 = ambient scale factor
		}
	}
	return color;
}

//---The most important function in a ray tracer! ---------------------------------- 
//   Computes the colour value obtained by tracing a ray and finding its 
//     closest point of intersection with objects in the scene.
//----------------------------------------------------------------------------------
glm::vec3 trace(Ray ray, int step)
{
	glm::vec3 backgroundCol(0);						//Background colour = (0,0,0)
	glm::vec3 backgroundFog(1);
	glm::vec3 lightPos(50, 40, 80);					//Light's position
	glm::vec3 lightPos2(-50, 40, 80);
	glm::vec3 spotPos(-17.0, 5.0, -120.0);
	glm::vec3 spotDir(0, 1, -0.3);
	glm::vec3 color(0);
	SceneObject* obj;

    ray.closestPt(sceneObjects);					//Compare the ray with all objects in the scene
    if(ray.index == -1) 
    {
		if (step == 1) return backgroundFog;
		else return backgroundCol;		//no intersection
	}
	obj = sceneObjects[ray.index];					//object on which the closest point of intersection is found
	
	if (ray.index == 6)
	{
		//Stripe pattern
		int stripeWidthZ = 5;
		int stripeWidthX = 2;
		int iz = (ray.hit.z) / stripeWidthZ;
		int ix = (ray.hit.x + 100) / stripeWidthX;  //Add 100 to fix mod issues around 0
		int k = cmodulus(iz, 2); //2 colors
		int j = cmodulus(ix, 2);
		if (k == j) color = glm::vec3(0, 0.7, 0);
		else color = glm::vec3(0.8, 0.8, 0.4);
		obj->setColor(color);
	}
	
	if (ray.index == 7)
	{
		// Texturing
		float x1 = -40.0f;
		float x2 = 40.0f;
		float y1 = -25.0f;
		float y2 = 25.0f;
		
		float texcoords = (ray.hit.x - x1)/(x2 - x1);
		float texcoordt = (ray.hit.y - y1)/(y2 - y1);
		if(texcoords > 0 && texcoords < 1 && texcoordt > 0 && texcoordt < 1)
		{
			color=texture.getColorAt(1 - texcoordt, texcoords);
			obj->setColor(color);
		}
	}
	
	if (ray.index == 4)   // Proceedural generation
	{
		// Proceedural polka dots
		float y = sin(ray.hit.x) + cos(ray.hit.z);
		if (y > 0.7) color = glm::vec3(0.75, 0, 0);
		else color = glm::vec3(0, 0.75, 0);
		
		obj->setColor(color);
	}
	
	if (ray.index == 0)
	{
		// Proceedural rainbow
		glm::vec3 center(0.0, -10.0, ray.hit.z);
		glm::vec3 diff = ray.hit - center;
		float diffLength = glm::length(diff);
		color = glm::vec3(customSine(diffLength), customSine(diffLength + PI), customSine(diffLength + 2 * PI));
		obj->setColor(color);
	}

	color = obj->lighting(lightPos2, -ray.dir, ray.hit).first + obj->lighting(lightPos, -ray.dir, ray.hit).second + obj->lighting(lightPos2, -ray.dir, ray.hit).second;
	//color = obj->getColor();						//Object's colour
	
	color = shadowColor(lightPos, ray, color);
	color = shadowColor(lightPos2, ray, color);
	
	glm::vec3 spotVec = spotPos - ray.hit;
	float angleDiff = acos(glm::dot(spotVec, spotDir) / (glm::length(spotVec) * glm::length(spotDir)));
	Ray spotRay(ray.hit, spotVec);
	spotRay.closestPt(sceneObjects);
	if (angleDiff < 0.4f)
	{
		if (spotRay.index == -1 || sceneObjects[spotRay.index]->isTransparent() || sceneObjects[spotRay.index]->isRefractive()) 
		{
			color = color + obj->getColor();
		}
	}
	
	if (obj->isTransparent() && step < MAX_STEPS)
	{
		float rho = obj->getTransparencyCoeff();
		Ray insideRay(ray.hit, ray.dir);
		insideRay.closestPt(sceneObjects);
		
		Ray transparentRay(insideRay.hit, ray.dir);
		transparentRay.closestPt(sceneObjects);
		glm::vec3 transparentColor = trace(transparentRay, step + 1);
		color = color + (rho * transparentColor);
	}
	
	if (obj->isRefractive() && step < MAX_STEPS)
	{
		float rho = obj->getRefractionCoeff();
		glm::vec3 n = obj->normal(ray.hit);
		float eta = 1.0 / obj->getRefractiveIndex();
		glm::vec3 g = glm::refract(ray.dir, n, eta);    // eta = n1/n2
		
		Ray refrRay(ray.hit, g);
		refrRay.closestPt(sceneObjects);
		glm::vec3 m = obj->normal(refrRay.hit);
		glm::vec3 h = glm::refract(g, -m, 1.0f/eta);
		
		Ray refrRay2(refrRay.hit, h);
		refrRay2.closestPt(sceneObjects);
		glm::vec3 refractedColor = trace(refrRay2, step + 1);
		color = color + (rho * refractedColor);
	}
	
	if (obj->isReflective() && step < MAX_STEPS)
	{
		float rho = obj->getReflectionCoeff();
		glm::vec3 normalVec = obj->normal(ray.hit);
		glm::vec3 reflectedDir = glm::reflect(ray.dir, normalVec);
		Ray reflectedRay(ray.hit, reflectedDir);
		glm::vec3 reflectedColor = trace(reflectedRay, step + 1);
		color = color + (rho * reflectedColor);
	}
	
	float z1 = -140.0;
	float z2 = -210.0;
	float t = (ray.hit.z - z1) / (z2 - z1);
	if (t > 0) color = (1 - t) * color + t * glm::vec3(1, 1, 1);
	
	return color;
}

//---The main display module -----------------------------------------------------------
// In a ray tracing application, it just displays the ray traced image by drawing
// each cell as a quad.
//---------------------------------------------------------------------------------------
void display()
{
	float xp, yp;  //grid point
	float cellX = (XMAX - XMIN) / NUMDIV;  //cell width
	float cellY = (YMAX - YMIN) / NUMDIV;  //cell height
	glm::vec3 eye(0., 0., 0.);

	glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

	glBegin(GL_QUADS);  //Each cell is a tiny quad.

	for(int i = 0; i < NUMDIV; i++)	//Scan every cell of the image plane
	{
		xp = XMIN + i * cellX;
		for(int j = 0; j < NUMDIV; j++)
		{
			yp = YMIN + j * cellY;
			
			// Anti aliasing
			glm::vec3 dir1(xp + 0.25 * cellX, yp + 0.25 * cellY, -EDIST);	//direction of the primary ray
		    glm::vec3 dir2(xp + 0.25 * cellX, yp + 0.75 * cellY, -EDIST);	//direction of the primary ray
		    glm::vec3 dir3(xp + 0.75 * cellX, yp + 0.25 * cellY, -EDIST);	//direction of the primary ray
		    glm::vec3 dir4(xp + 0.75 * cellX, yp + 0.75 * cellY, -EDIST);	//direction of the primary ray

		    Ray ray1 = Ray(eye, dir1);
		    Ray ray2 = Ray(eye, dir2);
		    Ray ray3 = Ray(eye, dir3);
		    Ray ray4 = Ray(eye, dir4);

		    glm::vec3 col1 = trace (ray1, 1); //Trace the primary ray and get the colour value
		    glm::vec3 col2 = trace (ray2, 1);
		    glm::vec3 col3 = trace (ray3, 1);
		    glm::vec3 col4 = trace (ray4, 1);
		    
			glColor3f((col1.r + col2.r + col3.r + col4.r)/4, (col1.g + col2.g + col3.g + col4.g)/4, (col1.b + col2.b + col3.b + col4.b)/4);
			glVertex2f(xp, yp);				//Draw each cell with its color value
			glVertex2f(xp + cellX, yp);
			glVertex2f(xp + cellX, yp + cellY);
			glVertex2f(xp, yp + cellY);

		    
		    
		    
		    //glm::vec3 dir(xp + 0.5 * cellX, yp + 0.5 * cellY, -EDIST);	//direction of the primary ray

		    //Ray ray = Ray(eye, dir);

		    //glm::vec3 col = trace (ray, 1); //Trace the primary ray and get the colour value
			//glColor3f(col.r, col.g, col.b);
			//glVertex2f(xp, yp);				//Draw each cell with its color value
			//glVertex2f(xp + cellX, yp);
			//glVertex2f(xp + cellX, yp + cellY);
			//glVertex2f(xp, yp + cellY);
        }
    }

    glEnd();
    glFlush();
}

void cube(glm::vec3 origin, float width, float height, float length)
{
	glm::vec3 colour(0.8, 0.8, 0);
	
	glm::vec3 point1(origin.x, origin.y, origin.z);
	glm::vec3 point2(origin.x + width, origin.y, origin.z);
	glm::vec3 point3(origin.x + width, origin.y + height, origin.z);
	glm::vec3 point4(origin.x, origin.y + height, origin.z);
	glm::vec3 point5(origin.x, origin.y, origin.z + length);
	glm::vec3 point6(origin.x + width, origin.y, origin.z + length);
	glm::vec3 point7(origin.x + width, origin.y + height, origin.z + length);
	glm::vec3 point8(origin.x, origin.y + height, origin.z + length);
	
	Plane *plane1 = new Plane (point1, point2, point3, point4);  // Front
	plane1->setSpecularity(false);
	plane1->setColor(colour);
	sceneObjects.push_back(plane1);
	
	Plane *plane2 = new Plane (point1, point5, point8, point4);  //Left side
	plane2->setSpecularity(false);
	plane2->setColor(colour);
	sceneObjects.push_back(plane2);
	
	Plane *plane3 = new Plane (point2, point6, point7, point3);  // Right side
	plane3->setSpecularity(false);
	plane3->setColor(colour);
	sceneObjects.push_back(plane3);
	
	Plane *plane4 = new Plane (point1, point5, point6, point2);  // Bottom
	plane4->setSpecularity(false);
	plane4->setColor(colour);
	sceneObjects.push_back(plane4);
	
	Plane *plane5 = new Plane (point4, point3, point7, point8);  // Top
	plane5->setSpecularity(false);
	plane5->setColor(colour);
	sceneObjects.push_back(plane5);
	
	Plane *plane6 = new Plane (point5, point6, point7, point8);  // Back
	plane6->setSpecularity(false);
	plane6->setColor(colour);
	sceneObjects.push_back(plane6);
}


//---This function initializes the scene ------------------------------------------- 
//   Specifically, it creates scene objects (spheres, planes, cones, cylinders etc)
//     and add them to the list of scene objects.
//   It also initializes the OpenGL 2D orthographc projection matrix for drawing the
//     the ray traced image.
//----------------------------------------------------------------------------------
void initialize()
{
    glMatrixMode(GL_PROJECTION);
    gluOrtho2D(XMIN, XMAX, YMIN, YMAX);

    glClearColor(0, 0, 0, 1);

	texture = TextureBMP("Butterfly.bmp");
	
	glm::vec3 origin(-20.0, -15.0, -100.0);
	float width = 40.0f;  // x left/right
	float length = 1.0f;  // y up/down
	float height = -50.0f;  // z forward/back
	cube(origin, width, length, height);
	
	Plane *floor = new Plane (glm::vec3(-40., -25, -40), //Point A
						   	  glm::vec3(40., -25, -40), //Point B
							  glm::vec3(40., -25, -180), //Point C
							  glm::vec3(-40., -25, -180)); //Point D
	floor->setSpecularity(false);
	floor->setColor(glm::vec3(0.8, 0.8, 0));
	sceneObjects.push_back(floor);
	
	Plane *backPlane = new Plane (glm::vec3(-40., -25, -180), //Point A
								  glm::vec3(40., -25, -180), //Point B
								  glm::vec3(40., 25, -180), //Point C
								  glm::vec3(-40., 25, -180)); //Point D
	backPlane->setSpecularity(false);
	backPlane->setColor(glm::vec3(0.0, 0.0, 0.0));
	sceneObjects.push_back(backPlane);
	
	Sphere *sphere1 = new Sphere(glm::vec3(-13.0, -11.0, -117.0), 3.0);
	sphere1->setReflectivity(true, 0.8);
	//sphere1->setSpecularity(false);
	sphere1->setColor(glm::vec3(0, 0, 1));   //Set colour to blue
	sceneObjects.push_back(sphere1);		 //Add sphere to scene objects
	
	Sphere *sphere2 = new Sphere(glm::vec3(0, -11.0, -105.0), 3.0);
	sphere2->setColor(glm::vec3(0, 0, 0));   //Set colour to black
	sphere2->setRefractivity(true, 1.0, 1.01);
	sphere2->setReflectivity(true, 0.15);
	sceneObjects.push_back(sphere2);		 //Add sphere to scene objects
	
	Sphere *sphere3 = new Sphere(glm::vec3(13.0, -8.0, -120.0), 3.0);
	sphere3->setColor(glm::vec3(1, 0, 0));   //Set colour to green
	sphere3->setTransparency(true, 0.5);
	sphere3->setReflectivity(true, 0.15);
	sceneObjects.push_back(sphere3);		 //Add sphere to scene objects
	
	Cone *cone3 = new Cone(glm::vec3(13.0, -14.0, -120.0), 2.0, 4.0);  // radius then height
	cone3->setColor(glm::vec3(0.8, 0, 0));
	sceneObjects.push_back(cone3);
	
	Cylinder *cylinder1 = new Cylinder(glm::vec3(-18.0, -25.0, -105.0), 1.0, 10.0);
	cylinder1->setColor(glm::vec3(0.5, 0, 0.5));
	sceneObjects.push_back(cylinder1);
	
	Cylinder *cylinder2 = new Cylinder(glm::vec3(18.0, -25.0, -105.0), 1.0, 10.0);
	cylinder2->setColor(glm::vec3(0.5, 0, 0.5));
	sceneObjects.push_back(cylinder2);
	
	Cylinder *cylinder3 = new Cylinder(glm::vec3(-18.0, -25.0, -145.0), 1.0, 10.0);
	cylinder3->setColor(glm::vec3(0.5, 0, 0.5));
	sceneObjects.push_back(cylinder3);
	
	Cylinder *cylinder4 = new Cylinder(glm::vec3(18.0, -25.0, -145.0), 1.0, 10.0);
	cylinder4->setColor(glm::vec3(0.5, 0, 0.5));
	sceneObjects.push_back(cylinder4);
	
	//Torus *torus = new Torus(glm::vec3(13.0, 5.0, -80.0), 10.0, 5.0);
	//torus->setColor(glm::vec3(1, 0, 1));
	//sceneObjects.push_back(torus);

}


int main(int argc, char *argv[]) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB );
    glutInitWindowSize(500, 500);
    glutInitWindowPosition(20, 20);
    glutCreateWindow("Raytracing");

    glutDisplayFunc(display);
    initialize();

    glutMainLoop();
    return 0;
}
