#ifndef FBO_HPP_
#define FBO_HPP_

#include <GL/glew.h>

class Fbo
{
public:
  Fbo();

  bool    create(GLuint width, GLuint height, GLuint count);
  bool    enable();
  bool    disable();
  void    reset();
  GLuint  getDepthTextureHandle() { return depth_handle; }
  GLuint  getHandle() { return fbo_handle; }
  GLuint  getTextureCount() { return texture_count; }
  GLuint* getTextureHandles() { return texture_handles; }

protected:
  bool initializeFBO(bool useDepthBuffer = true );
  bool generateTexture(GLuint width, GLuint height, GLuint *handle);

  GLuint  fbo_handle;         // FBO handle
  GLuint  depth_handle;       // Depth buffer ID
  GLuint  width, height;      // Width and height of buffers
  bool    enabled;            // Whether or not the FBO is enabled
  GLuint  texture_count;	  // Amount of textures attached to the FBO
  GLuint* texture_handles;	  // Texture handles for each texture
};


#endif /* FBO_HPP_ */
