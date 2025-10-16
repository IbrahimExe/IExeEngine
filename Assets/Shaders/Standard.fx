// Standard Effect for basic lighting, texturing & shading

cbuffer TransformBuffer : register(b0)
{
    matrix wvp;
    matrix world;
    float3 viewPosition;
}

cbuffer LightBuffer : register(b1)
{
    float4 lightAmbient;
    float4 lightDiffuse;
    float4 lightSpecular;
    float3 lightDirection;
}

cbuffer MaterialBuffer : register(b2)
{
    float4 materialEmissive;
    float4 materialAmbient;
    float4 materialDiffuse;
    float4 materialSpecular;
    float materialShininess;
}

SamplerState textureSampler : register(s0);

Texture2D diffuseMap : register(t0);
Texture2D specMap : register(t1);


struct VS_INPUT
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float2 texCoord : TEXCOORD;
};

struct VS_OUTPUT
{
    float4 position : SV_Position;
    float3 worldNormal : NORMAL;
    float3 worldTangent : TANGENT;
    float2 texCoord : TEXCOORD;
    float3 dirToLight : TEXCOORD1;
    float3 dirToView : TEXCOORD2;
};

VS_OUTPUT VS(VS_INPUT input)
{
    VS_OUTPUT output;
    output.position = mul(float4(input.position, 1.0f), wvp);
    output.worldNormal = mul(input.normal, (float3x3) world);
    output.worldTangent = mul(input.tangent, (float3x3) world);
    output.texCoord = input.texCoord;
    output.dirToLight = -lightDirection;
    
    float4 worldPosition = mul(float4(input.position, 1.0f), world);
    output.dirToView = normalize(viewPosition - worldPosition.xyz);
    
    return output;
}

float4 PS(VS_OUTPUT input) : SV_Target
{
    float3 n = normalize(input.worldNormal);
    float3 light = normalize(input.dirToLight);
    float3 view = normalize(input.dirToView);
    
    // Emissive
    float4 emissive = materialEmissive;
    
    // Ambient 
    float4 ambient = lightAmbient * materialAmbient;
    
    // Diffuse
    float d = saturate(dot(light, n));
    float4 diffuse = d * lightDiffuse * materialDiffuse;
    
    // Specular 
    float3 r = reflect(-light, n);
    float base = saturate(dot(r, view));
    float s = pow(base, materialShininess);
    float4 specular = s * lightSpecular * materialSpecular;
    
    // Colours
    float4 diffuseMapColor = diffuseMap.Sample(textureSampler, input.texCoord);
    float4 specMapColor = specMap.Sample(textureSampler, input.texCoord).r;
    
    float4 finalColor = (emissive + ambient + diffuse) * diffuseMapColor + (specular * specMapColor);
    
    return finalColor;
}