#include "Ocean.h"
#include "stdio.h"

#define TILE 0
#define TEXTURE 0

#pragma region Setup
Ocean::Ocean(int gridX, int gridZ, int patchLength, float period, float phillipA, float suppressor, vec3 windDir) 
: N(gridX), M(gridZ), L(patchLength), T(period), PhA(phillipA), Suppressor(suppressor), 
WindDir(windDir), Time(10), numIndices(0), g(9.81), lambda(1.5), WindSpeed(32), nAdjust(N/2), mAdjust(M/2)
{
    printf("GL %s, GLSL %s\n", glGetString(GL_VERSION), glGetString(GL_SHADING_LANGUAGE_VERSION));

#if (TILE)
    NS = N + 1;
    MS = M + 1;
    vertices = new vertex[(N + 1) * (M + 1)];
    indices = new unsigned int[(N+1) * (M+1) * 6];
#else
    NS = N;
    MS = M;
    vertices = new vertex[N*M];
    indices = new unsigned int[N * M * 6];
#endif

    waveVecAdjust = (float) 2 * M_PI / L;
    AmplitudeNought = new complex<float>[(N + 1) * (M + 1)];
    Amplitudes = new complex<float>[N*M];
    SlopeX = new complex<float>[N*M];
    SlopeZ = new complex<float>[N*M];
    DispX = new complex<float>[N*M];
    DispZ = new complex<float>[N*M];

    InitVertices();
    InitIndices();
    InitAmplitudeNought();
    CheckVerts();
    GenerateCoefficients();
    GenerateVertices();
    SetupRender();
}
Ocean::~Ocean()
{
}

void Ocean::InitVertices()
{
    float xAdjust = (float) L / N;
    float zAdjust = (float) L / M;

    for (int i = 0; i < NS; ++i)
    {
        for (int j = 0; j < MS; ++j)
        {
            int arrIndex = i * MS + j;
            vertices[arrIndex].originalPosition[0] = vertices[arrIndex].displacedPosition[0] = (i - nAdjust) * xAdjust;
            vertices[arrIndex].originalPosition[1] = vertices[arrIndex].displacedPosition[1] = 0;
            vertices[arrIndex].originalPosition[2] = vertices[arrIndex].displacedPosition[2] = (j - mAdjust) * zAdjust;
            vertices[arrIndex].texCoord[0] = (float) i / NS;
            vertices[arrIndex].texCoord[1] = (float) j / MS;
        }
    }

}
void Ocean::InitIndices()
{
    for (int i = 0; i < (NS - 1); ++i)
    {
        for (int j = 0; j < (MS - 1); ++j)
        {
            int arrIndex = i * MS + j;
            indices[numIndices++] = arrIndex;
            indices[numIndices++] = arrIndex + MS;
            indices[numIndices++] = arrIndex + (MS + 1);
            indices[numIndices++] = arrIndex;
            indices[numIndices++] = arrIndex + (MS + 1);
            indices[numIndices++] = arrIndex + 1;
        }
    }
}
void Ocean::InitAmplitudeNought()
{
    for (int i = 0; i <= N; ++i)
    {
        for (int j = 0; j <= M; ++j)
        {
            int arrIndex = i * (M + 1) + j;
            vec3 waveVec = vec3((i - nAdjust) * waveVecAdjust, 0.0, (j - mAdjust) * waveVecAdjust);
            AmplitudeNought[arrIndex] = FourierAmplitudeNought(waveVec);
        }
    }
}
#pragma endregion

