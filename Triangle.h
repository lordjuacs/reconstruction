//
// Created by juacs on 27/11/2023.
//

#ifndef RECONSTRUCTION_TRIANGLE_H
#define RECONSTRUCTION_TRIANGLE_H

#include <glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glad.h>
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include "shader_m.h"

#include <iostream>
#include <ctime>
#include <vector>
#include <random>

class Triangle {
public:
    GLint POSITION_ATTRIBUTE = 0, NORMAL_ATTRIBUTE = 1;
    bool visible = true;

    GLuint vao = 0;
    GLuint vbos[2]{};
    float escala = 1.0f;
    std::vector<glm::vec3> vertexData;
    std::vector<glm::vec3> normalData;
    glm::vec3 color{};

    Triangle(const std::vector<glm::vec3> &triangle, glm::vec3 col) {
        setup(triangle);
        color = col;
    }

    ~Triangle() {
        /*glDeleteVertexArrays(1, &vao);
        glDeleteBuffers(1, &vbos[0]);
        glDeleteBuffers(1, &vbos[1]);*/
    }

    void setup(const std::vector<glm::vec3> &triangle) {
        // Flatten vertex and normal data
        for (int i = 0; i < 3; i++) {
            vertexData.push_back(triangle[i * 2]);
            normalData.push_back(triangle[i * 2 + 1]);

        }
        if (vao == 0) {
            //GLuint vao;
            glGenVertexArrays(1, &vao);
            glBindVertexArray(vao);

            glGenBuffers(2, vbos);

            glBindBuffer(GL_ARRAY_BUFFER, vbos[0]);
            glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(glm::vec3), vertexData.data(), GL_STATIC_DRAW);
            glVertexAttribPointer(POSITION_ATTRIBUTE, 3, GL_FLOAT, GL_FALSE, 0, (void *) 0);
            glEnableVertexAttribArray(POSITION_ATTRIBUTE);

            glBindBuffer(GL_ARRAY_BUFFER, vbos[1]);
            glBufferData(GL_ARRAY_BUFFER, normalData.size() * sizeof(glm::vec3), normalData.data(), GL_STATIC_DRAW);
            glVertexAttribPointer(NORMAL_ATTRIBUTE, 3, GL_FLOAT, GL_TRUE, 0, (void *) 0);
            glEnableVertexAttribArray(NORMAL_ATTRIBUTE);
        }
        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        //indices_size = indices.size();
    }

    void display(Shader &sh, float cambio_escala) {
        escala = cambio_escala;
        glm::mat4 model = glm::mat4(1.0);
        model = scale(model, glm::vec3(escala));
        //model = glm::rotate(model, glm::radians(rotacion), vec3(0, 1, 1));
        //cout << endl << to_string(centro);
        //model = translate(model, centro);
        sh.setMat4("model", model);
        sh.setVec3("objectColor", color);


        if (visible) {
            glBindVertexArray(vao);
            glDrawArrays(GL_TRIANGLES, 0, 3);
            glBindVertexArray(0);
        }
    }
};




#endif //RECONSTRUCTION_TRIANGLE_H
