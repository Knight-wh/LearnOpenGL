#pragma once
#include <glad/glad.h>

class Texture2D
{
public:
  // holds the ID of the texture object
  unsigned int ID;
  // texture image dimesions
  unsigned int Width, Height;
  // texture Format
  unsigned int Internal_Format; // format of texture object
  unsigned int Image_Format; // format of loaded image
  // texture configuration
  unsigned int Wrap_S; // wrapping mode on S axis
  unsigned int Wrap_T; // wrapping mode on T axis
  unsigned int Filter_Min; // filtering mode if texture pixels < screen pixels
  unsigned int Filter_Max; // filtering mode if texture pixels > screen pixels
  // constructor (sets default texture modes)
  Texture2D():Width(0), Height(0), Internal_Format(GL_RGB), Image_Format(GL_RGB),
    Wrap_T(GL_REPEAT), Filter_Min(GL_LINEAR), Filter_Max(GL_LINEAR)
  {
    glGenTextures(1, &this->ID);
  };
  // generates texture from image data
  void Generate(unsigned int width, unsigned int height, unsigned char* data)
  {
    this->Width = width;
    this->Height = height;
    // create Texture
    glBindTexture(GL_TEXTURE_2D, this->ID);
    glTexImage2D(GL_TEXTURE_2D, 0, this->Internal_Format, width, height, 0, this->Image_Format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    // set Texture wrap and filter modes
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, this->Wrap_S);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, this->Wrap_T);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, this->Filter_Min);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, this->Filter_Max);
    // unbind texture
    glBindTexture(GL_TEXTURE_2D, 0);
  };
  // binds the texture as the current active GL_TEXTURE_2D texture object
  void Bind() const
  {
    glBindTexture(GL_TEXTURE_2D, this->ID);
  };
};