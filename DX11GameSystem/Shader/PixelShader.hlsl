struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
    float4 color : COLOR0;
};

//--------------------------------------------------------------------------------------
// ピクセルシェーダ
//--------------------------------------------------------------------------------------
float4 main(VS_OUTPUT input) : SV_TARGET
{
    return input.color;
}