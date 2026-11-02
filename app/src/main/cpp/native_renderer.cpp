#include <jni.h>
#include <GLES3/gl3.h>

#include <algorithm>
#include <array>
#include <atomic>
#include <chrono>
#include <cmath>
#include <cstdio>
#include <string>
#include <mutex>
#include <vector>

namespace {
struct Vertex { GLfloat x, y, r, g, b; };
struct Enemy { float x, y, hp = 3.0F; bool alive = true; float contact = 0.0F; };

GLuint program = 0, vao = 0, vbo = 0;
std::mutex game_mutex;
GLint color_location = -1;
std::atomic<float> input_x{0.0F}, input_y{0.0F};
std::atomic<bool> joystick_active{false}, attack_requested{false};
std::atomic<float> joystick_origin_x{0.18F}, joystick_origin_y{0.78F};
std::array<Enemy, 4> enemies{};
float player_x = 0.0F, player_y = 0.0F;
float player_hp = 100.0F, attack_flash = 0.0F, attack_cooldown = 0.0F;
int kills = 0, wave = 1;
std::chrono::steady_clock::time_point previous_frame = std::chrono::steady_clock::now();

GLuint compile_shader(GLenum type, const char* source) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);
    GLint ok = GL_FALSE;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &ok);
    if (ok != GL_TRUE) { glDeleteShader(shader); return 0; }
    return shader;
}

void add_triangle(std::vector<Vertex>& out, float ax, float ay, float bx, float by,
                  float cx, float cy, float r, float g, float b) {
    out.push_back({ax, ay, r, g, b});
    out.push_back({bx, by, r, g, b});
    out.push_back({cx, cy, r, g, b});
}
void add_rect(std::vector<Vertex>& out, float x, float y, float w, float h,
              float r, float g, float b) {
    add_triangle(out,x,y,x+w,y,x+w,y+h,r,g,b);
    add_triangle(out,x,y,x+w,y+h,x,y+h,r,g,b);
}
void add_circle(std::vector<Vertex>& out, float cx, float cy, float radius,
                float r, float g, float b) {
    constexpr int segments = 24;
    for (int i=0; i<segments; ++i) {
        const float a0 = 6.2831853F * static_cast<float>(i) / segments;
        const float a1 = 6.2831853F * static_cast<float>(i+1) / segments;
        add_triangle(out,cx,cy,cx+std::cos(a0)*radius,cy+std::sin(a0)*radius,
                     cx+std::cos(a1)*radius,cy+std::sin(a1)*radius,r,g,b);
    }
}
void add_bar(std::vector<Vertex>& out, float x, float y, float w, float h,
             float value, float r, float g, float b) {
    add_rect(out,x,y,w,h,0.16F,0.13F,0.21F);
    add_rect(out,x,y,w*std::clamp(value,0.0F,1.0F),h,r,g,b);
}
void spawn_wave() {
    const float positions[4][2] = {{-0.72F,0.48F},{0.72F,0.42F},{-0.68F,-0.48F},{0.68F,-0.45F}};
    for (std::size_t i=0; i<enemies.size(); ++i) {
        enemies[i] = {positions[i][0],positions[i][1],2.0F+0.5F*static_cast<float>(wave-1),true,0.0F};
    }
}
void update_game(float dt) {
    const float dx = joystick_active.load() ? input_x.load() : 0.0F;
    const float dy = joystick_active.load() ? input_y.load() : 0.0F;
    player_x = std::clamp(player_x + dx * 1.35F * dt,-0.90F,0.90F);
    player_y = std::clamp(player_y - dy * 1.35F * dt,-0.82F,0.82F);
    attack_cooldown = std::max(0.0F,attack_cooldown-dt);
    attack_flash = std::max(0.0F,attack_flash-dt);
    if (attack_requested.exchange(false) && attack_cooldown <= 0.0F) {
        attack_cooldown = 0.34F;
        attack_flash = 0.16F;
        for (auto& e : enemies) {
            if (!e.alive) continue;
            const float ex=e.x-player_x, ey=e.y-player_y;
            if (ex*ex+ey*ey < 0.34F*0.34F) {
                e.hp -= 1.0F;
                if (e.hp <= 0.0F) { e.alive=false; ++kills; }
            }
        }
    }
    for (auto& e : enemies) {
        if (!e.alive) continue;
        const float dxp=player_x-e.x, dyp=player_y-e.y;
        const float dist=std::sqrt(dxp*dxp+dyp*dyp);
        if (dist > 0.13F) {
            e.x += (dxp/dist)*0.22F*dt;
            e.y += (dyp/dist)*0.22F*dt;
        }
        e.contact=std::max(0.0F,e.contact-dt);
        if (dist < 0.12F && e.contact <= 0.0F) {
            player_hp=std::max(0.0F,player_hp-8.0F);
            e.contact=0.85F;
        }
    }
    if (player_hp <= 0.0F) {
        player_hp=100.0F;
        player_x=0.0F; player_y=0.0F;
        for (auto& e: enemies) e.alive=false;
        spawn_wave();
    }
    const bool any_alive=std::any_of(enemies.begin(),enemies.end(),[](const Enemy& e){return e.alive;});
    if (!any_alive) { ++wave; spawn_wave(); }
}
void release_gl_resources() {
    if (vbo) { glDeleteBuffers(1,&vbo); vbo=0; }
    if (vao) { glDeleteVertexArrays(1,&vao); vao=0; }
    if (program) { glDeleteProgram(program); program=0; }
}
} // namespace

