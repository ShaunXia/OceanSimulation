#include "SkyBox.h"

#define CUBEMAP 1

struct vert
{
    GLfloat v[3];
    GLfloat n[3];
    GLfloat t[2];
};

static const struct vert verts[24] = 
{
    /* +X */
    { { 1.f, 1.f, 1.f }, { 1.f, 0.f, 0.f }, { 0.f, 0.f } },
    { { 1.f, -1.f, 1.f }, { 1.f, 0.f, 0.f }, { 0.f, 1.f } },
    { { 1.f, -1.f, -1.f }, { 1.f, 0.f, 0.f }, { 1.f, 1.f } },
    { { 1.f, 1.f, -1.f }, { 1.f, 0.f, 0.f }, { 1.f, 0.f } },

    /* -X */
    { { -1.f, 1.f, -1.f }, { -1.f, 0.f, 0.f }, { 0.f, 0.f } },
    { { -1.f, -1.f, -1.f }, { -1.f, 0.f, 0.f }, { 0.f, 1.f } },
    { { -1.f, -1.f, 1.f }, { -1.f, 0.f, 0.f }, { 1.f, 1.f } },
    { { -1.f, 1.f, 1.f }, { -1.f, 0.f, 0.f }, { 1.f, 0.f } },

    /* +Y */
    { { -1.f, 1.f, -1.f }, { 0.f, 1.f, 0.f }, { 0.f, 0.f } },
    { { -1.f, 1.f, 1.f }, { 0.f, 1.f, 0.f }, { 0.f, 1.f } },
    { { 1.f, 1.f, 1.f }, { 0.f, 1.f, 0.f }, { 1.f, 1.f } },
    { { 1.f, 1.f, -1.f }, { 0.f, 1.f, 0.f }, { 1.f, 0.f } },

    /* -Y */
    { { -1.f, -1.f, 1.f }, { 0.f, -1.f, 0.f }, { 0.f, 0.f } },
    { { -1.f, -1.f, -1.f }, { 0.f, -1.f, 0.f }, { 0.f, 1.f } },
    { { 1.f, -1.f, -1.f }, { 0.f, -1.f, 0.f }, { 1.f, 1.f } },
    { { 1.f, -1.f, 1.f }, { 0.f, -1.f, 0.f }, { 1.f, 0.f } },

    /* +Z */
    { { -1.f, 1.f, 1.f }, { 0.f, 0.f, 1.f }, { 0.f, 0.f } },
    { { -1.f, -1.f, 1.f }, { 0.f, 0.f, 1.f }, { 0.f, 1.f } },
    { { 1.f, -1.f, 1.f }, { 0.f, 0.f, 1.f }, { 1.f, 1.f } },
    { { 1.f, 1.f, 1.f }, { 0.f, 0.f, 1.f }, { 1.f, 0.f } },

    /* -Z */
    { { 1.f, 1.f, -1.f }, { 0.f, 0.f, -1.f }, { 0.f, 0.f } },
    { { 1.f, -1.f, -1.f }, { 0.f, 0.f, -1.f }, { 0.f, 1.f } },
    { { -1.f, -1.f, -1.f }, { 0.f, 0.f, -1.f }, { 1.f, 1.f } },
    { { -1.f, 1.f, -1.f }, { 0.f, 0.f, -1.f }, { 1.f, 0.f } },
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

    //Set up buffer objects
    glGenBuffers(1, &vbo_verts);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_verts);
    glBufferData(GL_ARRAY_BUFFER, 24 * sizeof(vert), verts, GL_STATIC_DRAW);

    glGenBuffers(1, &vbo_inds);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_inds);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 36 * sizeof(GLushort), indices, GL_STATIC_DRAW);