#pragma region Height Field Helper Functions
float Ocean::Dispersion(vec3 waveVec)
{
    static float baseFrequency = (2 * M_PI) / T;
    float waveNumber = length(waveVec);
    float dispersion = floor(sqrt(g * waveNumber) / baseFrequency) * baseFrequency;

    return dispersion;
}
float Ocean::PhillipSpectrum(vec3 waveVec)
{
    float waveNumber = length(waveVec);
    if (waveNumber < Suppressor) return 0.0;

    float v = WindSpeed;
    float maxWave = (float) pow(v, 2) / g;

    float exponential = exp((float) -1 / pow((waveNumber * maxWave), 2)) / pow(waveNumber, 4);
    float waveDotWind = pow(normalize(waveVec) * normalize(WindDir), 2);
    float suppression = exp(-pow(waveNumber, 2) * pow(Suppressor, 2));

    float PhillipValue = PhA * exponential * waveDotWind * suppression;
    return PhillipValue;
}
complex<float> Ocean::FourierAmplitudeNought(vec3 waveVec)
{
    std::default_random_engine generator;
    std::normal_distribution<float> gaussian(0.0, 1.0);
    complex<float> randomDraw(gaussian(generator), gaussian(generator));

    complex<float> ampNought = randomDraw * sqrt((float) PhillipSpectrum(waveVec) / 2);
    return ampNought;
}
complex<float> Ocean::FourierAmplitude(int n, int m, vec3 waveVec)
{
    complex<float> amp0 = AmplitudeNought[n * (M + 1) + m];
    complex<float> amp0Conj = std::conj(AmplitudeNought[(N - n) * (M + 1) + (M - m)]);

    float timeDispersion = Dispersion(waveVec) * Time;
    float dispReal = cos(timeDispersion);
    float dispImag = sin(timeDispersion);

    complex<float> e0(dispReal, dispImag);
    complex<float> e1(dispReal, -dispImag);

    complex<float> amp = (amp0 * e0) + (amp0Conj * e1);

    return amp;
}
#pragma endregion

