#pragma once

#include <vector>
#include <glm/glm.hpp>

bool loadObj(const char* path, std::vector<float>& out_vertices); // É um bool para indicar sucesso ou falha