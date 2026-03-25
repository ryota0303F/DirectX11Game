cbuffer ConstantBuffer : register(b0)
{
    matrix world;
    matrix view;
    matrix projection;
}

struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
    float4 color : COLOR0;
};

//--------------------------------------------------------------------------------------
// バーテックスシェーダ
//--------------------------------------------------------------------------------------
VS_OUTPUT main(float4 pos : POSITION, float4 color : COLOR)
{
    VS_OUTPUT output = (VS_OUTPUT) 0;
    
    output.pos = mul(pos, world);
    output.pos = mul(output.pos, view);
    output.pos = mul(output.pos, projection);
    
    output.pos = pos;
    output.color = color;
    return output;
}