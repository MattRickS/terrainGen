#include <map>
#include <vec3.hpp>

enum LayerType
{
    Air,
    Rock,
    Gravel,
    Soil,
    Sand
};

struct LayerProperties
{
    vec3f color;
};

std::map<LayerType, LayerProperties> LAYER_PROPERTIES = {
    {LayerType::Air, {vec3f(1.0f, 1.0f, 1.0f)}},
    {LayerType::Rock, {vec3f(0.2f, 0.2f, 0.2f)}},
    {LayerType::Soil, {vec3f(0.4f, 0.2f, 0.0f)}},
};

struct Layer
{
    LayerType type;
    float height;
};
