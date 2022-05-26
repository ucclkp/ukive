Texture2D shader_texture_ : register(t0);
Texture2D<float> kernel_texture_ : register(t1);


struct PixelInputType {
    float4 position : SV_POSITION;
    float3 raw_position : POSITION;
};


float4 main(PixelInputType input) : SV_TARGET {
    int width = 0, height = 0;
    kernel_texture_.GetDimensions(width, height);

    int sw = 0, sh = 0;
    shader_texture_.GetDimensions(sw, sh);

    int radius = width - 1;
    int diameter = width * 2 - 1;

    // 渲染目标的坐标位置（未进行坐标变换），
    // 其位置遍历传入的顶点坐标围成的范围。该坐标为半像素坐标。
    int rx = (int)floor(input.raw_position.x);
    int ry = sh + radius * 2 - (int)ceil(input.raw_position.y);

    float acc_alpha = 0;
    for (int i = 0; i < diameter; ++i) {
        int x = rx + i - 2 * radius;
        int y = ry - radius;
        float4 color = shader_texture_.Load(int3(x, y, 0));

        int index_x = min(i, radius) * 2 - i;
        float weight = kernel_texture_.Load(int3(index_x, 0, 0));
        acc_alpha += color.w * weight;
    }

    return float4(0, 0, 0, acc_alpha * 0.6f);
}