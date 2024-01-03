cbuffer cbData {
    float4x4 wvo;
};

struct VertexInputType {
    float3 position : POSITION;
};

struct PixelInputType {
    float4 position : SV_POSITION;
};

PixelInputType main(VertexInputType input) {
    PixelInputType output;
    output.position = mul(float4(input.position, 1.0f), wvo);
    return output;
}