#pragma region Vertex Generation
void Ocean::GenerateCoefficients()
{
    for (int i = 0; i < N; ++i)
    {
        for (int j = 0; j < M; ++j)
        {
            int arrIndex = i * M + j;
            vec3 waveVec = vec3((i - nAdjust) * waveVecAdjust, 0.0, (j - mAdjust) * waveVecAdjust);
            float waveNumber = length(waveVec);

            Amplitudes[arrIndex] = FourierAmplitude(i, j, waveVec);
            SlopeX[arrIndex] = Amplitudes[arrIndex] * complex<float>(0, waveVec[0]);
            SlopeZ[arrIndex] = Amplitudes[arrIndex] * complex<float>(0, waveVec[2]);
            if (waveNumber < Suppressor)
            {
                DispX[arrIndex] = DispZ[arrIndex] = complex<float>(0.0, 0.0);
            }
            else
            {
                DispX[arrIndex] = Amplitudes[arrIndex] * complex<float>(0, ((float) waveVec[0] / waveNumber));
                DispZ[arrIndex] = Amplitudes[arrIndex] * complex<float>(0, ((float) waveVec[2] / waveNumber));
            }
        }
    }
}
void Ocean::ComputeIFFT(complex<float>* coefficients, int dimX, int dimZ)
{
    fftwf_plan transform = fftwf_plan_dft_2d(dimX, dimZ, reinterpret_cast<fftwf_complex*>(coefficients),
        reinterpret_cast<fftwf_complex*>(coefficients), FFTW_BACKWARD, FFTW_ESTIMATE);
    fftwf_execute(transform);
    fftwf_destroy_plan(transform);
    reinterpret_cast<complex<float>*>(coefficients);

}
void Ocean::Tile(int arrIndex, int vertIndex, int tileIndex)
{
    vec3  tileOriginal = vertices[tileIndex].originalPosition;
    vertices[tileIndex].displacedPosition[0] = tileOriginal[0] + lambda * std::abs(DispX[arrIndex]);
    vertices[tileIndex].displacedPosition[1] = vertices[vertIndex].displacedPosition[1];
    vertices[tileIndex].displacedPosition[2] = tileOriginal[2] + lambda * std::abs(DispZ[arrIndex]);
    vertices[tileIndex].normal = vertices[vertIndex].normal;
}
void Ocean::GenerateVertices()
{
    GenerateCoefficients();

    ComputeIFFT(Amplitudes, N, M);
    ComputeIFFT(SlopeX, N, M);
    ComputeIFFT(SlopeZ, N, M);
    ComputeIFFT(DispX, N, M);
    ComputeIFFT(DispZ, N, M);

    const float signs[2] = { 1.0, -1.0 };
    for (int i = 0; i < N; ++i)
    {
        for (int j = 0; j < M; ++j)
        {
            int arrIndex = i * M + j;
            int vertIndex = i * (M + 1) + j;
            int sign = signs[(i + j) & 1];

            //Sign adjustments - needed for compatibility with fftw
            Amplitudes[arrIndex] *= sign;
            DispX[arrIndex] *= sign;
            DispZ[arrIndex] *= sign;
            SlopeX[arrIndex] *= sign;
            SlopeZ[arrIndex] *= sign;

#if (TILE)
            //Calculate final position
            vec3 original = vertices[vertIndex].originalPosition;
            vertices[vertIndex].displacedPosition[0] = original[0] + lambda * std::abs(DispX[arrIndex]);
            vertices[vertIndex].displacedPosition[1] = std::abs(Amplitudes[arrIndex]);
            vertices[vertIndex].displacedPosition[2] = original[2] + lambda * std::abs(DispZ[arrIndex]);

            //Calculate normals
            vec3 slopeVec = vec3(std::abs(SlopeX[arrIndex]), 1.0, std::abs(SlopeZ[arrIndex]));
            //vertices[vertIndex].normal = (vec3(0.0, 1.0, 0.0) - slopeVec);
            vertices[vertIndex].normal = (vec3(0.0, 1.0, 0.0) - slopeVec) / sqrt(1 + slopeVec*slopeVec);

            //tile at corner
            if (i == 0 && j == 0)
            {
                int tileIndex = (N+1) * (M+1) - 1;
                Tile(arrIndex, vertIndex, tileIndex);
            }
            //tile on M side
            if (j == 0)
            {
                int tileIndex = vertIndex + M;
                Tile(arrIndex, vertIndex, tileIndex);
            }
            //tile on N side
            if (i == 0)
            {
                int tileIndex = vertIndex + N * (M + 1);
                Tile(arrIndex, vertIndex, tileIndex);
            }
#else
            //Calculate final position
            vec3 original = vertices[arrIndex].originalPosition;
            vertices[arrIndex].displacedPosition[0] = original[0] + lambda * std::abs(DispX[arrIndex]);
            vertices[arrIndex].displacedPosition[1] = std::abs(Amplitudes[arrIndex]);
            vertices[arrIndex].displacedPosition[2] = original[2] + lambda * std::abs(DispZ[arrIndex]);

            //Calculate normals
            vec3 slopeVec = vec3(std::abs(SlopeX[arrIndex]), 1.0, std::abs(SlopeZ[arrIndex]));
            //vertices[arrIndex].normal = (vec3(0.0, 1.0, 0.0) - slopeVec);
            vertices[arrIndex].normal = (vec3(0.0, 1.0, 0.0) - slopeVec) / sqrt(1 + slopeVec*slopeVec);
#endif
        }
    }
}
#pragma endregion

#pragma region Rendering
void Ocean::SetupRender()
{
    //Create vertex buffer objects
    glGenBuffers(1, &vbo_vertices);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices);
    glBufferData(GL_ARRAY_BUFFER, NS*MS*sizeof(vertex), vertices, GL_DYNAMIC_DRAW);

    glGenBuffers(1, &vbo_indices);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_indices);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, numIndices * sizeof(unsigned int), indices, GL_DYNAMIC_DRAW);

#if (TEXTURE)
    //Load texture
    glEnable(GL_TEXTURE_2D);
    tbo = loadTexture("Textures\\ocean\\Ocean_water_surface_512.jpg");
    //Load shaders
    program = init_program("Shaders\\water_vshader.glsl", "Shaders\\water_fshader.glsl");
    tloc = glGetUniformLocation(program, "vTexCoord");
#else
    program = init_program("Shaders\\vshader.glsl", "Shaders\\fshader.glsl");
    cloc = glGetUniformLocation(program, "Color");
    nloc = glGetAttribLocation(program, "vNormal");
