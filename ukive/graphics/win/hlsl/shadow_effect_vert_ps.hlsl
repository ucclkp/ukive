// 经由 hori 像素着色器处理后的中间结果
Texture2D st_mid_ : register(t0);

// 原始纹理，用于屏蔽不透明的部分
Texture2D st_init_ : register(t1);

// 权重
Texture2D<float> kernel_texture_ : register(t2);


struct PixelInputType {
    float4 position : SV_POSITION;
    float3 raw_position : POSITION;
};


float4 main(PixelInputType input) : SV_TARGET {
    int width = 0, height = 0;
    kernel_texture_.GetDimensions(width, height);

    int sw = 0, sh = 0;
    st_init_.GetDimensions(sw, sh);

    int radius = width - 1;
    int diameter = width * 2 - 1;

    int rx = (int)floor(input.raw_position.x);
    int ry = sh + radius * 2 - (int)ceil(input.raw_position.y);

    float4 init_color = st_init_.Load(int3(rx - radius, ry - radius, 0));

    float acc_alpha = 0;
    for (int i = 0; i < diameter; ++i) {
        int x = rx;
        int y = ry + i - radius;
        float4 color = st_mid_.Load(int3(x, y, 0));

        int index_x = min(i, radius) * 2 - i;
        float weight = kernel_texture_.Load(int3(index_x, 0, 0));
        acc_alpha += color.w * weight;
    }

    float4 dst_color = float4(0, 0, 0, acc_alpha * 0.6f);
    return init_color + dst_color * (1 - init_color.w);
}