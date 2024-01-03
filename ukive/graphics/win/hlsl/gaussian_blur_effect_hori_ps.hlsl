#include "gaussian_blur.hlsli"


cbuffer cbData {
    uint2 rt_size;
    uint2 unused;
};

Texture2D img_ : register(t0);

struct PixelInputType {
    float4 position : SV_POSITION;
    float3 raw_position : POSITION;
};

float4 main(PixelInputType input) : SV_TARGET {
    // 渲染目标的坐标位置（未进行坐标变换），
    // 其位置遍历传入的顶点坐标围成的范围。该坐标为半像素坐标。
    int rx = (int)input.raw_position.x;
    int ry = (int)input.raw_position.y;
    
    uint sw = 0, sh = 0;
    img_.GetDimensions(sw, sh);
    int space_w = (rt_size.x - sw) / 2;
    int space_h = (rt_size.y - sh) / 2;
    
    return gaussian_blur_13h(img_, rx - space_w, ry - space_h);
}