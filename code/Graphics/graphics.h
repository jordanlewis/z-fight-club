#ifndef GRAPHICS_H
#define GRAPHICS_H
#include "../Engine/world.h"
#include "../Utilities/vec3f.h"
#include "color.h"

class Graphics {
    	bool 	initialized;	/* !<is opengl ready to go */
    public:
	Graphics();
	~Graphics();

	void initGraphics();
	void render(const World * const world);
	int sphere(Vec3f, float, Color);
	int arrow(Vec3f, Vec3f);
};
    
#endif
