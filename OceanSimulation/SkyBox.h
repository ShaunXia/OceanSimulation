#include <GL/glew.h>
#include <SDL.h>
#include "SDL_image.h"
#include "GLFundamentals.hpp"

using namespace gl;

class SkyBox
{
public:
    SkyBox(char* left, char* back, char* right, char* front, char* top, char* bottom);
    ~SkyBox();
    void draw(mat4 view);

private:
    enum { SKY_LEFT = 0, SKY_BACK, SKY_RIGHT, SKY_FRONT, SKY_TOP, SKY_BOTTOM };
    unsigned int skybox[6];
    GLuint skyShaders;
    GLuint vbo_verts, vbo_inds;
    GLuint vloc, tloc;
    GLuint modelLoc, projectionLoc, viewLoc;

    unsigned int loadJPG_Texture(char* filename);
};
