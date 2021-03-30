cbuffer mData
{
    float4x4 gWVP;
};

struct VertexInputType
{
    float3 position : POSITION;
    float4 color : COLOR;
    float2 tex : TEXCOORD0;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
    float2 tex : TEXCOORD0;
};


PixelInputType main(VertexInputType input)
{
    PixelInputType output;

    output.position = mul(float4(input.position, 1.0f), gWVP);
    output.color = input.color;
    output.tex = input.tex;

    return output;
}