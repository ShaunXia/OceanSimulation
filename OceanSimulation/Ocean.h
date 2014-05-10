#define _USE_MATH_DEFINES

#include <complex>
#include <random>
#include <cmath>
#include "GLFundamentals.hpp"
#include "GLDemonstration.hpp"
#include "fftw3.h"
#include "SkyBox.h"

#pragma once

using std::complex;
using namespace gl;

class Ocean
{
private:

#pragma region Vertex Data
    struct vertex {
        vec3 originalPosition;
        vec3 displacedPosition;
        vec3 normal;
    };
    vertex* vertices;
    unsigned int* indices;
    int numIndices;

    complex<float>* Amplitudes;
    complex<float>* AmplitudeNought;
    complex<float>* AmplitudeNoughtConjugate;
    complex<float>* SlopeX;
    complex<float>* SlopeZ;
    complex<float>* DispX;
    complex<float>* DispZ;
#pragma endregion

#pragma region Parameter Variables
    float g;                  //gravitational constant
    float lambda;
    int N, M;                 //grid dimensions
    int NS, MS;
    int nAdjust, mAdjust;     //Tessendorf model bounds: -N/2 <= n < N/2
                              //                         -M/2 <= m < M/2
    float waveVecAdjust; 
    int L;                    //patch dimensions L = Lx = Lz
    float Time;
    float T;                  //period
    float PhA;                //numeric constant, affects wave height
    float Suppressor;         //Suppression factor for Phillip Spectrum
    vec3 WindDir;             //wind direction
    float WindSpeed;
#pragma endregion

#pragma region Render Variables
    GLuint program;
    GLuint skyMap;
    GLuint vbo_vertices, vbo_indices;
    GLuint vloc, nloc, cloc, camLoc;
    GLuint modelLoc, projectionLoc, viewLoc;
#pragma endregion

#pragma region Helper Methods
    float Dispersion(vec3 waveVec);
    float PhillipSpectrum(vec3 waveVec);
    complex<float> FourierAmplitudeNought(vec3 waveVec);
    complex<float> FourierAmplitude(int n, int m, vec3 waveVec);
#pragma endregion

#pragma region Vertex Calculation Methods
    void InitVertices();
    void InitIndices();
    void InitAmplitudeNought();
    void GenerateCoefficients();
    void ComputeIFFT(complex<float>*, int, int);
    void Tile(int, int, int);
    void GenerateVertices();
#pragma endregion

#pragma region Render Setup
    void SetupRender();
#pragma endregion

#pragma region Testing Variables and Methods
    void CheckVerts();
#pragma endregion

public:
    Ocean(int gridX, int gridZ, int patchLength, float period, float phillipA, float suppressor, vec3 windDir);
    ~Ocean();

    void draw(mat4 viewMatrix, vec3 cam_position);
    void setSkyMap(GLuint texMap);
    void step();
};

