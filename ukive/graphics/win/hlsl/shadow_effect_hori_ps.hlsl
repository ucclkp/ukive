Texture2D img_ : register(t0);
Texture2D<float> kernel_ : register(t1);

cbuffer cbData {
    uint2 rt_size;
    uint2 unused;
};

struct PixelInputType {
    float4 position : SV_POSITION;
    // 渲染目标的坐标位置（未进行坐标变换），
    // 其位置遍历传入的顶点坐标围成的范围。该坐标为半像素坐标。
    float3 raw_position : POSITION;
};

float4 main(PixelInputType input) : SV_TARGET {
    uint kw = 0, kh = 0;
    kernel_.GetDimensions(kw, kh);
    
    uint sw = 0, sh = 0;
    img_.GetDimensions(sw, sh);
    
    int radius = kw - 1;
    int x = (int)input.raw_position.x - (rt_size.x - sw) / 2;
    int y = (int)input.raw_position.y - (rt_size.y - sh) / 2;

    float4 color = img_.Load(int3(x, y, 0)) * kernel_.Load(int3(radius, 0, 0));
    for (int i = 0; i < radius; ++i)
    {
        float w = kernel_.Load(int3(i, 0, 0));
        color += img_.Load(int3(x - (radius - i), y, 0)) * w;
        color += img_.Load(int3(x + (radius - i), y, 0)) * w;
    }
    return color;
}