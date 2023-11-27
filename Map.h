//
// Created by juacs on 27/11/2023.
//

#ifndef RECONSTRUCTION_MAP_H
#define RECONSTRUCTION_MAP_H

#include <utility>
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>
#include "Triangle.h"

class Map {
    int rows, cols;
    double min_height, max_height, scale_factor = 1;

    std::string eFile;
    std::string rgbFile;
    std::vector<std::vector<double>> elevationMatrix;
    std::vector<std::vector<glm::vec3>> rgbMatrix;

    std::vector<std::pair<std::vector<glm::vec3>, glm::vec3>> triangle_metadata;
    std::vector<Triangle> triangles;
public:
    Map(int rows_, int cols_, double minh, double maxh, std::string elevationFilename, std::string rgbFilename) : rows(
            rows_), cols(cols_), min_height(minh), max_height(maxh), eFile(std::move(elevationFilename)),
                                                                                                                  rgbFile(std::move(
                                                                                                                          rgbFilename)) {}

    void readElevation() {
        std::ifstream file(eFile);

        // Check if the file is open
        if (!file.is_open()) {
            std::cerr << "Error: Could not open the file " << eFile << std::endl;
            return;
        }


        std::string line;
        while (std::getline(file, line)) {
            std::istringstream iss(line);
            std::vector<double> row;

            double value;
            while (iss >> value) {
                value = (value * (max_height - min_height)) + min_height;
                row.push_back(value);
            }

            elevationMatrix.push_back(row);
        }

        file.close();
    }

    void readRGB() {
        std::ifstream file(rgbFile);
        // Check if the file is open
        if (!file.is_open()) {
            std::cerr << "Error: Could not open the file " << rgbFile << std::endl;
            return;
        }

        std::string line;
        while (std::getline(file, line)) {
            std::istringstream iss(line);
            std::vector<glm::vec3> row;

            glm::vec3 rgbValues;
            while (iss >> rgbValues.r >> rgbValues.g >> rgbValues.b) {
                row.push_back(rgbValues);
            }

            if (!row.empty()) {
                rgbMatrix.push_back(row);
            } else {
                std::cerr << "Error: Invalid number of RGB values in line" << std::endl;
            }
        }

        file.close();

    }

    void change_proximity(double scale_factor) {
        this->scale_factor = scale_factor;
    }

    static glm::vec3 calculateNormal(const glm::vec3 &vertex1, const glm::vec3 &vertex2, const glm::vec3 &vertex3) {
        glm::vec3 edge1 = vertex2 - vertex1;
        glm::vec3 edge2 = vertex3 - vertex1;
        glm::vec3 normal = glm::normalize(glm::cross(edge1, edge2));
        return normal;
    }

    void setup() {
        readElevation();
        readRGB();

        for (int i = 0; i < rows - 1; ++i) {
            for (int j = 0; j < cols - 1; ++j) {
                glm::vec3 vertex1(double(i) * scale_factor, elevationMatrix[i][j], double(j) * scale_factor);
                glm::vec3 vertex2(double(i + 1) * scale_factor, elevationMatrix[i + 1][j], double(j) * scale_factor);
                glm::vec3 vertex3(double(i) * scale_factor, elevationMatrix[i][j + 1], double(j + 1) * scale_factor);
                glm::vec3 vertex4(double(i + 1) * scale_factor, elevationMatrix[i + 1][j + 1],
                                  double(j + 1) * scale_factor);

                auto nt1 = calculateNormal(vertex3, vertex1, vertex2);
                auto nt2 = calculateNormal(vertex4, vertex3, vertex2);


                std::vector<glm::vec3> triangle1 = {vertex3, -nt1,
                                                    vertex1, -nt1,
                                                    vertex2, -nt1};
                std::vector<glm::vec3> triangle2 = {vertex4, -nt2,
                                                    vertex3, -nt2,
                                                    vertex2, -nt2};
                double p1x = (rgbMatrix[i][j].x + rgbMatrix[i + 1][j].x + rgbMatrix[i][j + 1].x) / 3;
                double p1y = (rgbMatrix[i][j].y + rgbMatrix[i + 1][j].y + rgbMatrix[i][j + 1].y) / 3;
                double p1z = (rgbMatrix[i][j].z + rgbMatrix[i + 1][j].z + rgbMatrix[i][j + 1].z) / 3;

                double p2x = (rgbMatrix[i + 1][j + 1].x + rgbMatrix[i + 1][j].x + rgbMatrix[i][j + 1].x) / 3;
                double p2y = (rgbMatrix[i + 1][j + 1].y + rgbMatrix[i + 1][j].y + rgbMatrix[i][j + 1].y) / 3;
                double p2z = (rgbMatrix[i + 1][j + 1].z + rgbMatrix[i + 1][j].z + rgbMatrix[i][j + 1].z) / 3;
                triangle_metadata.emplace_back(triangle1, glm::vec3(p1x, p1y, p1z));
                triangle_metadata.emplace_back(triangle2, glm::vec3(p2x, p2y, p2z));
            }
        }
        int x = 0;
        for (const auto &triangle: triangle_metadata) {
            std::cout << x++ << std::endl;
            Triangle t1(triangle.first, triangle.second);
            triangles.emplace_back(t1);
        }
    }

    void display(Shader &sh, float cambio_escala) {
        for (auto &triangle: triangles)
            triangle.display(sh, cambio_escala);
    }
};


#endif //RECONSTRUCTION_MAP_H
