#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <iostream>
#include <unordered_map>
#include "ShaderProgram.h"
#include "Chunk.h"

const unsigned int SCR_WIDTH = 1000;
const unsigned int SCR_HEIGHT = 600;
const float FOV = 45.0f;

// Mouse
bool firstMouse = true;
float yaw = -90.0f;
float pitch = 0.0f;
float lastX = SCR_WIDTH / 2.0;
float lastY = SCR_HEIGHT / 2.0;

// Camera
glm::vec3 cameraPos = glm::vec3(0.0f, 40.0f, 0.0f); // start in centre of world at altitude
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

// Chunks
std::unordered_map<std::string, Chunk*> chunkMap;
int playerChunkX = floor(cameraPos.x / CHUNK_SIZE);
int playerChunkY = floor(cameraPos.y / CHUNK_SIZE);
int playerChunkZ = floor(cameraPos.z / CHUNK_SIZE);

// Timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processMouseMove(GLFWwindow* window, double xPos, double yPos);
void processMouseButton(GLFWwindow* window, int button, int action, int mods);
void processKeyboardInput(GLFWwindow* window);
void loadChunks();

int main()
{
    // Initialize GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create window
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Voxoli", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window\n";
        glfwTerminate();
        return EXIT_FAILURE;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, processMouseMove);
    glfwSetMouseButtonCallback(window, processMouseButton);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Load OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD\n";
        return EXIT_FAILURE;
    }

    // Read and compile shaders
    ShaderProgram* shaderProgram = new ShaderProgram("vertex.glsl", "fragment.glsl");
    const unsigned int shaderId = (*shaderProgram).getId();

    // Generate initial chunks
    loadChunks();

    // Vertex data and buffers
    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glUseProgram(shaderId);

    // Load texture atlas
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    int width, height, nrChannels;

    unsigned char* data = stbi_load("atlas.png", &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture\n";
    }
    stbi_image_free(data);

    glBindTexture(GL_TEXTURE_2D, texture);

    // Set matrices
    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 projection = glm::mat4(1.0f);

    view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
    projection = glm::perspective(glm::radians(FOV),(float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);

    unsigned int modelLoc = glGetUniformLocation(shaderId, "model");
    unsigned int viewLoc = glGetUniformLocation(shaderId, "view");
    unsigned int projLoc = glGetUniformLocation(shaderId, "projection");

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

    glClearColor(0.3f, 0.6f, 0.9f, 1.0f);
    glEnable(GL_DEPTH_TEST);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    // Render loop
    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        processKeyboardInput(window);

        view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        for (auto const& chunkPair : chunkMap) {
            auto chunk = chunkPair.second;
            if (chunk->isMeshed()) {
                glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
                glEnableVertexAttribArray(0);

                glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
                glEnableVertexAttribArray(1);

                glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
                glEnableVertexAttribArray(2);

                glBindBuffer(GL_ARRAY_BUFFER, VBO);
                glBufferData(GL_ARRAY_BUFFER,
                    chunk->vertices.size() * sizeof(float),
                    chunk->vertices.data(),
                    GL_STATIC_DRAW);

                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
                glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                    chunk->indices.size() * sizeof(unsigned int),
                    chunk->indices.data(),
                    GL_STATIC_DRAW);

                glBindVertexArray(VAO);
                glDrawElements(GL_TRIANGLES, chunk->indices.size(), GL_UNSIGNED_INT, 0);
            }
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Tidy up
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glfwTerminate();
    return EXIT_SUCCESS;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void processKeyboardInput(GLFWwindow* window)
{
    // Exit
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    // Move camera
    float cameraSpeed = 3.0f * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
        cameraSpeed *= 5;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraUp;
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraUp;
    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) {
        std::cout << "Cam pos  (" << cameraPos.x << ", " << cameraPos.y << ", " << cameraPos.z << ")\n";
        std::cout << "In chunk (" << playerChunkX << ", " << playerChunkY << ", " << playerChunkZ << ")\n";
    }

    // Update chunks on player move
    if (playerChunkX != floor(cameraPos.x / CHUNK_SIZE)
        || playerChunkY != floor(cameraPos.y / CHUNK_SIZE)
        || playerChunkZ != floor(cameraPos.z / CHUNK_SIZE)) {
        playerChunkX = floor(cameraPos.x / CHUNK_SIZE);
        playerChunkY = floor(cameraPos.y / CHUNK_SIZE);
        playerChunkZ = floor(cameraPos.z / CHUNK_SIZE);
        loadChunks();
    }
}

void processMouseMove(GLFWwindow* window, double xPos, double yPos)
{
    if (firstMouse)
    {
        lastX = xPos;
        lastY = yPos;
        firstMouse = false;
    }

    float xoffset = xPos - lastX;
    float yoffset = lastY - yPos;
    lastX = xPos;
    lastY = yPos;

    float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);
}

void processMouseButton(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        auto it = chunkMap.find(Chunk::getId(playerChunkX, playerChunkY, playerChunkZ));
        if (it != chunkMap.end()) {
            auto currentChunk = it->second;
            auto hit = currentChunk->getBlock(cameraPos, cameraFront, 10);
            if (hit.face != -1) {
                std::cout << "Hit (" << hit.xIndex << ", " << hit.yIndex << ", " << hit.zIndex << ")\n";
                std::cout << "In chunk (" << playerChunkX << ", " << playerChunkY << ", " << playerChunkZ << ")\n";
                std::cout << "On face " << hit.face << "\n";
                currentChunk->setBlock(hit.xIndex, hit.yIndex, hit.zIndex, Block::BlockType::Air);
                currentChunk->remesh();
            }
        }
    }
}

void loadChunks() {
    // Seed chunks
    int range = 5;
    for (int x = -range; x < range + 1; x++) {
        for (int z = -range; z < range + 1; z++) {
            std::string id = Chunk::getId(playerChunkX + x, 0, playerChunkZ + z);
            if (chunkMap.find(id) == chunkMap.end()) {
                Chunk* chunk = new Chunk(playerChunkX + x, 0, playerChunkZ + z);
                chunkMap.insert(std::pair<std::string, Chunk*>(chunk->id, chunk));
            }
        }
    }

    // Generate and mesh chunks
    for (auto const& chunkPair : chunkMap) {
        auto chunk = chunkPair.second;
        auto xNeighbour = chunkMap.find(Chunk::getId(chunk->x + 1, chunk->y, chunk->z));
        auto zNeighbour = chunkMap.find(Chunk::getId(chunk->x, chunk->y, chunk->z + 1));
        auto xzNeighbour = chunkMap.find(Chunk::getId(chunk->x + 1, chunk->y, chunk->z + 1));

        if (xNeighbour != chunkMap.end() && zNeighbour != chunkMap.end() && xzNeighbour != chunkMap.end()) {
            chunk->generate(xNeighbour->second, zNeighbour->second, xzNeighbour->second);
            chunk->mesh();
        }
    }

    // Unload chunks out of range
    for (auto it = chunkMap.cbegin(), next_it = it; it != chunkMap.cend(); it = next_it)
    {
        ++next_it;
        if (abs(it->second->x - playerChunkX) > 2 * range || abs(it->second->z - playerChunkZ) > 2 * range)
        {
            delete it->second;
            chunkMap.erase(it);
        }
    }
}
