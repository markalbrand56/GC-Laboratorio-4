#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <iostream>
#include <fstream>
#include "gl.h"

enum class Shader {
    Earth,
    Sun,
    Moon,
    Jupiter,
};

class Model {
public:
    glm::mat4 modelMatrix;
    std::vector<glm::vec3> vertices;
    Uniforms uniforms;
    Shader shader;
};


bool loadOBJ(
        const std::string& path,
        std::vector<glm::vec3>& out_vertices,
        std::vector<Face>& out_faces,
        std::vector<glm::vec3>& out_normals,
        std::vector<glm::vec3>& out_texcords)
        {
    // Open the OBJ file
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Error opening OBJ file: " << path << std::endl;
        return false;
    }

    // Variables to store the vertices and faces from the OBJ file
    std::vector<glm::vec3> vertices;
    std::vector<Face> faces;

    // Read the file line by line
    std::string line;
    while (std::getline(file, line)) {
        // Split the line into tokens
        std::istringstream iss(line);
        std::string type;
        iss >> type;

        // Process vertex data
        if (type == "v") {
            glm::vec3 vertex;
            iss >> vertex.x >> vertex.y >> vertex.z;
            vertices.push_back(vertex);
        }
            // Process face data
        else if (type == "f") {
            Face face;
            std::string vertexStr;
            while (iss >> vertexStr) {
                std::array<int, 3> vertexIndices;
                std::replace(vertexStr.begin(), vertexStr.end(), '/', ' ');
                std::istringstream viss(vertexStr);
                viss >> vertexIndices[0] >> vertexIndices[1] >> vertexIndices[2];
                face.vertexIndices.push_back(vertexIndices);
            }
            faces.push_back(face);
        }
        else if (type == "vt") {
            glm::vec3 tex;
            iss >> tex.x >> tex.y >> tex.z;
        }
        else if (type == "vn") {
            glm::vec3 normal;
            iss >> normal.x >> normal.y >> normal.z;
        }
    }

    // Close the file
    file.close();

    // Return the extracted vertices and faces
    out_vertices = vertices;
    out_faces = faces;

    return true;
}