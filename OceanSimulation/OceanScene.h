#include "GLFundamentals.hpp"
#include "GLDemonstration.hpp"
#include "Ocean.h"
#include "SkyBox.h"

using namespace gl;

class OceanScene : public demonstration
{
private:
    Ocean* ocean;
    SkyBox* skybox;
    void draw();
    void step();
public:
    OceanScene();
    ~OceanScene();
};

