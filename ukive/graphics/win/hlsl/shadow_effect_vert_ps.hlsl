Texture2D img_ : register(t0);
Texture2D<float> kernel_ : register(t1);

struct PixelInputType {
    float4 position : SV_POSITION;
    float3 raw_position : POSITION;
};

float4 main(PixelInputType input) : SV_TARGET {
    uint kw = 0, kh = 0;
    kernel_.GetDimensions(kw, kh);
    
    int radius = kw - 1;
    int x = (int)input.raw_position.x;
    int y = (int)input.raw_position.y; 

    float4 color = img_.Load(int3(x, y, 0)) * kernel_.Load(int3(radius, 0, 0));
    for (int i = 0; i < radius; ++i)
    {
        float w = kernel_.Load(int3(i, 0, 0));
        color += img_.Load(int3(x, y - (radius - i), 0)) * w;
        color += img_.Load(int3(x, y + (radius - i), 0)) * w;
    }

    color.rgb = 0;
    color.a *= 0.6f;
    return color;
}