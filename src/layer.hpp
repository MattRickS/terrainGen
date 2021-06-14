#pragma once
#include <map>
#include <glm/glm.hpp>

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
    glm::vec3 color;
};

std::map<LayerType, LayerProperties> LAYER_PROPERTIES = {
    {LayerType::Air, {glm::vec3(1.0f, 1.0f, 1.0f)}},
    {LayerType::Rock, {glm::vec3(0.0f, 0.1f, 0.0f)}},
    {LayerType::Soil, {glm::vec3(1.0f, 0.0f, 0.0f)}},
};
