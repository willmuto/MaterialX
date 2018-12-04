#include "sxpbrlib/sx-glsl/lib/sx_bsdfs.glsl"

void sx_glossybrdf_reflection(vec3 L, vec3 V, float weight, vec3 color0, vec3 color90, float exponent, roughnessinfo roughness, vec3 normal, vec3 tangent, int distribution, out BSDF result)
{
    if (weight < M_FLOAT_EPS)
    {
        result = BSDF(0.0);
        return;
    }

    float NdotL = dot(normal,L);
    float NdotV = dot(normal,V);
    if (NdotL <= 0.0 || NdotV <= 0.0)
    {
        result = BSDF(0.0);
        return;
    }

    vec3 bitangent = normalize(cross(normal, tangent));

    vec3 H = normalize(L + V);
    float NdotH = dot(normal, H);

    float D = sx_microfacet_ggx_NDF(tangent, bitangent, H, NdotH, roughness.alphaX, roughness.alphaY);
    float G = sx_microfacet_ggx_smith_G(NdotL, NdotV, roughness.alpha);

    float VdotH = dot(V, H);
    vec3 F = sx_fresnel_schlick(VdotH, color0, color90, exponent);
    F *= weight;

    // Note: NdotL is cancelled out
    result = F * D * G / (4 * NdotV);
}

void sx_glossybrdf_indirect(vec3 V, float weight, vec3 color0, vec3 color90, float exponent, roughnessinfo roughness, vec3 normal, vec3 tangent, int distribution, out BSDF result)
{
    if (weight < M_FLOAT_EPS)
    {
        result = vec3(0.0);
        return;
    }

    vec3 Li = sx_environment_specular(normal, V, tangent, roughness);

    float NdotV = dot(normal,V);
    vec3 F = sx_fresnel_schlick(NdotV, color0, color90, exponent);
    F *= weight;

    result = Li * F;
;}
