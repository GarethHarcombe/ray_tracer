/*----------------------------------------------------------
* COSC363  Ray Tracer
*
*  The cylinder class
*  This is a subclass of Object, and hence implements the
*  methods intersect() and normal().
-------------------------------------------------------------*/

#include "Cylinder.h"
#include <math.h>

/**
* Cylinder's intersection method.  The input is a ray. 
*/
float Cylinder::intersect(glm::vec3 p0, glm::vec3 dir)
{
    glm::vec3 vdif = p0 - center;   //Vector s (see Slide 28)
    float a = dir.x * dir.x + dir.z * dir.z;
    float b = 2 * (dir.x * vdif.x + dir.z * vdif.z);
    float c = vdif.x * vdif.x + vdif.z * vdif.z - radius * radius;
    float delta = b*b - 4 * a * c;
   
	if(delta < 0.001) return -1.0;    //includes zero and negative values

    float t1 = (-b - sqrt(delta)) / (2 * a);
    float t2 = (-b + sqrt(delta)) / (2 * a);
	
	glm::vec3 intersectionPointT1 = p0 + t1 * dir;
	glm::vec3 intersectionPointT2 = p0 + t2 * dir;
	if (intersectionPointT1.y < center.y + height && intersectionPointT1.y > center.y) return t1;
	else if (intersectionPointT2.y < center.y + height && intersectionPointT2.y > center.y) return t2;
	else return -1;
}

/**
* Returns the unit normal vector at a given point.
* Assumption: The input point p lies on the cylinder.
*/
glm::vec3 Cylinder::normal(glm::vec3 p)
{
    glm::vec3 n = p - center;
    n.y = 0;
    n = glm::normalize(n);
    return n;
}
