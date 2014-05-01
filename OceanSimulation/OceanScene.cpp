#include "OceanScene.h"

#define OCEAN 1
#define SKY 0

OceanScene::OceanScene() : demonstration("Ocean", 512, 512)
{
#if(OCEAN)
    //ocean = new Ocean(64, 64, 64, 80, 0.00035, 0.000001, vec3(32.0, 32.0, 0.0));
    ocean = new Ocean(64, 64, 100, 80, 0.000015, 0.000001, vec3(32.0, 32.0, 0.0));
    cam_position = vec3(3 * 64 / 4, 2.0, -3 * 10);
#endif
#if(SKY)
    char* left = "Textures\\sky\\bluecloud_lf.jpg";
    char* back = "Textures\\sky\\bluecloud_bk.jpg";
    char* right = "Textures\\sky\\bluecloud_rt.jpg";
    char* front = "Textures\\sky\\bluecloud_ft.jpg";
    char* top = "Textures\\sky\\bluecloud_up.jpg";
    char* bottom = "Textures\\sky\\bluecloud_dn.jpg";
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