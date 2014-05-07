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
    GLuint getTexMap();

private:
    enum { SKY_LEFT = 0, SKY_BACK, SKY_RIGHT, SKY_FRONT, SKY_TOP, SKY_BOTTOM };
    unsigned int skybox[6];
    SDL_Surface* skyTex[6];
    GLuint skyShaders;
    GLuint vbo_verts, vbo_inds, vbo_tex;
    GLuint vloc, nloc, tloc, texMap;
    GLuint modelLoc, projectionLoc, viewLoc;

    void initBasic();
    void initSkyMap();
    unsigned int loadTexture(char* filename);
    void loadTex(char* filename, GLenum face);
    void setupCubeMap();
};

