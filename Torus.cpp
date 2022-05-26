/*----------------------------------------------------------
* COSC363  Ray Tracer
*
*  The torus class
*  This is a subclass of Object, and hence implements the
*  methods intersect() and normal().
-------------------------------------------------------------*/

#include "Torus.h"
#include "quartic.h"
#include <list>
#include <math.h>

/**
* Torus's intersection method.  The input is a ray. 
*/
float Torus::intersect(glm::vec3 p0, glm::vec3 dir)
{
    glm::vec3 vdif = p0 - center;
    float a = pow(pow(dir.x, 2) + pow(dir.y, 2) + pow(dir.z, 2), 2);
    float b = 4 * (pow(dir.x, 2) + pow(dir.y, 2) + pow(dir.z, 2)) * (vdif.x * dir.x + vdif.y * dir.y + vdif.z * dir.z);
    float c = 2 * (pow(dir.x, 2) + pow(dir.y, 2) + pow(dir.z, 2)) * (pow(vdif.x, 2) + pow(vdif.y, 2) + pow(vdif.z, 2) - (pow(outerRadius, 2) + pow(radius, 2))) + 4 * pow(vdif.x * dir.x + vdif.y * dir.y + vdif.z * dir.z, 2) + 4 * pow(radius, 2) * pow(dir.y, 2);
    float d = 4 * (pow(vdif.x, 2) + pow(vdif.y, 2) + pow(vdif.z, 2) - (pow(outerRadius, 2) + pow(radius, 2))) * (vdif.x * dir.x + vdif.y * dir.y + vdif.z * dir.z) + 8 * pow(radius, 2) * vdif.y * dir.y;
    float e = pow(pow(vdif.x, 2) + pow(vdif.y, 2) + pow(vdif.z, 2) - (pow(outerRadius, 2) + pow(radius, 2)), 2) - 4 * pow(radius, 2) * (pow(outerRadius, 2) - pow(vdif.y, 2));
    
    std::complex<double>* solutions = solve_quartic(a, b, c, d);
    
    
    std::list<float> solns;
    for (int i = 0; i < 4; i++) 
    {
		if (solutions[i].imag() == 0.0 && solutions[i].real() >= 0.0)  // Only want real, positive roots
		{
			solns.push_back(solutions[i].real());
		}
	}
	
	solns.sort();
    if (solns.size() >= 2) return solns.front();
    else return -1;
}

/**
* Returns the unit normal vector at a given point.
* Assumption: The input point p lies on the torus.
*/
glm::vec3 Torus::normal(glm::vec3 p)
{
	glm::vec3 vdif = p - center;
	//float theta = atan(radius / height);
	//float alpha = atan(vdif.x / vdif.z);
	//glm::vec3 n = glm::vec3(vdif.x, sin(theta), vdif.z);
    glm::vec3 n = glm::normalize(vdif);
    return n;
}
