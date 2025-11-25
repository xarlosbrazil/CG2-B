#include "loadObj.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <glm/glm.hpp>

// Função para carregar um OBJ
bool loadObj(const char* path, std::vector<float>& out_vertices)
{
    std::vector<glm::vec3> temp_positions; // Temporarily store positions for vectors
	std::vector<glm::vec2> temp_texCoords; // Temporarily store texture coordinates for vectors
	std::vector<glm::vec3> temp_normals; // Temporarily store normals for vectors


	std::ifstream file(path); // Open the file and store in 'file'

	if (!file.is_open()) { // Check if the file was opened successfully
        std::cerr << "ERRO: Não carregou o obj" << path << std::endl;
        return false;
    }

	std::string line; // To store each line of the file
	while (std::getline(file, line)) // Read the file line by line -- getline reads until it finds a newline character and stores it in 'line'
    {
		std::stringstream ss(line); // Create a stringstream from the line for easy parsing
		std::string prefix; // To store the prefix of each line (e.g., "v", "f") -- allows the extraction operator (>>) to ignore unwanted parts of the line
		ss >> prefix; // Pega a primeira palavra da linha (ex: "v", "f", "vf") -- faz isso porque prefix é uma variável string e o operador >> lê até encontrar um espaço em branco

		if (prefix == "v") // If the line starts with "v", it's a vertex position
        {
			glm::vec3 position; // Create a vec3 to store the position
			ss >> position.x >> position.y >> position.z; // Now the position variable will search for the next three floats in the line
			temp_positions.push_back(position); // Store the position by adding it to the last position of the vector
        }

		else if (prefix == "vt") // If the line starts with "vt", it's a texture coordinate
        {
            glm::vec2 texCoord;
            ss >> texCoord.x >> texCoord.y;
            texCoord.y = 1.0f - texCoord.y;
            temp_texCoords.push_back(texCoord);
        }

        else if (prefix == "vn") { // Leitura das Normais
            glm::vec3 normal;
            ss >> normal.x >> normal.y >> normal.z;
            temp_normals.push_back(normal);

        }
		else if (prefix == "f") // If the line starts with "f", it's a face
        {
			// OBJ format for faces can be like "f 1/1/1 2/2/2 3/3/3"
            unsigned int posIndex, texIndex, normalIndex;
            char slash;

            for (int i = 0; i < 3; ++i) {

                ss >> posIndex >> slash >> texIndex >> slash >> normalIndex;

                glm::vec3 position = temp_positions[posIndex - 1];
                out_vertices.push_back(position.x);
                out_vertices.push_back(position.y);
                out_vertices.push_back(position.z);

                if (!temp_texCoords.empty() && texIndex > 0) {

                    glm::vec2 texCoord = temp_texCoords[texIndex - 1];
                    out_vertices.push_back(texCoord.x);
                    out_vertices.push_back(texCoord.y);

                }

                if (!temp_normals.empty() && normalIndex > 0) {

                    glm::vec3 normal = temp_normals[normalIndex - 1];
                    out_vertices.push_back(normal.x);
                    out_vertices.push_back(normal.y);
                    out_vertices.push_back(normal.z);

                }
            }
        }
    }

    return true; // Optionally return true if loading was successful
}