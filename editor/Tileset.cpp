#include "Tileset.h"

static bool floatEqual(float l, float r, float epsilon = 1e-6f) {
    return std::fabs(l - r) < epsilon;
}

bool Editor::Tileset::Equal::operator()(const std::tuple<float, float>& a, const std::tuple<float, float>& b) const
{
    const auto& [aX, aY] = a;
    const auto& [bX, bY] = b;
    return floatEqual(aX, bX) && floatEqual(aY, bY);
}

size_t Editor::Tileset::Hash::operator()(const std::tuple<float, float>& t) const
{
    std::hash<float> hasher;
    return hasher(std::get<0>(t)) ^ hasher(std::get<1>(t));
}
