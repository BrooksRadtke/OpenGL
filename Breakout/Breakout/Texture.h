#pragma once
#ifndef TEXTURE_H
#define TEXTURE_H

#include <glad/glad.h>

// Used to store and configure a texture in OpenGL
// It also hosts utility functions for easy management
class Texture2D
{
public:
	// Holds the ID of the texture object, used for all texture operations to reference to this particular texture
	unsigned int ID;

	// texture image dimensions in pixels
	unsigned int Width, Height;
	// Format of loaded image
	unsigned int Image_Format;

	// Texture configuration
	// Wrapping mode on S axis
	unsigned int Wrap_S;
	// Wrapping mode on T axis
	unsigned int Wrap_T;
	// Filtering mode if texture pixels < screen pixels
	unsigned int Filter_Min;
	// Filtering mode if texture pixels > screen pixels
	unsigned int Filter_Max;

	// Constructor (sets default texture modes)
	Texture2D();

	// Generates texture from image data
	void Generate(unsigned int width, unsigned int height, unsigned char* data);

	// binds the texture as the current active GL_TEXTURE_2D texture object
	void Bind() const;
};
#endif // !TEXTURE_H
