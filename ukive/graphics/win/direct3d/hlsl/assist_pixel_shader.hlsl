Texture2D mShaderTexture;
SamplerState mSampleType;

struct PixelInputType
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
    float2 tex : TEXCOORD0;
};


float4 main(PixelInputType input) : SV_TARGET
{
    float4 texColor = mShaderTexture.Sample(mSampleType, input.tex);
    return input.color;
}