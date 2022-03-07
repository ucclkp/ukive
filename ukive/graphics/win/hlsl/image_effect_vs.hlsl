cbuffer cbData {
    float4x4 wvo;
};

struct VertexInputType {
    float3 position : POSITION;
};

struct PixelInputType {
    float4 position : SV_POSITION;
    float3 raw_position : POSITION;
};


PixelInputType main(VertexInputType input) {
    PixelInputType output;

    output.position = mul(float4(input.position, 1.0f), wvo);
    output.raw_position = input.position;

    return output;
}