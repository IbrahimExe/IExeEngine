// Hatching.fx
// Cross-hatching anchored to UVs.
// Hatching params bound to register(b4).

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

// Hatching parameters (b4)
cbuffer HatchingBuffer : register(b4)
{
    float cellCountX; // number of cells across U
    float cellCountY; // number of cells across V
    float opacity; // overall hatch strength (0..1)
    float darknessCutoff; // brightness cutoff where hatching begins [0..1]

    float lineThickness; // thickness (in cell-space, 0..0.5 typical)
    float angleA; // this in radians
    float angleB; // also in radians
    float padding1; // paddin
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

// small hash for per-cell randomness in [0..1]
float Hash01(float2 v)
{
    return frac(sin(dot(v, float2(12.9898, 78.233))) * 43758.5453);
}

// rotate a point by angle (radians)
float2 Rotate(float2 p, float angle)
{
    float c = cos(angle);
    float s = sin(angle);
    return float2(p.x * c - p.y * s, p.x * s + p.y * c);
}

float4 PS(VS_OUTPUT input) : SV_Target
{
    // Lighting
    float3 n = normalize(input.worldNormal);
    float3 L = normalize(input.dirToLight);
    float3 V = normalize(input.dirToView);

    if (useNormalMap)
    {
        float3 t = normalize(input.worldTangent);
        float3 b = normalize(cross(n, t));
        float3x3 tbn = float3x3(t, b, n);
        float4 nmap = normalMap.Sample(textureSampler, input.texCoord);
        float3 unpacked = normalize(float3(nmap.xy * 2.0f - 1.0f, nmap.z));
        n = normalize(mul(unpacked, tbn));
    }

    float4 emissive = materialEmissive;
    float4 ambient = lightAmbient * materialAmbient;

    float diffTerm = saturate(dot(L, n));
    float4 diffuse = diffTerm * lightDiffuse * materialDiffuse;

    // Specular
    float3 refl = reflect(-L, n);
    float base = saturate(dot(refl, V));
    float s = pow(base, materialShininess);
    float4 specular = s * lightSpecular * materialSpecular;

    // Textures
    float4 diffMapColor = (useDiffuseMap) ? diffuseMap.Sample(textureSampler, input.texCoord) : float4(1, 1, 1, 1);
    float specVal = (useSpecMap) ? specMap.Sample(textureSampler, input.texCoord).r : 1.0f;

    float3 litColor = (emissive.rgb + ambient.rgb + diffuse.rgb) * diffMapColor.rgb + (specular.rgb * specVal);

    // Shadow test
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
                litColor = (emissive.rgb + ambient.rgb) * diffMapColor.rgb;
            }
        }
    }

    // Luminance
    float lum = dot(litColor, float3(0.299, 0.587, 0.114));

    // Procedural cross-hatch (UV anchored)
    // Map UV to grid-space
    float2 gridUV = input.texCoord * float2(cellCountX, cellCountY);
    float2 cellIndex = floor(gridUV);
    float2 local = frac(gridUV);
    // center local to [-0.5..0.5]
    float2 localC = local - 0.5f;

    // Per cell random offset to break regularity
    float rnd = Hash01(cellIndex) - 0.5f; // [-0.5..0.5]
    float rnd2 = Hash01(cellIndex + 17.0f) - 0.5f;

    // Layer A
    float2 rotA = Rotate(localC, angleA);
    // shift phase by randomness (affects where line crosses cell)
    rotA.x += rnd * 0.25f;
    // tapered factor along line length (tap near edges)
    float taperA = saturate(1.0f - smoothstep(0.45f, 0.50f, abs(rotA.x)));
    float maskA = 1.0f - smoothstep(lineThickness - 0.002f, lineThickness + 0.002f, abs(rotA.y));
    maskA *= taperA;

    // Layer B
    float2 rotB = Rotate(localC, angleB);
    rotB.x += rnd2 * 0.25f;
    float taperB = saturate(1.0f - smoothstep(0.45f, 0.50f, abs(rotB.x)));
    float maskB = 1.0f - smoothstep(lineThickness - 0.002f, lineThickness + 0.002f, abs(rotB.y));
    maskB *= taperB;

    // Combine cross-hatch
    float hatchMask = saturate(max(maskA, maskB));

    // Darkness driving: stronger in darker regions (darknessCutoff in [0..1])
    // darknessFactor = 1 when lum==0, 0 when lum >= darknessCutoff
    float darknessFactor = saturate((darknessCutoff - lum) / max(0.0001, darknessCutoff));

    float finalMask = hatchMask * darknessFactor * opacity;

    // The hatch color is derived from underlying litColor
    float3 hatchColor = litColor;

    // Additively blend hatch on top of litColor; clamp with saturate to avoid overflow
    float3 outRGB = saturate(litColor + hatchColor * finalMask);
    // float3 outRGB = saturate(litColor * finalMask);
    
    return float4(outRGB, diffMapColor.a);
}
