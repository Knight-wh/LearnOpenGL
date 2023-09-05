#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <Util/stb_image.h>
#include <Util/shader.h>
#include <Util/camera.h>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
unsigned int loadTexture(const char* path);
unsigned int loadHeightMap(const char* path, int& width, int& height, int& nrComponents);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 40.0f, 3.0f));
float lastX = (float)SCR_WIDTH / 2.0;
float lastY = (float)SCR_HEIGHT / 2.0;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;
// for FPS counter
float lastTimeFPS = static_cast<float>(glfwGetTime());
int nbFrames = 0;

int main() {
  // glfw: initialize and configure
  // ------------------------------
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

  // glfw window creation
  // --------------------
  GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
  if (window == NULL)
  {
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
  // ---------------------------------------
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
  {
    std::cout << "Failed to initialize GLAD" << std::endl;
    return -1;
  }

  // configure global opengl state
  // -----------------------------
  glEnable(GL_DEPTH_TEST);

  // build and compile shaders
  // -------------------------
  Shader terrainshader("D:/Files/master/Study-Course/learnopengl/LearnOpenGL/data/shaders/Terrain/terrain.vert",
    "D:/Files/master/Study-Course/learnopengl/LearnOpenGL/data/shaders/Terrain/terrain.frag");

  stbi_set_flip_vertically_on_load(true);
  int width, height, nrComponents;
  unsigned char* data = stbi_load("D:/Files/master/Study-Course/learnopengl/LearnOpenGL/data/textures/Terrain/iceland_heightmap.png", &width, &height, &nrComponents, 0);
  // unsigned char* data = stbi_load("assets/stable-diffusion-xl-DEM2.jpg", &width, &height, &nrComponents, 0);
  // unsigned int HeightMap = loadHeightMap("assets/iceland_heightmap.png", width, height, nrComponents);
  // generate terrain vertices
  std::vector<float> vertices;
  float yScale = 64.0f / 256.0f, yShift = 16.0f;
  for (unsigned int i = 0; i < height; i++)
  {
    for (unsigned int j = 0; j < width; j++)
    {
      // retrieve texel for (i, j) tex coord
      unsigned char* texel = data + (j + i * width) * nrComponents;
      // raw height at coordinate
      unsigned char y = texel[0];

      // vertex
      vertices.push_back(-height / 2.0f + i); // x
      vertices.push_back((int)y * yScale - yShift); // y
      vertices.push_back(-width / 2.0f + j);
    }
  }
  stbi_image_free(data);
  // generate vertices indices
  std::vector<unsigned int> indices;
  for (unsigned int i = 0; i < height - 1; i++)
  {
    for (unsigned int j = 0; j < width; j++)
    {
      for (unsigned int k = 0; k < 2; k++)
        indices.push_back((i + k) * width + j);
    }
  }

  const unsigned int NUM_STRIPS = height - 1;
  const unsigned int NUM_VERTS_PER_STRIP = width * 2;

  GLuint terrainVAO, terrainVBO, terrainEBO;
  glGenVertexArrays(1, &terrainVAO);
  glBindVertexArray(terrainVAO);
  glGenBuffers(1, &terrainVBO);
  glBindBuffer(GL_ARRAY_BUFFER, terrainVBO);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
  glEnableVertexAttribArray(0);
  glGenBuffers(1, &terrainEBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, terrainEBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);


  // wireframe
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  glm::vec3 lightPos(0.5f, 1.0f, 0.3f);

  while (!glfwWindowShouldClose(window))
  {
    // per-frame time logic
    // --------------------
    float currentFrame = static_cast<float>(glfwGetTime());
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;
    // for FPS
    nbFrames++;
    if (currentFrame - lastTimeFPS >= 1.0f) {
      std::cout << 1000.0 / (float)(nbFrames) << " ms/frame" << std::endl;
      nbFrames = 0;
      lastTimeFPS += 1.0f;
    }

    // input
    // -----
    processInput(window);

    // render
    // ------
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // configure view/projection matrices
    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 300.0f);
    glm::mat4 view = camera.GetViewMatrix();
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::scale(model, glm::vec3(0.1f));

    // render terrain
    terrainshader.use();
    terrainshader.setMat4("model", model);
    terrainshader.setMat4("projection", projection);
    terrainshader.setMat4("view", view);
    glBindVertexArray(terrainVAO);
    // render the mesh triangle strip
    for (unsigned int strip = 0; strip < NUM_STRIPS; ++strip)
    {
      glDrawElements(GL_TRIANGLE_STRIP, NUM_VERTS_PER_STRIP, GL_UNSIGNED_INT,
        (void*)(sizeof(unsigned int) * NUM_VERTS_PER_STRIP * strip));
    }

    // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
    // -------------------------------------------------------------------------------
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glDeleteVertexArrays(1, &terrainVAO);
  glDeleteBuffers(1, &terrainVBO);
  glDeleteBuffers(1, &terrainEBO);

  glfwTerminate();
  return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
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

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
  // make sure the viewport matches the new window dimensions; note that width and 
  // height will be significantly larger than specified on retina displays.
  glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
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

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
  camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

// utility function for loading a 2D texture from file
// ---------------------------------------------------
unsigned int loadTexture(char const* path)
{
  unsigned int textureID;
  glGenTextures(1, &textureID);

  int width, height, nrComponents;
  unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
  if (data)
  {
    GLenum format{};
    if (nrComponents == 1)
      format = GL_RED;
    else if (nrComponents == 3)
      format = GL_RGB;
    else if (nrComponents == 4)
      format = GL_RGBA;

    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT); // for this tutorial: use GL_CLAMP_TO_EDGE to prevent semi-transparent borders. Due to interpolation it takes texels from next repeat 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(data);
  }
  else
  {
    std::cout << "Texture failed to load at path: " << path << std::endl;
    stbi_image_free(data);
  }

  return textureID;
}

unsigned int loadHeightMap(const char* path, int& width, int& height, int& nrComponents)
{
  unsigned int textureID;
  glGenTextures(1, &textureID);
  unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
  if (data)
  {
    GLenum format{};
    if (nrComponents == 1)
      format = GL_RED;
    else if (nrComponents == 3)
      format = GL_RGB;
    else if (nrComponents == 4)
      format = GL_RGBA;

    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT); // for this tutorial: use GL_CLAMP_TO_EDGE to prevent semi-transparent borders. Due to interpolation it takes texels from next repeat 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(data);
  }
  else
  {
    std::cout << "Texture failed to load at path: " << path << std::endl;
    stbi_image_free(data);
  }

  return textureID;
}