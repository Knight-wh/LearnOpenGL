#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader {
public:
  // the program ID
  unsigned int ID;

  // constructor reads and builds the shader
  
  // computer shader
  Shader(const char* computePath) {
    std::string compCode;
    std::ifstream cShaderFile;
    cShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try
    {
      cShaderFile.open(computePath);
      std::stringstream cShaderStream;
      cShaderStream << cShaderFile.rdbuf();
      cShaderFile.close();
      compCode = cShaderStream.str();
    }
    catch(std::ifstream::failure& e)
    {
      std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ: " << e.what() <<std::endl;
    }
    const char* cShaderCode = compCode.c_str();
    unsigned int compute;
    compute = glCreateShader(GL_COMPUTE_SHADER);
    glShaderSource(compute, 1, &cShaderCode, NULL);
    glCompileShader(compute);
    checkCompileErrors(compute, "COMPUTE");

    ID = glCreateProgram();
    glAttachShader(ID, compute);
    glLinkProgram(ID);
    checkCompileErrors(ID, "PROGRAM");

    glDeleteShader(compute);
  }

  // normal shader
  Shader(const char* vertexPath, const char* fragmentPath,
    const char* tescPath = nullptr, const char* tesePath = nullptr,
    const char* geometryPath = nullptr) {
    // retrieve the vertex/fragment source code from filePath
    // ------------------------------------------------------
    std::string vertexCode;
    std::string fragmentCode;
    std::string tescCode;
    std::string teseCode;
    std::string geometryCode;

    std::ifstream vShaderFile;
    std::ifstream fShaderFile;
    std::ifstream tescShaderFile;
    std::ifstream teseShaderFile;
    std::ifstream gShaderFile;
    // ensure ifstream objects can throw exceptions
    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    tescShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    teseShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    gShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try {
      // open files
      vShaderFile.open(vertexPath);
      fShaderFile.open(fragmentPath);
      std::stringstream vShaderStream, fShaderStream;
      // read file's buffer contents into streams
      vShaderStream << vShaderFile.rdbuf();
      fShaderStream << fShaderFile.rdbuf();
      // close file handlers
      vShaderFile.close();
      fShaderFile.close();
      // convert stream to string
      vertexCode = vShaderStream.str();
      fragmentCode = fShaderStream.str();
      // if tessellation shader path is present
      if (tesePath != nullptr) {
        tescShaderFile.open(tescPath);
        teseShaderFile.open(tesePath);
        std::stringstream tescShaderStream;
        std::stringstream teseShaderStream;
        tescShaderStream << tescShaderFile.rdbuf();
        teseShaderStream << teseShaderFile.rdbuf();
        tescShaderFile.close();
        teseShaderFile.close();
        tescCode = tescShaderStream.str();
        teseCode = teseShaderStream.str();
      }
      // if geometry shader path is present
      if (geometryPath != nullptr) {
        gShaderFile.open(geometryPath);
        std::stringstream gShaderStream;
        gShaderStream << gShaderFile.rdbuf();
        gShaderFile.close();
        geometryCode = gShaderStream.str();
      }
    }
    catch(std::ifstream::failure& e) {
      std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ: " << e.what() <<std::endl;
    }
    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();

    // compile shaders
    // ---------------
    unsigned int vertex, fragment;
    // vertex shader
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);
    checkCompileErrors(vertex, "VERTEX");

    // fragment shader
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);
    checkCompileErrors(fragment, "FRAGMENT");

    // tessellation control shader and tessellation evalution shader
    unsigned int tesc, tese;
    if (tesePath != nullptr)
    {
      const char* tescShaderCode = tescCode.c_str();
      const char* teseShaderCode = teseCode.c_str();
      tesc = glCreateShader(GL_TESS_CONTROL_SHADER);
      tese = glCreateShader(GL_TESS_EVALUATION_SHADER);
      glShaderSource(tesc, 1, &tescShaderCode, NULL);
      glShaderSource(tese, 1, &teseShaderCode, NULL);
      glCompileShader(tesc);
      glCompileShader(tese);
      checkCompileErrors(tesc, "TES_CONTROL");
      checkCompileErrors(tese, "TES_EVALUTION");
    }
    
    // geometry shader
    unsigned int geometry;
    if(geometryPath != nullptr)
    {
      const char * gShaderCode = geometryCode.c_str();
      geometry = glCreateShader(GL_GEOMETRY_SHADER);
      glShaderSource(geometry, 1, &gShaderCode, NULL);
      glCompileShader(geometry);
      checkCompileErrors(geometry, "GEOMETRY");
    }

    // shader program
    ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    if (tesePath != nullptr)
    {
      glAttachShader(ID, tesc);
      glAttachShader(ID, tese);
    }
    if(geometryPath != nullptr)
      glAttachShader(ID, geometry);
    glLinkProgram(ID);
    checkCompileErrors(ID, "PROGRAM");

    // delete the shaders as they're linked into program now
    glDeleteShader(vertex);
    glDeleteShader(fragment);
    if (tesePath != nullptr)
    {
      glDeleteShader(tesc);
      glDeleteShader(tese);
    }
    
    if(geometryPath != nullptr)
      glDeleteShader(geometry);
  };
  // use/active the shader
  void use() {
    glUseProgram(ID);
  };
  // utility uniform functions
  void setBool(const std::string& name, bool value) const {
    glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
  };
  void setInt(const std::string& name, int value) const {
    glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
  };
  void setFloat(const std::string& name, float value) const {
    glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
  };
  void setVec3(const std::string& name, const glm::vec3& value) const {
    glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
  };
  void setVec3(const std::string& name, float x, float y, float z) const {
    glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
  };
  void setMat4(const std::string& name, const glm::mat4& mat) const {
    glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
  };
private:
  void checkCompileErrors(unsigned int shader, std::string type) {
    int success;
    char infoLog[1024];
    if (type != "PROGRAM")
    {
      glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
      if (!success)
      {
        glGetShaderInfoLog(shader, 1024, NULL, infoLog);
        std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
      }
    }
    else
    {
      glGetProgramiv(shader, GL_LINK_STATUS, &success);
      if (!success)
      {
        glGetProgramInfoLog(shader, 1024, NULL, infoLog);
        std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
      }
    }
  };
};