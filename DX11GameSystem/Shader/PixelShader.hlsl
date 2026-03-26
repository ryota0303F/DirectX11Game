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
    float3 L = lightpos.xyz;
    float3 N = input.normal;
    float LN = dot(L, N);
    float3 diffuse = float3(1.0f, 1.0f, 1.0f) * max(0.0f, LN) * 0.12f;

    //スペキュラ
    float3 R = reflect(-lightpos.xyz, input.normal);
    float3 V = normalize(eyepos.xyz - input.normal);
    float RV = dot(R, V);
    float3 specular = float3(1.0f, 1.0f, 1.0f) * pow(max(0.0f, RV), 13.6f) * 0.000001f;

    return float4(saturate(ambient + diffuse + specular), 1.0f);
}