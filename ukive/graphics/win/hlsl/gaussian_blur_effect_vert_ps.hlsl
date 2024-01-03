#include "gaussian_blur.hlsli"


Texture2D img_ : register(t0);

struct PixelInputType {
    float4 position : SV_POSITION;
    float3 raw_position : POSITION;
};

float4 main(PixelInputType input) : SV_TARGET {
    int rx = (int)input.raw_position.x;
    int ry = (int)input.raw_position.y;
    return gaussian_blur_13v(img_, rx, ry);
}