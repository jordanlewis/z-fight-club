#ifndef GRAPHICS_H
#define GRAPHICS_H
#include "world.h"

class Graphics {
    public:
	Graphics();
	~Graphics();

	void InitGraphics();
	void render(const World * const world);

};
    
#endif
