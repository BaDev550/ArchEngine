#pragma once
// Forked from: https://github.com/emeiri/ogldev/blob/master/Common/ogldev_ect_cubemap.cpp
#include <iostream>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include "Bitmap.h"
#include "Math.h"

namespace ae {
#define CUBE_MAP_INDEX_POS_X 0
#define CUBE_MAP_INDEX_NEG_X 1
#define CUBE_MAP_INDEX_POS_Y 2
#define CUBE_MAP_INDEX_NEG_Y 3
#define CUBE_MAP_INDEX_POS_Z 4
#define CUBE_MAP_INDEX_NEG_Z 5
#define CUBEMAP_NUM_FACES 6
    static int types[CUBEMAP_NUM_FACES] = { CUBE_MAP_INDEX_POS_X,
                            CUBE_MAP_INDEX_NEG_X,
                            CUBE_MAP_INDEX_POS_Y,
                            CUBE_MAP_INDEX_NEG_Y,
                            CUBE_MAP_INDEX_POS_Z,
                            CUBE_MAP_INDEX_NEG_Z };

    static glm::vec3 FaceCoordsToXYZ(int x, int y, int FaceID, int FaceSize)
    {
        float A = 2.0f * float(x) / FaceSize;
        float B = 2.0f * float(y) / FaceSize;

        switch (FaceID) {
        case CUBE_MAP_INDEX_POS_X: return glm::vec3(A - 1.0f, 1.0f, 1.0f - B);
        case CUBE_MAP_INDEX_NEG_X: return glm::vec3(1.0f - A, -1.0f, 1.0f - B);
        case CUBE_MAP_INDEX_POS_Y: return glm::vec3(1.0f - B, A - 1.0f, 1.0f);
        case CUBE_MAP_INDEX_NEG_Y: return glm::vec3(B - 1.0f, A - 1.0f, -1.0f);
        case CUBE_MAP_INDEX_POS_Z: return glm::vec3(-1.0f, A - 1.0f, 1.0f - B);
        case CUBE_MAP_INDEX_NEG_Z: return glm::vec3(1.0f, 1.0f - A, 1.0f - B);
        default:
            assert(0);
        }
    }


    static int ConvertEquirectangularImageToCubemap(const Bitmap& b, std::vector<Bitmap>& Cubemap)
    {
        int FaceSize = b._w / 4;

        Cubemap.resize(CUBEMAP_NUM_FACES);
        for (int i = 0; i < CUBEMAP_NUM_FACES; i++) {
            Cubemap[i].Init(FaceSize, FaceSize, b._comp, b.fmt_);
        }

        int MaxW = b._w - 1;
        int MaxH = b._h - 1;

        for (int face = 0; face < CUBEMAP_NUM_FACES; face++) {
            for (int y = 0; y < FaceSize; y++) {
                for (int x = 0; x < FaceSize; x++) {
                    glm::vec3 P = FaceCoordsToXYZ(x, y, face, FaceSize);
                    float R = sqrtf(P.x * P.x + P.y * P.y);
                    float phi = atan2f(P.y, P.x);
                    float theta = atan2f(P.z, R);

                    float u = (float)((phi + math::PI) / (2.0f * math::PI));
                    float v = (float((math::PI / 2.0f - theta) / math::PI));

                    float U = u * b._w;
                    float V = v * b._h;
                    int U1 = glm::clamp(int(floor(U)), 0, MaxW);
                    int V1 = glm::clamp(int(floor(V)), 0, MaxH);
                    int U2 = glm::clamp(U1 + 1, 0, MaxW);
                    int V2 = glm::clamp(V1 + 1, 0, MaxH);

                    float s = U - U1;
                    float t = V - V1;

                    glm::vec4 BottomLeft = b.getPixel(U1, V1);
                    glm::vec4 BottomRight = b.getPixel(U2, V1);
                    glm::vec4 TopLeft = b.getPixel(U1, V2);
                    glm::vec4 TopRight = b.getPixel(U2, V2);
                    glm::vec4 color = BottomLeft * (1 - s) * (1 - t) +
                        BottomRight * (s) * (1 - t) +
                        TopLeft * (1 - s) * t +
                        TopRight * (s) * (t);

                    Cubemap[face].setPixel(x, y, color);
                }
            }
        }
        return FaceSize;
    }
}