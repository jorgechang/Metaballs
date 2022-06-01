#include <QDebug>
#include "glwidget.h"
#include "settings.h"
#include "openglshape.h"
#include "gl/shaders/ShaderAttribLocations.h"
#include "gl/textures/Texture2D.h"
#include "cs123_lib/resourceloader.h"

#define GLM_FORCE_RADIANS
#include "glm/glm.hpp"            // glm::vec*, mat*, and basic glm functions
#include "glm/gtx/transform.hpp"  // glm::translate, scale, rotate
#include "glm/gtc/type_ptr.hpp"   // glm::value_ptr

#include <string>
#include <iostream>
#include "glm/gtx/string_cast.hpp"

using namespace std;
const int MAX_NUM_BALLS = 7;
const int MIN_NUM_BALLS = 3;

inline int fieldUniformLoc(unsigned int program, int index)
{
    auto f = "balls[" + std::to_string(index) + "].f";
    return glGetUniformLocation(program, f.c_str());
}

inline int positionUniformLoc(unsigned int program, int index)
{
    auto p = "balls[" + std::to_string(index) + "].pos";
    return glGetUniformLocation(program, p.c_str());
}

inline int colorUniformLoc(unsigned int program, int index)
{
    auto p = "balls[" + std::to_string(index) + "].color";
    return glGetUniformLocation(program, p.c_str());
}

GLWidget::GLWidget(QGLFormat format, QWidget *parent)
    : QGLWidget(format, parent),
      m_width(width()),
      m_height(height()),
      m_quad(nullptr),
      m_program(0),
      m_hdrProgram(0),
      m_blurProgram(0),
      m_dofProgram(0),
      m_hdrFBO(nullptr),
      m_blurFBO1(nullptr),
      m_blurFBO2(nullptr),
      m_dfoFBO1(nullptr),
      m_dfoFBO2(nullptr),
      m_timer(this),
      m_fps(60.0f),
      m_time(),
      m_numActiveBalls(1),
      m_totalTime(0.0f),
      m_timeDelta(0.0f),
      m_colors({
        glm::vec3(0.0, 0.0, 1.0),
        glm::vec3(1.0, 0.0, 0.0),
        glm::vec3(0.2, 0.85, 0.0),
    }),
      m_fields({3.7f, 1.5f, 2.0f})

{
    // Set up 60 FPS draw loop.
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(tick()));
    //    m_time.start();
    m_timer.start(1000.0f / m_fps);
}

GLWidget::~GLWidget()
{

}


void GLWidget::initializeGL() {
    ResourceLoader::initializeGlew();
    m_time.start();

    // Set the color to set the screen when the color buffer is cleared.scale
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    // Creates the shader program that will be used for drawing.
    m_program = ResourceLoader::createShaderProgram(":/shaders/shader.vert", ":/shaders/shader.frag");
    m_hdrProgram = ResourceLoader::createShaderProgram(":/shaders/hdr.vert", ":/shaders/hdr.frag");
    // Re-use hdr.vert since it's a basic pass-through vertex shader
    m_blurProgram = ResourceLoader::createShaderProgram(":/shaders/hdr.vert", ":/shaders/blur.frag");
    m_dofProgram = ResourceLoader::createShaderProgram(":/shaders/hdr.vert", ":/shaders/dof.frag");

    std::vector<GLfloat> quadData = {-1,  1, 0,
                                     0,  1,
                                     -1, -1, 0,
                                     0,  0,
                                     1,  1, 0,
                                     1,  1,
                                     1, -1, 0,
                                     1,  0 };

    m_quad = std::make_unique<OpenGLShape>();
    m_quad->setVertexData(&quadData[0], quadData.size(), VBO::GEOMETRY_LAYOUT::LAYOUT_TRIANGLE_STRIP, 4);
    m_quad->setAttribute(ShaderAttrib::POSITION, 3, 0, VBOAttribMarker::DATA_TYPE::FLOAT, false);
    m_quad->setAttribute(ShaderAttrib::TEXCOORD0, 2, 3*sizeof(GLfloat), VBOAttribMarker::DATA_TYPE::FLOAT, false);
    m_quad->buildVAO();

    //    glEnable(GL_DEPTH_TEST);
    //    glEnable(GL_CULL_FACE);
    //    glCullFace(GL_BACK);
    //    glFrontFace(GL_CCW);


}

