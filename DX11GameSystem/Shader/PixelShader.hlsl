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
};

//--------------------------------------------------------------------------------------
// ピクセルシェーダ
//--------------------------------------------------------------------------------------
float4 main(VS_OUTPUT input) : SV_TARGET
{
    //アンビエント
    float3 ambient = float3(1.0f, 1.0f, 0.0f) * 0.6f;

    //ディフューズ
    float3 L = normalize(lightpos.xyz);
    float3 N = normalize(input.normal);
    float NL = dot(N, L);
    float3 diffuse = float3(1.0f, 1.0f, 1.0f) * saturate(NL) * 0.3f;

    //スペキュラ
    float3 R = 2.0f * N * NL - L;
    float3 V = normalize(eyepos.xyz - input.normal);
    float RV = saturate(dot(R, V));
    float3 specular = float3(1.0f, 1.0f, 1.0f) * pow(RV, 10.0f) * 0.15f;

    return float4(ambient + diffuse + specular, 1.0f);
}