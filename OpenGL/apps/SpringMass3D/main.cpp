#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <iostream>

#include "utils/Camera.hpp"
#include "utils/Shader.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

glm::vec3 cameraPos   = glm::vec3(0.0f, 0.0f,  3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f,  0.0f);

float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame

bool firstMouse = true;
double lastX =  400.;
double lastY =  300.;

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = (float)xpos - (float)lastX;
    float yoffset = (float)lastY - (float)ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    camera.ProcessMouseScroll((float)yoffset);
}

/* window resizing
 */
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

/* Process player's inputs
 */
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
}


class Mass {
    glm::vec3 pos_;
    // add physical stuff...

    Mass(glm::vec3 pos) : pos_(pos)
    {
    }

    Mass()
    {
        pos_ = glm::vec3(0.f, 0.f, 0.f);
    }
};

std::vector<float> generateRectangle(float x, float y) {
    std::vector<float> triangle {
        x, y, 0.0f,
        x + 1, y, 0.0f,
        x + 1, y + 1, 0.0f,

        x + 1, y + 1, 0.0f,
        x, y + 1, 0.0f,
        x, y, 0.0f
    };
    return triangle;
}

std::vector<float> generateFlag(int x, int y) {
    std::vector<float> tmp;
    std::vector<float> flag;
    for(int i = 0; i < y; i++) {
        for(int j = 0; j < x; j++) {
            tmp = generateRectangle((float)j, (float)i);
            flag.insert(flag.end(), tmp.begin(), tmp.end());
        }
    }
    return flag;
}


int main(int argc, char **argv) {
    // Window size
    const int W = 1200;
    const int H = 800;

    // Init GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); // Version 3.3
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create a window
    GLFWwindow* window = glfwCreateWindow(W, H, "LearnOpenGL", nullptr, nullptr);
    if (window == nullptr) {
            std::cout << "Failed to create GLFW window" << std::endl;
            glfwTerminate();
            return -1;
    }

    glfwMakeContextCurrent(window);
    // Call the mouse callback function every frame
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback); // using window resizing function
    glfwSetScrollCallback(window, scroll_callback);
    // Capture the cursor and disable it
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Init Glad
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
            std::cout << "Failed to initialize GLAD" << std::endl;
            return -1;
    }

    Shader shader("bin/shaders/SpringMass3D/3DColor.vs.glsl", "bin/shaders/SpringMass3D/3DColor.fs.glsl");

    /* ----------Initializations----------- */
        
    // Generate flag
    int sizeX = 48;
    int sizeY = 32;
    int flagSize = sizeX * sizeY;
    std::vector<float> vecFlag = generateFlag(sizeX, sizeY);
    // Move the camera with the flag size
    camera = Camera(glm::vec3(sizeX / 2, sizeY / 2, (flagSize / sqrt(flagSize)) * 2));
    //Convert vector to array
    float* vertices = vecFlag.data();

    // Mass vector
    std::vector<Mass> masses;
    // TODO : initialize the masses according the generated flag

    // Cubes VAO VBO
    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, /* number of rectangle * 18 */flagSize * 18 * sizeof(float), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glEnable(GL_DEPTH_TEST);

    shader.use();
    int modelLoc = glGetUniformLocation(shader.ID, "model");
    int viewLoc = glGetUniformLocation(shader.ID, "view");
    int projectionLoc = glGetUniformLocation(shader.ID, "projection");
    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)W / (float)H, 0.1f, 1000.0f);
    glm::mat4 view = camera.GetViewMatrix();
    glm::vec3 Color(1.0f, 0.5f, 0.31f);

    //Draw line mode
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    /* ----------Rendering loop----------- */
    while(!glfwWindowShouldClose(window)) {
    // rendering commands here
            glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            float currentFrame = (float)glfwGetTime();
            deltaTime = currentFrame - lastFrame;
            lastFrame = currentFrame;

            //
    // TODO : Update vertices according masses with physics
    //

            // Input processing
            processInput(window);
            // Update camera position
            projection = glm::perspective(glm::radians(camera.Zoom), (float)W / (float)H, 0.1f, 1000.0f);
            shader.setMat4("projection", projection);
            view = camera.GetViewMatrix();
            shader.setMat4("view", view);
    // Set color
            shader.setVec3("Color", Color);

            // Draw flag
    glBindVertexArray(VAO);
            glm::mat4 model = glm::mat4(1.0f);
            shader.setMat4("model", model);
            glDrawArrays(GL_TRIANGLES, 0, flagSize * 18);

            //
    // TODO : Update the masses position according vertices
    //

            // check and call events and swap the buffers
            glfwPollEvents();
            glfwSwapBuffers(window);
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    glfwTerminate();
    return 0;
}