void GLWidget::paintGL() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Initialize uniforms for HDR
    glUseProgram(m_hdrProgram);
    glUniform1i(glGetUniformLocation(m_hdrProgram, "hdrScene"), 0);
    glUniform1i(glGetUniformLocation(m_hdrProgram, "bloomBlur"), 1);
    glUniform1i(glGetUniformLocation(m_hdrProgram, "useGammaCorrection"), int(settings.useGammaCorrection));
    glUniform1i(glGetUniformLocation(m_hdrProgram, "useHDR"), int(settings.useHDR));
    glUniform1i(glGetUniformLocation(m_hdrProgram, "useExposure"), int(settings.useExposure));
    // Can play with this value for best appearance -> maybe a UI slider?
    glUniform1f(glGetUniformLocation(m_hdrProgram, "exposure"), settings.exposure);

    // Initialize uniforms for Bloom blur
    glUseProgram(m_blurProgram);
    glUniform1i(glGetUniformLocation(m_blurProgram, "scene"), 0);

    m_hdrFBO->bind();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(m_program);
    glViewport(0, 0, m_width, m_height);
    glUniform1f(glGetUniformLocation(m_program, "timeElapsed"), m_totalTime);
    glUniform1f(glGetUniformLocation(m_program, "timeDelta"), m_timeDelta);
    glUniform3fv(glGetUniformLocation(m_program, "resolution"), 1, glm::value_ptr(glm::vec3(m_width, m_height, 0)));
    glUniform3fv(glGetUniformLocation(m_program, "metaballBaseTintColor"), 1, glm::value_ptr(glm::vec3(0.0, 0.0, 1.0)));
    glUniform1i(glGetUniformLocation(m_program, "useReflections"), settings.useReflections);

    // Setup lights
    int nLights = 1;
    glUniform1i(glGetUniformLocation(m_program, "nLights"), nLights);

    std::vector<glm::vec3> lightDirections = {
        glm::vec3(1.0, 0.6, 0.5),
//        glm::vec3((settings.floatSlider1*2/1.0)-1.0f, (settings.floatSlider2*2/1.0)-1.0f, (settings.floatSlider3*2/1.0)-1.0f),
    };
    glUniform3fv(glGetUniformLocation(m_program, "lightDirections"), nLights, glm::value_ptr(lightDirections[0]));
    std::vector<glm::vec3>lightColors = {
        glm::vec3(1.0, 1.0, 1.0),
    };
    glUniform3fv(glGetUniformLocation(m_program, "lightColors"), nLights, glm::value_ptr(lightColors[0]));

    // Hard-coding values for now. Should be more dynamic in the future.
    int num_balls = min(m_numActiveBalls, MAX_NUM_BALLS);
    std::vector<glm::vec3> positions = {
        glm::vec3(cos(m_totalTime), 1.0, 1.0),
        glm::vec3(sin(m_totalTime)+2, 1.0, 1.0),
        glm::vec3(sin(m_totalTime)-2, 2.0, cos(m_totalTime)),
        glm::vec3(sin(m_totalTime)+2, 0.5f*cos(m_totalTime)+3, 1.5),
        glm::vec3(cos(m_totalTime)-2, 0.5f*sin(m_totalTime)+3, 1.5),
        glm::vec3(0.5, 3.0, sin(m_totalTime/2)*10 - 3), //Two new balls oscillating in Z Direction
        glm::vec3(-0.5, 3.0, cos(m_totalTime/2)*10 - 3),
    };


    if (num_balls > m_colors.size()){
        m_colors.push_back(glm::vec3(settings.ball1Color.redF(), settings.ball1Color.greenF(), settings.ball1Color.blueF()));
        m_fields.push_back(settings.ballSlider1);
    }
    else if( num_balls >= MIN_NUM_BALLS && num_balls < m_colors.size()){
        m_colors.pop_back();
        m_fields.pop_back();
    }



    glUniform1i(glGetUniformLocation(m_program, "nBalls"), num_balls);

    for (int i = 0; i< num_balls; i++) {
        glUniform1f(fieldUniformLoc(m_program, i), m_fields[i]);
        glUniform3fv(positionUniformLoc(m_program, i), 1, glm::value_ptr(positions[i]));
        glUniform3fv(colorUniformLoc(m_program, i), 1, glm::value_ptr(m_colors[i]));
    }

    m_quad->draw();

    m_hdrFBO->unbind();

    // Perform DOF first
    // creating two FBO for DOF. Probably order is important.
    // Perform DOF field first , integrate it with HDR image then integrate the BLOOM.
    glUseProgram(m_dofProgram);
    bool horizontal = false;
    bool initial = true;
    int iterations = 2;
    FBOPtr dfoFBOs[2] = {m_dfoFBO1, m_dfoFBO2};
    int horizontalUniformLoc = glGetUniformLocation(m_dofProgram, "horizontal");
    for (int i = 0; i < iterations; i++) {
        dfoFBOs[i]->bind();
        glUniform1i(horizontalUniformLoc, horizontal);
        glActiveTexture(GL_TEXTURE0);
        if (initial) {

            m_hdrFBO->getColorAttachment(0).bind(); //IF first pass use the HDR Image
        } else {
            dfoFBOs[!horizontal]->getColorAttachment(0).bind(); // If second pass use the first FBO.
        }

        m_quad->draw();
        dfoFBOs[i]->unbind();
        horizontal = !horizontal;
        if (initial) {
            initial = false;
        }
    }

    glUseProgram(0);

    // Perform iterative Gaussian blur for Bloom
    FBOPtr blurFBOs[2] = {m_blurFBO1, m_blurFBO2};
    glUseProgram(m_blurProgram);
    horizontal = false;
    initial = true;
    iterations = 50;
    horizontalUniformLoc = glGetUniformLocation(m_blurProgram, "horizontal");
    for (int i = 0; i < iterations; i++) {
        blurFBOs[horizontal]->bind();
        glActiveTexture(GL_TEXTURE0);
        glUniform1i(horizontalUniformLoc, horizontal);
        if (initial) {
            // Use the bright color attachment on the first iteration
            m_hdrFBO->getColorAttachment(1).bind();
        } else {
            blurFBOs[!horizontal]->getColorAttachment(0).bind();
        }
        m_quad->draw();
        blurFBOs[horizontal]->unbind();
        horizontal = !horizontal;
        if (initial) {
            initial = false;
        }
    }

    glUseProgram(m_hdrProgram);
    glUniform1i(glGetUniformLocation(m_hdrProgram, "useDOF"), int(settings.useDOF));
    glUniform1i(glGetUniformLocation(m_hdrProgram, "useBloom"), int(settings.useBloom));
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glViewport(0, 0, m_width, m_height);

    glActiveTexture(GL_TEXTURE0);
    m_hdrFBO->getColorAttachment(0).bind();
    glActiveTexture(GL_TEXTURE1);
    m_blurFBO2->getColorAttachment(0).bind();
    glActiveTexture(GL_TEXTURE2);
    m_dfoFBO2->getColorAttachment(0).bind();
    glActiveTexture(GL_TEXTURE3);
    m_hdrFBO->getColorAttachment(2).bind();

    glUniform1i(glGetUniformLocation(m_hdrProgram, "hdrscene"), 0); // set it manually
    glUniform1i(glGetUniformLocation(m_hdrProgram, "bloomscene"), 1); // set it manually
    glUniform1i(glGetUniformLocation(m_hdrProgram, "dofscene"), 2); // set it manually
    glUniform1i(glGetUniformLocation(m_hdrProgram, "normalizeddistance"), 3); // set it manually

    m_quad->draw();

    glUseProgram(0);
}



