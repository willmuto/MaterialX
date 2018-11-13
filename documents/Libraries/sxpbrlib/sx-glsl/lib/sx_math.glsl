float sx_square(float x)
{
    return x*x;
}

vec2 sx_square(vec2 x)
{
    return x*x;
}

vec3 sx_square(vec3 x)
{
    return x*x;
}

vec4 sx_square(vec4 x)
{
    return x*x;
}

float sx_max_component(vec2 v)
{
    return max(v.x, v.y);
}

float sx_max_component(vec3 v)
{
    return max(max(v.x, v.y), v.z);
}

float sx_max_component(vec4 v)
{
    return max(max(max(v.x, v.y), v.z), v.w);
}

bool sx_is_tiny(float v)
{
    return abs(v) < M_FLOAT_EPS;
}

bool sx_is_tiny(vec3 v)
{
    return all(lessThan(abs(v), vec3(M_FLOAT_EPS)));
}

// https://www.graphics.rwth-aachen.de/publication/2/jgt.pdf
float sx_golden_ratio_sequence(int i)
{
    return fract((float(i) + 1.0) * M_GOLDEN_RATIO);
}

// https://people.irisa.fr/Ricardo.Marques/articles/2013/SF_CGF.pdf
vec2 sx_spherical_fibonacci(int i, int numSamples)
{
    return vec2((float(i) + 0.5) / float(numSamples), sx_golden_ratio_sequence(i));
}
