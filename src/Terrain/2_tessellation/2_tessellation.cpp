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
// Camera camera(glm::vec3(67.0f, 400.5f, 169.9f),
//   glm::vec3(0.0f, 1.0f, 0.0f),
//   -128.1f, -42.4f);
Camera camera(glm::vec3(600.0f, 400.0f, 600.0f));
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
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
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

  GLint maxTessLevel;
  glGetIntegerv(GL_MAX_TESS_GEN_LEVEL, &maxTessLevel);
  std::cout << "Max available tess level: " << maxTessLevel << std::endl;

  // configure global opengl state
  // -----------------------------
  glEnable(GL_DEPTH_TEST);
  // build and compile shaders
  // -------------------------
  Shader terrainshader(
    "D:/Files/master/Study-Course/learnopengl/LearnOpenGL/data/shaders/Terrain/2_tessellation.vert",
    "D:/Files/master/Study-Course/learnopengl/LearnOpenGL/data/shaders/Terrain/2_tessellation.frag",
    "D:/Files/master/Study-Course/learnopengl/LearnOpenGL/data/shaders/Terrain/2_tessellation.tesc",
    "D:/Files/master/Study-Course/learnopengl/LearnOpenGL/data/shaders/Terrain/2_tessellation.tese"
  );

  //stbi_set_flip_vertically_on_load(true);
  int width, height, nrComponents;
  unsigned char* data = stbi_load("D:/Files/master/Study-Course/learnopengl/LearnOpenGL/data/textures/Terrain/iceland_heightmap.png", &width, &height, &nrComponents, 0);
  stbi_image_free(data);

  unsigned int heigtMap = loadTexture("D:/Files/master/Study-Course/learnopengl/LearnOpenGL/data/textures/Terrain/iceland_heightmap.png");

  // generate terrain vertices
  std::vector<float> vertices;
  unsigned int rez = 20;
  //float yScale = 64.0f / 256.0f, yShift = 16.0f;
  for (unsigned int i = 0; i < rez - 1; i++)
  {
    for (unsigned int j = 0; j < rez - 1; j++)
    {
      // push 4 vertices for one patch
    
      // original computation equaltion
      // vertices.push_back(-width / 2.0f + i * (width / rez)); // x
      // vertices.push_back(-height / 2.0f + j * (height / rez)); // z
      // vertices.push_back((i * (width / rez)) / width); // u
      // vertices.push_back((j * (height / rez)) / height); // v
      // simplified equalition
      vertices.push_back(-width / 2.0f + i * width / static_cast<float>(rez)); // x
      vertices.push_back(0.0f); // y
      vertices.push_back(-height / 2.0f + j * height / static_cast<float>(rez)); // z
      vertices.push_back(i / static_cast<float>(rez)); // u
      vertices.push_back(j / static_cast<float>(rez)); // v

      vertices.push_back(-width / 2.0f + width * (i + 1) / static_cast<float>(rez)); // v.x
      vertices.push_back(0.0f); // v.y
      vertices.push_back(-height / 2.0f + height * j / static_cast<float>(rez)); // v.z
      vertices.push_back((i + 1) / static_cast<float>(rez)); // u
      vertices.push_back(j / static_cast<float>(rez)); // v

      vertices.push_back(-width / 2.0f + width * i / static_cast<float>(rez)); // v.x
      vertices.push_back(0.0f); // v.y
      vertices.push_back(-height / 2.0f + height * (j + 1) / static_cast<float>(rez)); // v.z
      vertices.push_back(i / static_cast<float>(rez)); // u
      vertices.push_back((j + 1) / static_cast<float>(rez)); // v

      vertices.push_back(-width / 2.0f + width * (i + 1) / static_cast<float>(rez)); // v.x
      vertices.push_back(0.0f); // v.y
      vertices.push_back(-height / 2.0f + height * (j + 1) / static_cast<float>(rez)); // v.z
      vertices.push_back((i + 1) / static_cast<float>(rez)); // u
      vertices.push_back((j + 1) / static_cast<float>(rez)); // v
    }
  }

  GLuint terrainVAO, terrainVBO;
  //, terrainEBO;
  glGenVertexArrays(1, &terrainVAO);
  glBindVertexArray(terrainVAO);
  glGenBuffers(1, &terrainVBO);
  glBindBuffer(GL_ARRAY_BUFFER, terrainVBO);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  glPatchParameteri(GL_PATCH_VERTICES, 4);

  // wireframe
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  glm::vec3 lightPos(0.5f, 1.0f, 0.3f);

  // shader config
  terrainshader.use();
  terrainshader.setInt("heightMap", 0);

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
    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 5000.0f);
    glm::mat4 view = camera.GetViewMatrix();
    glm::mat4 model = glm::mat4(1.0f);
    //model = glm::scale(model, glm::vec3(1.0f));

    // render terrain
    terrainshader.use();
    terrainshader.setMat4("model", model);
    terrainshader.setMat4("projection", projection);
    terrainshader.setMat4("view", view);

    glBindVertexArray(terrainVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, heigtMap);
    glDrawArrays(GL_PATCHES, 0, 4 * rez * rez);
 
    // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
    // -------------------------------------------------------------------------------
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glDeleteVertexArrays(1, &terrainVAO);
  glDeleteBuffers(1, &terrainVBO);

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