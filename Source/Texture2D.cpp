#include "Texture2D.h"

#include <stb_image.h>
#include "Logger.h"

namespace Nexus {
	Texture2D::Texture2D(unsigned int width, unsigned int height, GLint internal_format, GLenum format, GLenum type,
		const GLvoid* data, bool enable_mipmap) : Width(width), Height(height), InternalFormat(internal_format)
	{
		glGenTextures(1, &this->ID);

		this->Bind();
		
		glTexImage2D(GL_TEXTURE_2D, 0, this->InternalFormat, this->Width, this->Height, 0, format, type, data);
		if (enable_mipmap) {
			glGenerateMipmap(GL_TEXTURE_2D);
		}

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_MIRRORED_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_MIRRORED_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, enable_mipmap ? GL_LINEAR_MIPMAP_LINEAR : GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		
		// this->Unbind();
	}

	Texture2D::~Texture2D() {
		glDeleteTextures(1, &this->ID);
	}

	std::unique_ptr<Nexus::Texture2D> Texture2D::CreateFromFile(const char* path, bool flip_vertically) {
		
		std::unique_ptr<Nexus::Texture2D> result = nullptr;
		stbi_set_flip_vertically_on_load(flip_vertically);

		int width, height, channel_count;
		stbi_uc* data = stbi_load(path, &width, &height, &channel_count, 0);
		if (data) {
			GLenum internal_format(-1);
			GLenum format(-1);

			switch (channel_count) {
				case 1:
					internal_format = GL_R8;
					format = GL_RED;
					break;
				case 3:
					internal_format = GL_RGB8;
					format = GL_RGB;
					break;
				case 4:
					internal_format = GL_RGBA8;
					format = GL_RGBA;
					break;
				default:
					Logger::Message(LOG_ERROR, "The Images File format not supported yet!, Path: " + std::string(path));
			}

			result = std::make_unique<Nexus::Texture2D>(width, height, internal_format, format, GL_UNSIGNED_BYTE, data);
			// Logger::Message(LOG_DEBUG, "The Image loads successfully, path: " + std::string(path));
			
		} else {
			Logger::Message(LOG_ERROR, "Failed to load image at path: " + std::string(path));
		}
		
		stbi_image_free(data);

		return result;
	}

	void Texture2D::SetWrappingParams(GLint wrap_s, GLint wrap_t) const {
		glTextureParameteri(this->ID, GL_TEXTURE_WRAP_S, wrap_s);
		glTextureParameteri(this->ID, GL_TEXTURE_WRAP_T, wrap_t);
	}

	void Texture2D::SetFilterParams(GLint min_filter, GLint max_filter) const {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min_filter);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, max_filter);
	}

	void Texture2D::BindImage(GLuint unit, GLenum access, GLenum format) const {
		glBindImageTexture(unit, this->ID, 0, GL_FALSE, 0, access, format);
	}
}