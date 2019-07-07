#ifndef CUBE_H
#define CUBE_H


#include <GL/glew.h>


class Cube
{

private:
	unsigned int vao;

public:
	Cube();

	void render();
};

#endif //CUBE_H
