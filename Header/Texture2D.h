#pragma once

#include <glad/glad.h>
#include <memory>

namespace Nexus {
	class Texture2D {
	public:
		GLuint ID = GLuint(-1);
		unsigned int Width;
		unsigned int Height;
		GLint InternalFormat;
		
		Texture2D(unsigned int width, unsigned int height, GLint internal_format, GLenum format, GLenum type, 
			const GLvoid* data = 0, bool enable_mipmap = true);

		~Texture2D();
		
		static std::unique_ptr<Nexus::Texture2D> CreateFromFile(const char* path, bool flip_vertically = false);
		
		inline void Bind(const unsigned int unit = 0) const {
			glActiveTexture(GL_TEXTURE0 + unit);
			glBindTexture(GL_TEXTURE_2D, this->ID);
		}

		inline void Unbind() const {
			glBindTexture(GL_TEXTURE_2D, 0);
		}

		void SetWrappingParams(GLint wrap_s, GLint wrap_t) const;
		void SetFilterParams(GLint min_filter, GLint max_filter) const;
		
		void BindImage(GLuint unit, GLenum access, GLenum format) const;
		
	private:	
		
	};	
}
