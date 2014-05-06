#include "OceanScene.h"

#define OCEAN 0
#define SKY 1

OceanScene::OceanScene() : demonstration("Ocean", 512, 512)
{
#if(OCEAN)
    //ocean = new Ocean(64, 64, 64, 80, 0.00035, 0.000001, vec3(32.0, 32.0, 0.0));
    ocean = new Ocean(64, 64, 80, 90, 0.000085, 0.000001, vec3(10.0, 0.0, 0.0));
    //cam_position = vec3(3 * 64 / 4, 2.0, -3 * 10);
#endif
#if(SKY)
    char* left = "Textures\\skymap\\xneg.png";
    char* back = "Textures\\skymap\\zpos.png";
    char* right = "Textures\\skymap\\xpos.png";
    char* front = "Textures\\skymap\\zneg.png";
    char* top = "Textures\\skymap\\ypos.png";
    char* bottom = "Textures\\skymap\\yneg.png";
    skybox = new SkyBox(left, back, right, front, top, bottom);
    cam_position = vec3(0.0, 0.0, 0.0);
#endif
}


OceanScene::~OceanScene()
{
}

void OceanScene::draw()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
#if(SKY)
    skybox->draw(view());
#endif
#if(OCEAN)
    ocean->draw(view());
#endif
}

void OceanScene::step()
{
#if(OCEAN)
    ocean->step();
#endif
    mat3 N = normal(inverse(view()));
    cam_position = cam_position + N * cam_velocity / 5.0;
}