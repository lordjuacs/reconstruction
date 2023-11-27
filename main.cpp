#include <glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader_m.h"
#include "camera.h"
#include "Triangle.h"
#include "Cube.h"
#include "Map.h"

#include <iostream>
#include <random>

void framebuffer_size_callback(GLFWwindow *window, int width, int height);

void mouse_callback(GLFWwindow *window, double xpos, double ypos);

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);

void processInput(GLFWwindow *window);

void readMetadata(const std::string &metadataPath, int &rows, int &cols, std::string &name);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;
float cambio_escala = 1.0f;


int min_height = -10;
int max_height = 0;
int rows, cols;

// lighting
glm::vec3 lightPos;

// camera
Camera camera;
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;


int main() {
    std::string metadata = "../meta.data";
    std::string name;
    readMetadata(metadata, rows, cols, name);
    std::string elevationPath = "../data/elevation/" + name + ".e";
    std::string rgbPath = "../data/rgb/" + name + ".rgb";

    Map map(rows, cols, min_height, max_height, elevationPath, rgbPath);
    map.change_proximity(1.0);
    Cube cube(lightPos);
    // glfw: initialize and configure
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    // configure global opengl state
    glEnable(GL_DEPTH_TEST);
    // build and compile our shader zprogram
    Shader lightingShader("../shaders/basic_lighting.vs", "../shaders/basic_lighting.fs");
    //Shader lightingShader("../1.basico_sin_luz.vs", "../1.basico_sin_luz.fs");
    Shader lightCubeShader("../shaders/light_source.vs", "../shaders/light_source.fs");

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // first, configure the cube's VAO (and VBO)
    map.setup();
    cube.setup();

    // render loop
    while (!glfwWindowShouldClose(window)) {
        // per-frame time logic
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        processInput(window);

        // render
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // be sure to activate shader when setting uniforms/drawing objects
        lightingShader.use();
        lightingShader.setVec3("objectColor", 1.0f, 0.5f, 0.31f);
        lightingShader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
        lightingShader.setVec3("lightPos", lightPos);
        lightingShader.setVec3("viewPos", camera.Position);
        cube.updatePos(lightPos);

        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float) SCR_WIDTH / (float) SCR_HEIGHT, 0.1f,
                                                100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        lightingShader.setMat4("projection", projection);
        lightingShader.setMat4("view", view);
        map.display(lightingShader, cambio_escala);

        lightCubeShader.use();
        lightCubeShader.setMat4("projection", projection);
        lightCubeShader.setMat4("view", view);
        cube.display(lightCubeShader);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
        cambio_escala *= 1.001;
    if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS)
        cambio_escala /= 1.001;
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        lightPos += glm::vec3(2.0f / 10.0f, 0, 0);
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        lightPos += glm::vec3(-2.0f / 10.0f, 0, 0);
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        lightPos += glm::vec3(0, 0, -2.0f / 10.0f);
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        lightPos += glm::vec3(0, 0, 2.0f / 10.0f);
    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
        lightPos += glm::vec3(0, -2.0f / 10.0f, 0);
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
        lightPos += glm::vec3(0, 2.0f / 10.0f, 0);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}


// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow *window, double xposIn, double yposIn) {
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

void readMetadata(const std::string &metadataPath, int &rows, int &cols, std::string &name) {
    // Read the metadata file
    std::ifstream metadataFile(metadataPath);
    if (metadataFile.is_open()) {
        // Read the first line (name of another file)
        std::getline(metadataFile, name);

        // Read the next line (two numbers separated by space)
        std::string line;
        if (std::getline(metadataFile, line)) {
            std::istringstream iss(line);
            iss >> rows >> cols;
        } else {
            // Handle the case where there is no second line or it doesn't contain two numbers
            std::cerr << "Error: Unable to read rows and columns from metadata file." << std::endl;
            return; // Return an error code
        }

        // Close the file
        metadataFile.close();
    } else {
        // Handle the case where the file couldn't be opened
        std::cerr << "Error: Unable to open metadata file." << std::endl;
        return; // Return an error code
    }
    lightPos = glm::vec3(rows / 2, max_height + 5, cols / 2);
    camera = glm::vec3(rows / 2, max_height + 50, cols / 2);

}