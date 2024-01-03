Texture2D img_ : register(t0);
Texture2D bg_img_ : register(t1);
Texture2D<float> kernel_ : register(t2);

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
    bg_img_.GetDimensions(sw, sh);
    
    int radius = kw - 1;
    int x = (int)(input.position.x);
    int y = (int)(input.position.y);
    
    float4 init_color = bg_img_.Load(int3(x - (rt_size.x - sw) / 2, y - (rt_size.y - sh) / 2, 0));

    float4 color = img_.Load(int3(x, y, 0)) * kernel_.Load(int3(radius, 0, 0));
    for (int i = 0; i < radius; ++i)
    {
        float w = kernel_.Load(int3(i, 0, 0));
        color += img_.Load(int3(x, y - (radius - i), 0)) * w;
        color += img_.Load(int3(x, y + (radius - i), 0)) * w;
    }

    color.rgb = 0;
    color.a *= 0.6f;
    return init_color + color * (1 - init_color.w);
}