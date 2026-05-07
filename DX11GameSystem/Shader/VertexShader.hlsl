cbuffer ConstantBuffer : register(b0)
{
    matrix world;
    matrix view;
    matrix projection;
    float4 lightpos;
    float4 eyepos;
}

struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
    float3 normal : NORMAL0;
    float3 color : COLOR;
    float2 tex : TEXCOORD;
};

//--------------------------------------------------------------------------------------
// バーテックスシェーダ
//--------------------------------------------------------------------------------------
VS_OUTPUT main(float4 pos : POSITION, float3 normal : NORMAL, float3 color : COLOR, float3 tex : TEXCOORD)
{
    VS_OUTPUT output = (VS_OUTPUT) 0;
    output.pos = mul(pos, world);
    output.pos = mul(output.pos, view);
    output.pos = mul(output.pos, projection);
    output.normal = mul(normal, (float3x3)world);
    output.color = color;
    output.tex = tex;
    return output;
}