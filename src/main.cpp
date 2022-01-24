#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <shader.hpp>
#include <camera.hpp>
#include <PerlinNoise.hpp>

#include <iostream>
#include <cmath>
#include <array>

constexpr int gridSize = 100;

int windowWidth = 1920;
int windowHeight = 1080;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

float yaw = -90.0f;
float pitch = 0.0f;

void framebufferSizeCallback(GLFWwindow* window, int newWidth, int newHeight)
{
    windowWidth = newWidth;
    windowHeight = newHeight;
    glViewport(0, 0, windowWidth, windowHeight);
}  

void mouseCallback(GLFWwindow* window, double xposIn, double yposIn) {
    static float lastX = windowWidth / 2;
    static float lastY = windowHeight / 2;
    static bool firstMouse = true;

    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
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

void processInput(GLFWwindow* window) {
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

void MessageCallback(
    unsigned int source,
    unsigned int type,
    unsigned int id,
    unsigned int severity,
    int length,
    const char* message,
    const void* userParam)
{
    if (type == GL_DEBUG_TYPE_ERROR)
    {
        std::cout << "GL CALLBACK: Critical type = " << type << ", severity = " << severity << ", message = " << message << std::endl;
    }
}

int main() {
    float vertices[] = {
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
        0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
    };

    int indices[] {
        3, 2, 0,
        1, 0, 2
    };

    // Initialize glfw and set values
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4.5);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4.5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create window
    GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, "Spunch Bob", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create window!\n";
        glfwTerminate();
        return -1;
    }

    // Set context (of OpenGL) to the window
    glfwMakeContextCurrent(window);

    // Check if glad loaded properly
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);  
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(MessageCallback, 0);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);  
    // Create viewport & tell glfw what to do when window size changes
    glViewport(0, 0, windowWidth, windowHeight);    
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    glfwSetCursorPosCallback(window, mouseCallback);

    Shader shader("/home/josh/Documents/VSCode/C++/3DRenderingTest/shaders/vertex.glsl", "/home/josh/Documents/VSCode/C++/3DRenderingTest/shaders/fragment.glsl");

    // Generate a VBO 
    GLuint VBO;
    glGenBuffers(1, &VBO);

    // Generate an EBO
    GLuint EBO;
    glGenBuffers(1, &EBO);

    // Generate a VAO
    GLuint VAO;
    glGenVertexArrays(1, &VAO);

    // Apply settings to VAO
    glBindVertexArray(VAO);

    // Putting vertices in buffer
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Telling OpenGL how to interpret the vertices array
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(0));
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Loading Texture 0
    GLuint texture0;
    glGenTextures(1, &texture0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture0);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // Binding and setting texture params
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load("/home/josh/Documents/VSCode/C++/3DRenderingTest/assets/spongebobsad_resize.jpg", &width, &height, &nrChannels, 0); 
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
        unsigned char otherData[] = {255, 0, 255, 255,    0, 0, 0, 255,
                0, 0, 0, 255,   255, 0, 255, 255};
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 2, 2, 0, GL_RGBA, GL_UNSIGNED_BYTE, otherData);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    stbi_image_free(data);
    
     // Loading Texture 1
    GLuint texture1;
    glGenTextures(1, &texture1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture1);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // Binding and setting texture params
    width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    data = stbi_load("/home/josh/Documents/VSCode/C++/3DRenderingTest/assets/awesomeface.png", &width, &height, &nrChannels, 0); 
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
        std::cout << "Failed to load texture" << std::endl;
        unsigned char otherData[] = {255, 0, 255, 255,    0, 0, 0, 255,
                0, 0, 0, 255,   255, 0, 255, 255};
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 2, 2, 0, GL_RGBA, GL_UNSIGNED_BYTE, otherData);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    stbi_image_free(data);

    /*
    // ---- [uncomment to test colours] ---- //
    unsigned char data[] = {
        255, 0, 0, 255,        0, 255, 0, 255,        0, 0, 255, 255,       255, 255, 255, 255,
        0, 255, 0, 255,        0, 0, 255, 255,        255, 255, 255, 255,   255, 0, 0, 255,
        0, 0, 255, 255,        255, 255, 255, 255,    255, 0, 0, 255,       0, 255, 0, 255,
        255, 255, 255, 255,    255, 0, 0, 255,        0, 255, 0, 255,       0, 0, 255, 255

    };
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 2, 2, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    */

    // Cleanup
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Last minute texture things
    shader.use();
    shader.setInt("texture0", 0);
    shader.setInt("texture1", 1);

    // Camera Setup
    /* glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);

    glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 cameraDirectionReversed = glm::normalize(cameraPos - cameraTarget);

    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 cameraRight = glm::normalize(glm::cross(up, cameraDirectionReversed));

    glm::vec3 cameraUp = glm::normalize(glm::cross(cameraDirectionReversed, cameraRight)); 

    glm::mat4 view = glm::lookAt(cameraPos,
                                cameraTarget,
                                cameraUp); */

    const siv::PerlinNoise::seed_type seed = glfwGetTime();
    const siv::PerlinNoise perlin{ seed };

    std::array<std::array<std::array<bool, gridSize>, gridSize>, gridSize> cubePositions {};
    for(float posX = 0.5f; posX < gridSize; ++posX) {
        for(float posY = 0.5f; posY < gridSize; ++posY) {
            for(float posZ = 0.5f; posZ < gridSize; ++posZ) {
                if(perlin.noise3D(posX, posY, posZ) > 0.2) {
                    cubePositions[posX][posY][posZ] = true;
                } else {
                    cubePositions[posX][posY][posZ] = false;
                }
            }
        } 
    }

    while(!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame; 

        // Check for input
        processInput(window);

        // Clear buffer
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Telling OpenGL how to process (render) the vertices and to use the VAO
        shader.use();
        glBindVertexArray(VAO);

        // Textur!
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture0);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture1);

        // Set up matrices
        glm::mat4 view = camera.GetViewMatrix();
        shader.setMat4("view", view);

        glm::mat4 projection;
        projection = glm::perspective(glm::radians(45.0f), ((float)windowWidth) / ((float)windowHeight), 0.1f, 1000.0f);
        shader.setMat4("projection", projection);

        /* 
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, -3.0f));
        model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(1.0f, 0.3f, 0.5f));
        shader.setMat4("model", model);

        glDrawArrays(GL_TRIANGLES, 0, sizeof(vertices) / sizeof(float) * 5); 
        */
        for(float posX = 0.5f; posX < gridSize; ++posX) {
            for(float posY = 0.5f; posY < gridSize; ++posY) {
                for(float posZ = 0.5f; posZ < gridSize; ++posZ) {
                    if(cubePositions[posX][posY][posZ]) {
                        glm::mat4 model = glm::mat4(1.0f);
                        model = glm::translate(model, glm::vec3(posX, posY, posZ));
                        shader.setMat4("model", model);
                        glDrawArrays(GL_TRIANGLES, 0, 36);
                    }
                }
            } 
        }
        // model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(1.0f, 0.3f, 0.5f));
        // model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));

        // If any errors occur, they'll be printed at this step.
        for (int i = glGetError(); i != 0; i = glGetError()) {
            std::cout << i << std::endl;
        }

        // Swap buffers and clean up loop
        glfwSwapBuffers(window);
        glfwPollEvents();    
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glfwTerminate();
    return 0;
}