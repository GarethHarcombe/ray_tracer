/*----------------------------------------------------------
* COSC363  Ray Tracer
*
*  The torus class
*  This is a subclass of Object, and hence implements the
*  methods intersect() and normal().
-------------------------------------------------------------*/

#ifndef H_TORUS
#define H_TORUS
#include <glm/glm.hpp>
#include "SceneObject.h"

/**
 * Defines a simple Torus located at 'center'
 * with the specified radius
 */
class Torus : public SceneObject
{

private:
    glm::vec3 center = glm::vec3(0);
    float radius = 3;
    float outerRadius = 1;

public:
	Torus() {};  //Default constructor creates a unit torus

	Torus(glm::vec3 c, float R, float r) : center(c), radius(R), outerRadius(r) {}

	float intersect(glm::vec3 p0, glm::vec3 dir);

	glm::vec3 normal(glm::vec3 p);

};

#endif //!H_TORUS
