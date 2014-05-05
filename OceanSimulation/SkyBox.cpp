#include "SkyBox.h"
struct vert
{
    GLfloat v[3];
    GLfloat n[3];
    GLfloat t[2];
};

static const struct vert verts[24] = {

    /* +X */
    { { 50.f, 50.f, 50.f }, { 1.f, 0.f, 0.f }, { 0.f, 0.f } },
    { { 50.f, -50.f, 50.f }, { 1.f, 0.f, 0.f }, { 0.f, 1.f } },
    { { 50.f, -50.f, -50.f }, { 1.f, 0.f, 0.f }, { 1.f, 1.f } },
    { { 50.f, 50.f, -50.f }, { 1.f, 0.f, 0.f }, { 1.f, 0.f } },

    /* -X */
    { { -50.f, 50.f, -50.f }, { -1.f, 0.f, 0.f }, { 0.f, 0.f } },
    { { -50.f, -50.f, -50.f }, { -1.f, 0.f, 0.f }, { 0.f, 1.f } },
    { { -50.f, -50.f, 50.f }, { -1.f, 0.f, 0.f }, { 1.f, 1.f } },
    { { -50.f, 50.f, 50.f }, { -1.f, 0.f, 0.f }, { 1.f, 0.f } },

    /* +Y */
    { { -50.f, 50.f, -50.f }, { 0.f, 1.f, 0.f }, { 0.f, 0.f } },
    { { -50.f, 50.f, 50.f }, { 0.f, 1.f, 0.f }, { 0.f, 1.f } },
    { { 50.f, 50.f, 50.f }, { 0.f, 1.f, 0.f }, { 1.f, 1.f } },
    { { 50.f, 50.f, -50.f }, { 0.f, 1.f, 0.f }, { 1.f, 0.f } },

    /* -Y */
    { { -50.f, -50.f, 50.f }, { 0.f, -1.f, 0.f }, { 0.f, 0.f } },
    { { -50.f, -50.f, -50.f }, { 0.f, -1.f, 0.f }, { 0.f, 1.f } },
    { { 50.f, -50.f, -50.f }, { 0.f, -1.f, 0.f }, { 1.f, 1.f } },
    { { 50.f, -50.f, 50.f }, { 0.f, -1.f, 0.f }, { 1.f, 0.f } },

    /* +Z */
    { { -50.f, 50.f, 50.f }, { 0.f, 0.f, 1.f }, { 0.f, 0.f } },
    { { -50.f, -50.f, 50.f }, { 0.f, 0.f, 1.f }, { 0.f, 1.f } },
    { { 50.f, -50.f, 50.f }, { 0.f, 0.f, 1.f }, { 1.f, 1.f } },
    { { 50.f, 50.f, 50.f }, { 0.f, 0.f, 1.f }, { 1.f, 0.f } },

    /* -Z */
    { { 50.f, 50.f, -50.f }, { 0.f, 0.f, -1.f }, { 0.f, 0.f } },
    { { 50.f, -50.f, -50.f }, { 0.f, 0.f, -1.f }, { 0.f, 1.f } },
    { { -50.f, -50.f, -50.f }, { 0.f, 0.f, -1.f }, { 1.f, 1.f } },
    { { -50.f, 50.f, -50.f }, { 0.f, 0.f, -1.f }, { 1.f, 0.f } },
};

static const GLushort indices[36] =
{
    0, 1, 2, 0, 2, 3,
    5, 4, 6, 6, 4, 7,
    8, 9, 10, 10, 11, 8,
    13, 12, 14, 14, 12, 15,
    17, 16, 18, 18, 16, 19,
    21, 22, 20, 22, 23, 20
};

SkyBox::SkyBox(char* left, char* back, char* right, char* front, char* top, char* bottom)
{
    const size_t sz = sizeof (GLfloat);

    glEnable(GL_TEXTURE_2D);

    //Load textures
    skybox[0] = loadJPG_Texture(right);
    skybox[1] = loadJPG_Texture(left);
    skybox[2] = loadJPG_Texture(top);
    skybox[3] = loadJPG_Texture(bottom);
    skybox[4] = loadJPG_Texture(back);
    skybox[5] = loadJPG_Texture(front);

    //Set up buffer objects
    glGenBuffers(1, &vbo_verts);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_verts);
    glBufferData(GL_ARRAY_BUFFER, 24 * sizeof(vert), verts, GL_STATIC_DRAW);

    glGenBuffers(1, &vbo_inds);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_inds);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 36 * sizeof(GLushort), indices, GL_STATIC_DRAW);

    //Load shaders
    skyShaders = init_program("Shaders\\sky_vshader.glsl", "Shaders\\sky_fshader.glsl");
    glUseProgram(skyShaders);

    //Get attribute and uniform locations
    vloc = glGetAttribLocation(skyShaders, "vPosition");
    tloc = glGetAttribLocation(skyShaders, "vTexCoord");
    modelLoc = glGetUniformLocation(skyShaders, "Model");
    projectionLoc = glGetUniformLocation(skyShaders, "Projection");
    viewLoc = glGetUniformLocation(skyShaders, "View");

    glClearColor(0.53, 0.81, 0.98, 1.0);
}


SkyBox::~SkyBox()
{
    glDeleteTextures(6, &skybox[0]);
}

unsigned int SkyBox::loadJPG_Texture(char* filename)
{
    //Generate texture id
    unsigned int id;
    glGenTextures(1, &id);

    //load SDL surface
    SDL_Surface *img;
    img = IMG_Load(filename);

    if (!img)
    {
        printf("IMG_Load: %s\n", IMG_GetError());
    }
    //Get properties
    GLint f = img->format->BytesPerPixel;
    GLenum texFormat;
    if (f == 4) texFormat = GL_RGBA;
    else if (f == 3) texFormat = GL_RGB;

    //Bind the texture
    glBindTexture(GL_TEXTURE_2D, id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, img->w, img->h, 0, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, img->pixels);
    glTexImage2D(GL_TEXTURE_2D, 0, f, img->w, img->h, 0, texFormat, GL_UNSIGNED_BYTE, img->pixels);
    SDL_FreeSurface(img);

    return id;
}

void SkyBox::draw(mat4 view)
{
    glUseProgram(skyShaders);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_verts);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_inds);

    //Disable writes to depth buffer so the skybox will always appear "behind" everything else
    glDisable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);

    mat4 projection = perspective(45.0, 1.0, 0.1, 100.0);
    mat4 model = translation(vec3(0.0, 0.0, 0.0)) * xrotation(0.0) * yrotation(0.0);
    glUniformMatrix4fv(projectionLoc, 1, GL_TRUE, projection);
    glUniformMatrix4fv(modelLoc, 1, GL_TRUE, model);
    glUniformMatrix4fv(viewLoc, 1, GL_TRUE, view);

    const size_t sz = sizeof (GLfloat);
    glEnableVertexAttribArray(vloc);
    glEnableVertexAttribArray(tloc);

    glVertexAttribPointer(vloc, 3, GL_FLOAT, GL_FALSE, sz * 8, 0);
    glVertexAttribPointer(tloc, 2, GL_FLOAT, GL_FALSE, sz * 8, (void*)(sz * 6));

    for (int i = 0; i < 6; ++i)
    {
        glBindTexture(GL_TEXTURE_2D, skybox[i]);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, (void*)(i*sizeof(GLushort)* 6));
    }

    //Re-enable depth buffer
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
}