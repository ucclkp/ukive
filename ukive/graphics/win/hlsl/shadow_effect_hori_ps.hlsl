Texture2D img_ : register(t0);
Texture2D<float> kernel_ : register(t1);

cbuffer cbData {
    uint2 rt_size;
    uint2 unused;
};

struct PixelInputType {
    float4 position : SV_POSITION;
};

float4 main(PixelInputType input) : SV_TARGET {
    uint kw = 0, kh = 0;
    kernel_.GetDimensions(kw, kh);
    
    uint sw = 0, sh = 0;
    img_.GetDimensions(sw, sh);
    
    int radius = kw - 1;
    int x = (int)(input.position.x) - (rt_size.x - sw) / 2;
    int y = (int)(input.position.y) - (rt_size.y - sh) / 2;

    float4 color = img_.Load(int3(x, y, 0)) * kernel_.Load(int3(radius, 0, 0));
    for (int i = 0; i < radius; ++i)
    {
        float w = kernel_.Load(int3(i, 0, 0));
        color += img_.Load(int3(x - (radius - i), y, 0)) * w;
        color += img_.Load(int3(x + (radius - i), y, 0)) * w;
    }
    return color;
}