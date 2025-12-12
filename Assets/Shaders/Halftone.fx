// Halftone.fx
// Halftone / Ben-Day dot overlay (composited inside pixel shader)
// cbuffer register layout kept compatible with Standard.fx
// Halftone params are in register(b4)

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

// Halftone-specific parameters (bound to b4 by your C++)
cbuffer HalftoneBuffer : register(b4)
{
    float cellCountX; // number of cells across screen X
    float cellCountY; // number of cells across screen Y
    float opacity; // [0..1] overall halftone strength
    float edgeSoftness; // small value for smoothstep antialiasing
}

SamplerState textureSampler : register(s0);

Texture2D diffuseMap : register(t0);
Texture2D specMap : register(t1);
Texture2D normalMap : register(t2);
Texture2D bumpMap : register(t3);
Texture2D shadowMap : register(t4); // shadow map saved by ShadowEffect

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
        // Bump mapping like Standard.fx (displace along normal)
        float4 bumpColor = bumpMap.SampleLevel(textureSampler, input.texCoord, 0.0f);
        float bumpHeight = (bumpColor.r * 2.0f) - 1.0f;
        localPosition += (input.normal * bumpHeight * bumpMapIntensity);
    }

    // Transform to clip space for camera
    output.position = mul(float4(localPosition, 1.0f), wvp);

    // World-space normal & tangent
    output.worldNormal = mul(input.normal, (float3x3) world);
    output.worldTangent = mul(input.tangent, (float3x3) world);
    output.texCoord = input.texCoord;
    output.dirToLight = -lightDirection; // Standard.fx uses -lightDirection
    // Compute view direction for spec
    float4 worldPosition = mul(float4(localPosition, 1.0f), world);
    output.dirToView = normalize(viewPosition - worldPosition.xyz);

    // Light-space position for shadow lookup (if used)
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

    // Emissive and ambient (match Standard.fx)
    float4 emissive = materialEmissive;
    float4 ambient = lightAmbient * materialAmbient;

    // Diffuse
    float d = saturate(dot(light, n));
    float4 diffuse = d * lightDiffuse * materialDiffuse;

    // Specular (we will ignore spec in brightness but compute for final color)
    float3 r = reflect(-light, n);
    float base = saturate(dot(r, view));
    float s = pow(base, materialShininess);
    float4 specular = s * lightSpecular * materialSpecular;

    // Sample maps
    float4 diffuseMapColor = (useDiffuseMap) ? diffuseMap.Sample(textureSampler, input.texCoord) : float4(1.0f, 1.0f, 1.0f, 1.0f);
    float specMapValue = (useSpecMap) ? specMap.Sample(textureSampler, input.texCoord).r : 1.0f;

    // Compose lit color (ambient + diffuse) * texture + specular
    float3 litColor = (emissive.rgb + ambient.rgb + diffuse.rgb) * diffuseMapColor.rgb + (specular.rgb * specMapValue);

    // Shadow mapping (optional) - same approach as Standard.fx
    if (useShadowMap)
    {
        float actualDepth = 1.0f - (input.lightNDCPosition.z / input.lightNDCPosition.w);
        float2 shadowUV = input.lightNDCPosition.xy / input.lightNDCPosition.w;
        float u = (shadowUV.x + 1.0f) * 0.5f;
        float v = 1.0f - (shadowUV.y + 1.0f) * 0.5f;
        if (saturate(u) == u && saturate(v) == v)
        {
            float savedDepth = shadowMap.Sample(textureSampler, float2(u, v)).r;
            if (savedDepth > actualDepth + depthBias) // If in shadow
            {
                // reduce lit contribution (similar to Standard.fx: keep emissive + ambient only)
                litColor = (emissive.rgb + ambient.rgb) * diffuseMapColor.rgb;
            }
        }
    }

    // Compute luminance from litColor (perceptual weights)
    float lum = dot(litColor, float3(0.299, 0.587, 0.114));

    // Convert clip-space position to screen-space uv in [0,1]
    float2 screenPos = input.position.xy / input.position.w; // clip space [-1,1]
    screenPos = screenPos * 0.5f + 0.5f;

    // Build grid and local cell coords
    float2 grid = screenPos * float2(cellCountX, cellCountY);
    float2 cell = floor(grid) + 0.5f;
    float2 local = grid - cell; // range [-0.5, 0.5]
    float dist = length(local); // distance in cell-space units (0..~0.707)

    // Dot radius (in cell units). Scale this so that radius 0.5 fills cell.
    // We'll map lum [0..1] to radius [0..0.5]
    float radius = saturate(lum) * 0.5f;

    // Smooth mask (anti-aliased circle)
    float mask = 1.0f - smoothstep(radius - edgeSoftness, radius + edgeSoftness, dist);

    // Prevent dots in very dark areas
    if (lum <= 0.01f)
        mask = 0.0f;

    // Compose final color by darkening litColor where mask is high (blend towards black).
    // strength is mask * opacity
    float strength = mask * opacity;
    float3 finalRGB = lerp(litColor, float3(0.0f, 0.0f, 0.0f), strength);

    // Return composited color with original alpha
    return float4(finalRGB, diffuseMapColor.a);
}