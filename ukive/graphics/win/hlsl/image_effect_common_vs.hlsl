cbuffer cbData {
    float4x4 wvo;
};

struct VertexInputType {
    float3 position : POSITION;
    float2 tex : TEXCOORD0;
};

struct PixelInputType {
    float4 position : SV_POSITION;
    float3 raw_position : POSITION;
    float2 tex : TEXCOORD0;
};


PixelInputType main(VertexInputType input) {
    PixelInputType output;

    output.position = mul(float4(input.position, 1.0f), wvo);
    output.raw_position = input.position;
    output.tex = input.tex;

    return output;
}