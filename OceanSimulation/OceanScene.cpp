#include "OceanScene.h"

OceanScene::OceanScene() : demonstration("Ocean", 512, 512)
{
    //Ocean parameters: (gridX, gridZ, patchLength, period, PhillipsConstant, Suppressor, windDirection)
    ocean = new Ocean(64, 64, 80, 90, 0.000085, 0.000001, vec3(10.0, 0.0, 0.0));

    char* left = "Textures\\skymap\\xneg.png";
    char* back = "Textures\\skymap\\zpos.png";
    char* right = "Textures\\skymap\\xpos.png";
    char* front = "Textures\\skymap\\zneg.png";
    char* top = "Textures\\skymap\\ypos.png";
    char* bottom = "Textures\\skymap\\yneg.png";
    skybox = new SkyBox(left, back, right, front, top, bottom);

    ocean->setSkyMap(skybox->getTexMap());
    cam_position = vec3(3*22.5, 10.0, -3*22.5);
}


OceanScene::~OceanScene()
{
}

void OceanScene::draw()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    skybox->draw(view(), cam_position);
    ocean->draw(view(), cam_position);
}

void OceanScene::step()
{
    ocean->step();
    mat3 N = normal(inverse(view()));
    cam_position = cam_position + N * cam_velocity / 5.0;
}