#endif

    glUseProgram(program);
    //Get attribute and uniform locations
    vloc = glGetAttribLocation(program, "vPosition");
    modelLoc = glGetUniformLocation(program, "Model");
    projectionLoc = glGetUniformLocation(program, "Projection");
    viewLoc = glGetUniformLocation(program, "View");

#if (!TEXTURE && TILE)
    glUniform4fv(glGetUniformLocation(program, "LightPosition"), 1, vec4(3 * L / 4, 40.0, -3 * L / 4, 0.0));
#else if(!TEXTURE && !TILE)
    glUniform4fv(glGetUniformLocation(program, "LightPosition"), 1, vec4(-L/2, 40.0, L/2, 0.0));
#endif

    glClearColor(0.53,0.81,0.98,1.0);
}
unsigned int Ocean::loadTexture(char* filename)
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

    glTexImage2D(GL_TEXTURE_2D, 0, f, img->w, img->h, 0, texFormat, GL_UNSIGNED_BYTE, img->pixels);
    SDL_FreeSurface(img);

    return id;
}
void Ocean::draw(mat4 viewMatrix)
{
    glEnable(GL_DEPTH_TEST);
    glUseProgram(program);

#if (TEXTURE)
    glBindTexture(GL_TEXTURE_2D, tbo);
#else
    glUniform4fv(cloc, 1, vec4(0.18, 0.55, 0.34, 0.0));
#endif

    mat4 modelMatrix = translation(vec3(0.0, -15.0, -60.0)) * xrotation(0.0) * yrotation(0.0);
    mat4 projectionMatrix = perspective(45.0, 1.0, 0.1, 100.0);

    glUniformMatrix4fv(modelLoc, 1, GL_TRUE, modelMatrix);
    glUniformMatrix4fv(viewLoc, 1, GL_TRUE, viewMatrix);
    glUniformMatrix4fv(projectionLoc, 1, GL_TRUE, projectionMatrix);

    glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices);
    glBufferSubData(GL_ARRAY_BUFFER, 0, NS*MS*sizeof(vertex), vertices);
    glEnableVertexAttribArray(vloc);
    glVertexAttribPointer(vloc, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (const GLfloat*)(sizeof(GLfloat)* 3));

#if (TEXTURE)
    glEnableVertexAttribArray(tloc);
    glVertexAttribPointer(tloc, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), (const GLfloat*)(sizeof(GLfloat)* 9));
#else
    glEnableVertexAttribArray(nloc);
    glVertexAttribPointer(nloc, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (const GLfloat *)(sizeof(GLfloat)* 6));
#endif

#if (TILE)
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_indices);
    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j)
        {
            modelMatrix = translation(vec3(L*i, 0.0, L*-j));
            glUniformMatrix4fv(modelLoc, 1, GL_TRUE, modelMatrix);
            glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, 0);
        }
#else
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_indices);
    glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, 0);
#endif

    //Clear texture binding
    //glBindTexture(GL_TEXTURE_2D, 0);
}
void Ocean::step()
{
    Time += (1/T);
    GenerateVertices();
}
#pragma endregion

#pragma region Testing
void Ocean::CheckVerts()
{
    FILE *uv = fopen("uv.txt", "w");
    FILE *verts = fopen("vertices.txt", "w");
    FILE *ind = fopen("indices.txt", "w");
    fprintf(verts, "%d\n", numIndices);

    for (int i = 0; i < numIndices; ++i)
    {
        int j = indices[i];
        fprintf(ind, "%d: %d\n", i, j);
        fprintf(verts, "%d: %f %f %f\n", j, vertices[j].displacedPosition[0],
            vertices[j].displacedPosition[1], vertices[j].displacedPosition[2]);
        if ((i + 1) % 3 == 0) fprintf(verts, "\n");
    }

    for (int i = 0; i < NS * MS; ++i)
    {
        fprintf(uv, "%d: %f %f\n", i, vertices[i].texCoord[0], vertices[i].texCoord[1]);
    }
}
#pragma endregion