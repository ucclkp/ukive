Texture2D st_init_ : register(t0);
Texture2D<uint> kernel_texture_ : register(t1);


struct PixelInputType {
    float4 position : SV_POSITION;
    float3 raw_position : POSITION;
};


float4 main(PixelInputType input) : SV_TARGET {
    int width = 0, height = 0;
    kernel_texture_.GetDimensions(width, height);

    int sw = 0, sh = 0;
    st_init_.GetDimensions(sw, sh);

    int rx = (int)floor(input.raw_position.x);
    int ry = (int)floor(input.raw_position.y);

    float4 init_color = st_init_.Load(int3(rx, ry, 0));

    return init_color;
}