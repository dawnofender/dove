#include "openGLRenderer.hpp"

// CURRENTLY UNUSED

void openGLRenderer::initialize() {
  if (!glfwInit()) {
    fprintf(stderr, "Failed to initialize GLFW\n");
    return -1;
  }

  // glfwWindowHint(GLFW_SAMPLES, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT,
                 GL_TRUE); // To make MacOS happy; should not be needed
  glfwWindowHint(GLFW_OPENGL_PROFILE,
                 GLFW_OPENGL_CORE_PROFILE); // We don't want the old OpenGL
  glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);  


  window = glfwCreateWindow(1024, 768, "dream", NULL, NULL);
  if (window == NULL) {
    fprintf(stderr,
            "Failed to open GLFW window. If you have an Intel GPU, they are "
            "not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
    glfwTerminate();
    return -1;
  }

  glfwMakeContextCurrent(window);
  glewExperimental = true;
  if (glewInit() != GLEW_OK) {
    fprintf(stderr, "Failed to initialize GLEW\n");
    return -1;
  }

  glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  glfwPollEvents();
  glfwSetCursorPos(window, 1024 / 2, 768 / 2);
  glClearColor(0.6f, 0.7f, 0.9f, 0.0f);
  glEnable(GL_DEPTH_TEST); // Enable depth test
  glDepthFunc(GL_LESS);   // Accept fragment if it closer to the camera than the
                          // former one
  glEnable(GL_CULL_FACE); // backface culling

}

void openGLRenderer::stop() {
  glfwTerminate();
}
