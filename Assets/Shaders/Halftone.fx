// Halftone.fx

cbuffer TransformBuffer : register(b0)
{
    matrix wvp; // world*view*proj
    matrix world; // world matrix (for normals)
};

cbuffer LightBuffer : register(b1)
{
    float3 lightDir; // Directional light vector (pointing from surface to light)
    float pad; // Padding
};

cbuffer HalftoneBuffer : register(b2)
{
    float countX; // Number of cells across width
    float countY; // Number of cells across height
    float opacity; // Overlay opacity (0–1)
    float threshold; // Edge thickness for smoothstep
};

struct VS_INPUT
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float2 texCoord : TEXCOORD0; // (not used here)
};

struct VS_OUTPUT
{
    float4 position : SV_Position;
    float3 normal : NORMAL;
};

VS_OUTPUT VS(VS_INPUT input)
{
    VS_OUTPUT output;
    float4 worldPos = mul(float4(input.position, 1.0f), world);
    output.position = mul(worldPos, wvp); // apply world*view*proj
    // Transform normal by world (assumes no non-uniform scale)
    output.normal = mul(input.normal, (float3x3) world);
    return output;
}

float4 PS(VS_OUTPUT input) : SV_Target
{
    // Normalize light direction
    float3 L = normalize(lightDir);
    float3 N = normalize(input.normal);
    // Compute brightness via diffuse term (dot of normal and light)
    float brightness = saturate(dot(N, -L));

    // If below threshold (very dark), skip drawing any dot
    if (brightness <= 0.01f)  
        discard;

    // Compute screen-space UV (0..1)
    float2 uv = input.position.xy / input.position.w;
    uv = uv * 0.5f + 0.5f;

    // Map UV into grid space [0..countX], [0..countY]
    float2 grid = uv * float2(countX, countY);
    // Find center of current cell
    float2 cell = floor(grid) + 0.5f;
    // Distance from pixel to cell center
    float dist = length(grid - cell);

    // Dot radius (in grid units). 0.5 = half cell size.
    float radius = brightness * 0.5f;
    // Smoothly transition at the edge
    float edge = threshold; // e.g. ~0.01
    float mask = 1.0f - smoothstep(radius - edge, radius + edge, dist);

    // Output black dot with alpha = mask * opacity
    return float4(0.0f, 0.0f, 0.0f, mask * opacity);
}
