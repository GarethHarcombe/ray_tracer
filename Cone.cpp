/*----------------------------------------------------------
* COSC363  Ray Tracer
*
*  The cone class
*  This is a subclass of Object, and hence implements the
*  methods intersect() and normal().
-------------------------------------------------------------*/

#include "Cone.h"
#include <math.h>

/**
* Cone's intersection method.  The input is a ray. 
*/
float Cone::intersect(glm::vec3 p0, glm::vec3 dir)
{
    glm::vec3 vdif = p0 - center;   //Vector s (see Slide 28)
    float a = pow(dir.x, 2) + pow(dir.z, 2) - pow(radius / height, 2) * pow(dir.y, 2);
    float b = 2.0 * (dir.x * vdif.x + dir.z * vdif.z) - pow(radius / height, 2) * (-2.0 * height * dir.y + 2.0 * dir.y * vdif.y);
    float c = pow(vdif.x, 2) + pow(vdif.z, 2) - pow(radius / height, 2) * (pow(height, 2) - 2 * height * vdif.y + pow(vdif.y, 2));
    float delta = b*b - 4.0 * a * c;
   
	if(delta < 0.001) return -1.0;    //includes zero and negative values

    float t1 = (-b - sqrt(delta)) / (2.0 * a);
    float t2 = (-b + sqrt(delta)) / (2.0 * a);
	
	glm::vec3 intersectionPointT1 = p0 + t1 * dir;
	glm::vec3 intersectionPointT2 = p0 + t2 * dir;
	if (intersectionPointT1.y < center.y + height && intersectionPointT1.y > center.y) return t1;
	else if (intersectionPointT2.y < center.y + height && intersectionPointT2.y > center.y) return t2;
	else return -1;
}

/**
* Returns the unit normal vector at a given point.
* Assumption: The input point p lies on the cone.
*/
glm::vec3 Cone::normal(glm::vec3 p)
{
	glm::vec3 vdif = p - center;
	float theta = atan(radius / height);
	float alpha = atan(vdif.x / vdif.z);
	glm::vec3 n = glm::vec3(vdif.x, sin(theta), vdif.z);
    n = glm::normalize(n);
    return n;
}
