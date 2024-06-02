struct PixelInputType {
    float4 position : SV_POSITION;
    float3 raw_position : POSITION;
};

Texture2D img_ : register(t0);

float4 main(PixelInputType input) : SV_TARGET{
    int rx = (int)input.raw_position.x;
    int ry = (int)input.raw_position.y;

    float4 color = img_.Load(int3(rx, ry, 0));
    return color;
}