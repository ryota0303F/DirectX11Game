cbuffer ConstantBuffer : register(b0)
{
    matrix world;
    matrix view;
    matrix projection;
    float4 lightpos;
}

struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
    float4 normal : NORMAL0;
};

//--------------------------------------------------------------------------------------
// ピクセルシェーダ
//--------------------------------------------------------------------------------------
float4 main(VS_OUTPUT input) : SV_TARGET
{
    float4 L = normalize(lightpos - input.normal);
    float4 N = normalize(input.normal);
    
    return saturate(dot(L, N) * float4(1.0f, 1.0f, 0.0f, 1.0f));
}