extern "C" JNIEXPORT jstring JNICALL
Java_com_curseddominion_game_MainActivity_nativeBuildLabel(JNIEnv* env, jobject) {
    return env->NewStringUTF("C++20 • OpenGL ES • Offline");
}
extern "C" JNIEXPORT jstring JNICALL
Java_com_curseddominion_game_MainActivity_nativeStatus(JNIEnv* env, jobject) {
    std::lock_guard<std::mutex> lock(game_mutex);
    char text[96];
    std::snprintf(text,sizeof(text),"HP %.0f%%   WAVE %d   KILLS %d",player_hp,wave,kills);
    return env->NewStringUTF(text);
}
extern "C" JNIEXPORT void JNICALL
Java_com_curseddominion_game_MainActivity_nativeTouch(JNIEnv*, jobject,jfloat x,jfloat y,jint action) {
    if (action==0) {
        if (x<0.48F) {
            joystick_origin_x.store(x); joystick_origin_y.store(y);
            joystick_active.store(true);
            input_x.store(0.0F); input_y.store(0.0F);
        } else if (x>0.58F && y>0.42F) {
            attack_requested.store(true);
        }
    } else if (action==2 && joystick_active.load()) {
        input_x.store(std::clamp((x-joystick_origin_x.load())*3.2F,-1.0F,1.0F));
        input_y.store(std::clamp((y-joystick_origin_y.load())*3.2F,-1.0F,1.0F));
    } else if (action==1) {
        joystick_active.store(false); input_x.store(0.0F); input_y.store(0.0F);
    }
}
extern "C" JNIEXPORT void JNICALL
Java_com_curseddominion_game_MainActivity_nativeSurfaceCreated(JNIEnv*, jobject) {
    std::lock_guard<std::mutex> lock(game_mutex);
    release_gl_resources();
    constexpr char vs[] =
        "#version 300 es\nlayout(location=0) in vec2 p; layout(location=1) in vec3 c;"
        "out vec3 tint; void main(){gl_Position=vec4(p,0.0,1.0);tint=c;}";
    constexpr char fs[] =
        "#version 300 es\nprecision mediump float; in vec3 tint; out vec4 color;"
        "void main(){color=vec4(tint,1.0);}";
    GLuint v=compile_shader(GL_VERTEX_SHADER,vs), f=compile_shader(GL_FRAGMENT_SHADER,fs);
    if (!v || !f) { if(v)glDeleteShader(v); if(f)glDeleteShader(f); return; }
    program=glCreateProgram(); glAttachShader(program,v); glAttachShader(program,f); glLinkProgram(program);
    glDeleteShader(v); glDeleteShader(f);
    GLint linked=GL_FALSE; glGetProgramiv(program,GL_LINK_STATUS,&linked);
    if(linked!=GL_TRUE){glDeleteProgram(program);program=0;return;}
    glGenVertexArrays(1,&vao); glGenBuffers(1,&vbo);
    glBindVertexArray(vao); glBindBuffer(GL_ARRAY_BUFFER,vbo);
    glEnableVertexAttribArray(0); glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,sizeof(Vertex),nullptr);
    glEnableVertexAttribArray(1); glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,sizeof(Vertex),
        reinterpret_cast<void*>(2*sizeof(GLfloat)));
    glBindVertexArray(0);
    glClearColor(0.035F,0.025F,0.065F,1.0F);
    player_x=0.0F; player_y=0.0F; player_hp=100.0F; kills=0; wave=1;
    spawn_wave(); previous_frame=std::chrono::steady_clock::now();
}
extern "C" JNIEXPORT void JNICALL
Java_com_curseddominion_game_MainActivity_nativeSurfaceChanged(JNIEnv*,jobject,jint w,jint h) {
    glViewport(0,0,w,h);
}
extern "C" JNIEXPORT void JNICALL
Java_com_curseddominion_game_MainActivity_nativeDrawFrame(JNIEnv*,jobject) {
    std::lock_guard<std::mutex> lock(game_mutex);
    const auto now=std::chrono::steady_clock::now();
    float dt=std::chrono::duration<float>(now-previous_frame).count();
    previous_frame=now; dt=std::clamp(dt,0.0F,0.05F);
    update_game(dt);
    glClear(GL_COLOR_BUFFER_BIT);
    if(!program||!vao)return;
    std::vector<Vertex> vertices; vertices.reserve(1800);
    add_rect(vertices,-1.0F,-1.0F,2.0F,2.0F,0.045F,0.032F,0.075F);
    for(int i=0;i<11;++i) {
        const float x=-1.0F+static_cast<float>(i)*0.2F;
        add_rect(vertices,x,-0.92F,0.003F,1.84F,0.09F,0.065F,0.13F);
        const float y=-0.9F+static_cast<float>(i)*0.18F;
        add_rect(vertices,-0.98F,y,1.96F,0.003F,0.09F,0.065F,0.13F);
    }
    add_bar(vertices,-0.94F,0.82F,0.34F,0.045F,player_hp/100.0F,0.18F,0.82F,0.48F);
    add_bar(vertices,-0.94F,0.75F,0.34F,0.025F,1.0F-attack_cooldown/0.34F,0.50F,0.28F,0.92F);
    for(const auto& e:enemies) if(e.alive) {
        add_circle(vertices,e.x,e.y,0.075F,0.55F,0.12F,0.20F);
        add_triangle(vertices,e.x,e.y+0.07F,e.x-0.07F,e.y-0.055F,e.x+0.07F,e.y-0.055F,
                     0.95F,0.24F,0.30F);
        add_bar(vertices,e.x-0.055F,e.y+0.09F,0.11F,0.012F,e.hp/(2.0F+0.5F*static_cast<float>(wave-1)),
                0.95F,0.25F,0.30F);
    }
    if(attack_flash>0.0F) add_circle(vertices,player_x,player_y,0.28F,0.70F,0.38F,1.0F);
    add_circle(vertices,player_x,player_y,0.09F,0.15F,0.85F,0.95F);
    add_triangle(vertices,player_x,player_y+0.11F,player_x-0.075F,player_y-0.07F,
                 player_x+0.075F,player_y-0.07F,0.82F,0.95F,1.0F);
    const float bx=joystick_active.load()?joystick_origin_x.load():0.18F;
    const float by=joystick_active.load()?1.0F-2.0F*joystick_origin_y.load():-0.70F;
    add_circle(vertices,bx*2.0F-1.0F,by,0.19F,0.18F,0.14F,0.25F);
    add_circle(vertices,bx*2.0F-1.0F+input_x.load()*0.10F,by-input_y.load()*0.10F,
               0.075F,0.50F,0.40F,0.68F);
    add_circle(vertices,0.76F,-0.70F,0.19F,0.36F,0.13F,0.48F);
    add_circle(vertices,0.76F,-0.70F,0.12F,0.80F,0.48F,1.0F);
    glUseProgram(program); glBindVertexArray(vao); glBindBuffer(GL_ARRAY_BUFFER,vbo);
    glBufferData(GL_ARRAY_BUFFER,static_cast<GLsizeiptr>(vertices.size()*sizeof(Vertex)),
                 vertices.data(),GL_DYNAMIC_DRAW);
    glDrawArrays(GL_TRIANGLES,0,static_cast<GLsizei>(vertices.size()));
    glBindVertexArray(0);
}