/** Repaints the canvas. Called 60 times per second. */
void GLWidget::tick()
{
    // Get the number of seconds since the last tick (variable update rate)
    m_timeDelta = m_time.restart() * 0.001f;
    m_totalTime += m_timeDelta;
    update();
}

void GLWidget::settingsChanged()
{
}

bool GLWidget::addMetaball()
{
    m_numActiveBalls = min(m_numActiveBalls+1, MAX_NUM_BALLS);
    return m_numActiveBalls == MAX_NUM_BALLS;
}

bool GLWidget::removeMetaball()
{
    m_numActiveBalls = max(m_numActiveBalls-1, 1);
    return m_numActiveBalls != MAX_NUM_BALLS;
}

void GLWidget::resizeGL(int w, int h) {
    m_width = w;
    m_height = h;

    glViewport(0, 0, w, h);

    m_hdrFBO = std::make_unique<FBO>(
                3, FBO::DEPTH_STENCIL_ATTACHMENT::NONE, w, h,
                TextureParameters::WRAP_METHOD::CLAMP_TO_EDGE,
                TextureParameters::FILTER_METHOD::LINEAR,
                GL_FLOAT);

    m_blurFBO1 = std::make_shared<FBO>(
                1, FBO::DEPTH_STENCIL_ATTACHMENT::NONE, w, h,
                TextureParameters::WRAP_METHOD::CLAMP_TO_EDGE,
                TextureParameters::FILTER_METHOD::LINEAR,
                GL_FLOAT);
    m_blurFBO2 = std::make_shared<FBO>(
                1, FBO::DEPTH_STENCIL_ATTACHMENT::NONE, w, h,
                TextureParameters::WRAP_METHOD::CLAMP_TO_EDGE,
                TextureParameters::FILTER_METHOD::LINEAR,
                GL_FLOAT);

    m_dfoFBO1 = std::make_shared<FBO>(
                1, FBO::DEPTH_STENCIL_ATTACHMENT::NONE, w, h,
                TextureParameters::WRAP_METHOD::CLAMP_TO_EDGE,
                TextureParameters::FILTER_METHOD::LINEAR,
                GL_FLOAT);

    m_dfoFBO2 = std::make_shared<FBO>(
                1, FBO::DEPTH_STENCIL_ATTACHMENT::NONE, w, h,
                TextureParameters::WRAP_METHOD::CLAMP_TO_EDGE,
                TextureParameters::FILTER_METHOD::LINEAR,
                GL_FLOAT);
}