#if (CUBEMAP)
    //Load Textures
    glEnable(GL_TEXTURE_CUBE_MAP);
    glGenTextures(1, &texMap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texMap);

    loadTex(right,  GL_TEXTURE_CUBE_MAP_POSITIVE_X);
    loadTex(left,   GL_TEXTURE_CUBE_MAP_NEGATIVE_X);
    loadTex(top,    GL_TEXTURE_CUBE_MAP_POSITIVE_Y);
    loadTex(bottom, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y);
    loadTex(back,   GL_TEXTURE_CUBE_MAP_POSITIVE_Z);
    loadTex(front,  GL_TEXTURE_CUBE_MAP_NEGATIVE_Z);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    //Load shaders
    skyShaders = init_program("Shaders\\skymap_vshader.glsl", "Shaders\\skymap_fshader.glsl");
    glUseProgram(skyShaders);
#else
    //Load Textures
    glEnable(GL_TEXTURE_2D);
    skybox[0] = loadTexture(right);
    skybox[1] = loadTexture(left);
    skybox[2] = loadTexture(top);
    skybox[3] = loadTexture(bottom);
    skybox[4] = loadTexture(back);
    skybox[5] = loadTexture(front);

    //Load shaders
    skyShaders = init_program("Shaders\\sky_vshader.glsl", "Shaders\\sky_fshader.glsl");
    glUseProgram(skyShaders);

    //Get attribute and uniform locations
    tloc = glGetAttribLocation(skyShaders, "vTexCoord");
#endif

    vloc = glGetAttribLocation(skyShaders, "vPosition");
    modelLoc = glGetUniformLocation(skyShaders, "Model");
    projectionLoc = glGetUniformLocation(skyShaders, "Projection");
    viewLoc = glGetUniformLocation(skyShaders, "View");

    glClearColor(0.53, 0.81, 0.98, 1.0);
}

SkyBox::~SkyBox()
{
}

unsigned int SkyBox::loadTexture(char* filename)
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
    GLenum texFormat = f == 4 ? GL_RGBA : GL_RGB;

    //Bind the texture
    glBindTexture(GL_TEXTURE_2D, id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexImage2D(GL_TEXTURE_2D, 0, f, img->w, img->h, 0, texFormat, GL_UNSIGNED_BYTE, img->pixels);
    SDL_FreeSurface(img);

    return id;
}

void SkyBox::loadTex(char* filename, GLenum face)
{
    //load SDL surface
    SDL_Surface *img;
    img = IMG_Load(filename);

    if (!img)
    {
        printf("IMG_Load: %s\n", IMG_GetError());
    }
    //Get properties
    GLint f = img->format->BytesPerPixel;
    GLenum texFormat = f == 4 ? GL_RGBA : GL_RGB;

    glTexImage2D(face, 0, f, img->w, img->h, 0, texFormat, GL_UNSIGNED_BYTE, img->pixels);
}

void SkyBox::draw(mat4 view, vec3 cam_position)
{
    glUseProgram(skyShaders);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_verts);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_inds);

    //Disable writes to depth buffer
    glDisable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);

    mat4 projection = perspective(45.0, 1.0, 0.1, 100.0);
    mat4 model = translation(cam_position) * xrotation(0.0) * yrotation(0.0) * scale(vec3(50, 50, 50));
    glUniformMatrix4fv(projectionLoc, 1, GL_TRUE, projection);
    glUniformMatrix4fv(modelLoc, 1, GL_TRUE, model);
    glUniformMatrix4fv(viewLoc, 1, GL_TRUE, view);

    const size_t sz = sizeof (GLfloat);
    glEnableVertexAttribArray(vloc);
    glVertexAttribPointer(vloc, 3, GL_FLOAT, GL_FALSE, sz * 8, 0);

#if (CUBEMAP)
    glBindTexture(GL_TEXTURE_CUBE_MAP, texMap);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, 0);
#else
    glEnableVertexAttribArray(tloc);
    glVertexAttribPointer(tloc, 2, GL_FLOAT, GL_FALSE, sz * 8, (void*)(sz * 6));

    for (int i = 0; i < 6; ++i)
    {
        glBindTexture(GL_TEXTURE_2D, skybox[i]);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, (void*)(i*sizeof(GLushort)* 6));
    }

    //Clear texture binding
    glBindTexture(GL_TEXTURE_2D, 0);
#endif

    //Re-enable depth buffer
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
}

GLuint SkyBox::getTexMap()
{
    return texMap;
}