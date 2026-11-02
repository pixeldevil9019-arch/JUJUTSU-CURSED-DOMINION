#include <jni.h>
#include <GLES3/gl3.h>

namespace {
GLuint program = 0;
GLuint vao = 0;
GLuint vbo = 0;

GLuint compile_shader(GLenum type, const char* source) {
    const GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);
    GLint compiled = GL_FALSE;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
    if (compiled != GL_TRUE) {
        glDeleteShader(shader);
        return 0;
    }
    return shader;
}

void release_gl_resources() {
    if (vbo != 0) { glDeleteBuffers(1, &vbo); vbo = 0; }
    if (vao != 0) { glDeleteVertexArrays(1, &vao); vao = 0; }
    if (program != 0) { glDeleteProgram(program); program = 0; }
}
}  // namespace

extern "C" JNIEXPORT jstring JNICALL
Java_com_curseddominion_game_MainActivity_nativeBuildLabel(JNIEnv* env, jobject) {
    return env->NewStringUTF("Native C++20 • OpenGL ES 3.0");
}

extern "C" JNIEXPORT void JNICALL
Java_com_curseddominion_game_MainActivity_nativeSurfaceCreated(JNIEnv*, jobject) {
    release_gl_resources();
    constexpr char vertex_source[] =
        "#version 300 es\n"
        "layout(location=0) in vec2 position;\n"
        "void main(){ gl_Position=vec4(position,0.0,1.0); }";
    constexpr char fragment_source[] =
        "#version 300 es\n"
        "precision mediump float;\n"
        "out vec4 fragment_color;\n"
        "void main(){ fragment_color=vec4(0.36,0.12,0.62,1.0); }";

    const GLuint vertex = compile_shader(GL_VERTEX_SHADER, vertex_source);
    const GLuint fragment = compile_shader(GL_FRAGMENT_SHADER, fragment_source);
    if (vertex == 0 || fragment == 0) {
        if (vertex != 0) glDeleteShader(vertex);
        if (fragment != 0) glDeleteShader(fragment);
        return;
    }

    program = glCreateProgram();
    glAttachShader(program, vertex);
    glAttachShader(program, fragment);
    glLinkProgram(program);
    glDeleteShader(vertex);
    glDeleteShader(fragment);
    GLint linked = GL_FALSE;
    glGetProgramiv(program, GL_LINK_STATUS, &linked);
    if (linked != GL_TRUE) {
        glDeleteProgram(program);
        program = 0;
        return;
    }

    constexpr GLfloat vertices[] = {
        0.0F, 0.58F, -0.55F, -0.42F, 0.55F, -0.42F
    };
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE,
                          2 * static_cast<GLsizei>(sizeof(GLfloat)), nullptr);
    glBindVertexArray(0);
    glClearColor(0.025F, 0.018F, 0.045F, 1.0F);
}

extern "C" JNIEXPORT void JNICALL
Java_com_curseddominion_game_MainActivity_nativeSurfaceChanged(
    JNIEnv*, jobject, jint width, jint height) {
    glViewport(0, 0, width, height);
}

extern "C" JNIEXPORT void JNICALL
Java_com_curseddominion_game_MainActivity_nativeDrawFrame(JNIEnv*, jobject) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    if (program == 0 || vao == 0) return;
    glUseProgram(program);
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glBindVertexArray(0);
}
