// Halftone.fx
// UV-anchored Ben-Day dots.
// Halftone params are in register(b4): (cellCountX, cellCountY, opacity, brightnessThreshold)

cbuffer TransformBuffer : register(b0)
{
    matrix wvp;
    matrix world;
    matrix lwvp;
    float3 viewPosition;
};

cbuffer LightBuffer : register(b1)
{
    float4 lightAmbient;
    float4 lightDiffuse;
    float4 lightSpecular;
    float3 lightDirection;
};

cbuffer MaterialBuffer : register(b2)
{
    float4 materialEmissive;
    float4 materialAmbient;
    float4 materialDiffuse;
    float4 materialSpecular;
    float materialShininess;
};

cbuffer SettingBuffer : register(b3)
{
    bool useDiffuseMap;
    bool useSpecMap;
    bool useNormalMap;
    bool useBumpMap;
    bool useShadowMap;
    float bumpMapIntensity;
    float depthBias;
    float padding0;
};

// Halftone CB
cbuffer HalftoneBuffer : register(b4)
{
    float cellCountX;
    float cellCountY;
    float opacity; // global strength 
    float brightnessCutoff; // minimum brightness to start showing dots
}

SamplerState textureSampler : register(s0);

Texture2D diffuseMap : register(t0);
Texture2D specMap : register(t1);
Texture2D normalMap : register(t2);
Texture2D bumpMap : register(t3);
Texture2D shadowMap : register(t4);

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
    float4 lightNDCPosition : TEXCOORD3;
};

VS_OUTPUT VS(VS_INPUT input)
{
    VS_OUTPUT output;

    float3 localPosition = input.position;

    if (useBumpMap)
    {
        float4 bumpColor = bumpMap.SampleLevel(textureSampler, input.texCoord, 0.0f);
        float bumpHeight = (bumpColor.r * 2.0f) - 1.0f;
        localPosition += (input.normal * bumpHeight * bumpMapIntensity);
    }

    output.position = mul(float4(localPosition, 1.0f), wvp);
    output.worldNormal = mul(input.normal, (float3x3) world);
    output.worldTangent = mul(input.tangent, (float3x3) world);
    output.texCoord = input.texCoord;
    output.dirToLight = -lightDirection;

    float4 worldPosition = mul(float4(localPosition, 1.0f), world);
    output.dirToView = normalize(viewPosition - worldPosition.xyz);

    output.lightNDCPosition = mul(float4(localPosition, 1.0f), lwvp);

    return output;
}

float4 PS(VS_OUTPUT input) : SV_Target
{
    // Normalize base vectors
    float3 n = normalize(input.worldNormal);
    float3 light = normalize(input.dirToLight);
    float3 view = normalize(input.dirToView);

    // Normal mapping (optional)
    if (useNormalMap)
    {
        float3 t = normalize(input.worldTangent);
        float3 b = normalize(cross(n, t));
        float3x3 tbnw = float3x3(t, b, n);
        float4 normalMapColor = normalMap.Sample(textureSampler, input.texCoord);
        float3 unpackedNormalMap = normalize(float3((normalMapColor.xy * 2.0f) - 1.0f, normalMapColor.z));
        n = normalize(mul(unpackedNormalMap, tbnw));
    }

    // Emissive + Ambient
    float4 emissive = materialEmissive;
    float4 ambient = lightAmbient * materialAmbient;

    // Diffuse (Lambert)
    float d = saturate(dot(light, n));
    float4 diffuse = d * lightDiffuse * materialDiffuse;

    // Specular (kept for final composite but not used for halftone brightness)
    float3 r = reflect(-light, n);
    float base = saturate(dot(r, view));
    float s = pow(base, materialShininess);
    float4 specular = s * lightSpecular * materialSpecular;

    // Sample texture maps 
    float4 diffuseMapColor = (useDiffuseMap) ? diffuseMap.Sample(textureSampler, input.texCoord) : float4(1.0f, 1.0f, 1.0f, 1.0f);
    float specMapVal = (useSpecMap) ? specMap.Sample(textureSampler, input.texCoord).r : 1.0f;

    // Compose lit color (ambient + diffuse) * texture + specular
    float3 litColor = (emissive.rgb + ambient.rgb + diffuse.rgb) * diffuseMapColor.rgb + (specular.rgb * specMapVal);

    // Shadow mapping
    if (useShadowMap)
    {
        float actualDepth = 1.0f - (input.lightNDCPosition.z / input.lightNDCPosition.w);
        float2 shadowUV = input.lightNDCPosition.xy / input.lightNDCPosition.w;
        float u = (shadowUV.x + 1.0f) * 0.5f;
        float v = 1.0f - (shadowUV.y + 1.0f) * 0.5f;
        if (saturate(u) == u && saturate(v) == v)
        {
            float savedDepth = shadowMap.Sample(textureSampler, float2(u, v)).r;
            if (savedDepth > actualDepth + depthBias)
            {
                // In shadow -> only emissive + ambient remain
                litColor = (emissive.rgb + ambient.rgb) * diffuseMapColor.rgb;
            }
        }
    }

    // Compute luminance (perceptual)
    float lum = dot(litColor, float3(0.299, 0.587, 0.114));


    // HALFTONE (UV-anchored)
    float2 gridUV = input.texCoord * float2(cellCountX, cellCountY); // grid space
    // Find cell center (in grid space)
    float2 cell = floor(gridUV) + 0.5f;
    // Local position inside the cell in [-0.5, 0.5]
    float2 local = gridUV - cell;
    float dist = length(local); // 0..~0.707 depending on cell

    // Dot radius: map lum [0..1] to radius [0..0.5]
    float radius = saturate(lum) * 0.5f;

    // Smooth edge for anti-aliasing. Use small softness relative to 1/cell-size.
    float softness = 0.02;
    float mask = 1.0f - smoothstep(radius - softness, radius + softness, dist);

    // Brightness gating: compute factor that ramps from 0 at brightnessCutoff to 1 at 1.0
    float brightFactor = saturate((lum - brightnessCutoff) / max(0.0001, 1.0 - brightnessCutoff));

    // Final dot alpha
    float dotAlpha = mask * brightFactor * opacity;

    // Avoid tiny artifacts in dark regions
    if (lum <= 0.001f)
        dotAlpha = 0.0f;

    // Composite: darken the lit color by dotAlpha (blend towards black)
    float3 finalRGB = lerp(litColor, float3(0.0f, 0.0f, 0.0f), dotAlpha);

    return float4(finalRGB, diffuseMapColor.a);
}