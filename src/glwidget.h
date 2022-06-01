#ifndef GLVERTWIDGET_H
#define GLVERTWIDGET_H
#include "GL/glew.h"
#ifdef __APPLE__
#include <glu.h>
#else
#include "GL/glu.h"
#endif

#include <memory>
#include <QGLWidget>
#include <QTimer>
#include <QTime>

#include "glm/glm.hpp"            // glm::vec*, mat*, and basic glm functions
#include "glm/gtx/transform.hpp"  // glm::translate, scale, rotate
#include "glm/gtc/type_ptr.hpp"   // glm::value_ptr

#include "gl/datatype/FBO.h"

class OpenGLShape;

using namespace CS123::GL;

class GLWidget : public QGLWidget {
    Q_OBJECT

public:
    GLWidget(QGLFormat format, QWidget *parent = 0);
    ~GLWidget();

    void settingsChanged();
    bool addMetaball();
    bool removeMetaball();

protected:
    /** This is called once, before any calls to paintGL. */
    void initializeGL();

    /** This is called every time the window needs to be repainted. */
    void paintGL();

    void resizeGL(int w, int h);

protected slots:
    /** Repaints the canvas. Called 60 times per second by m_timer. */
    void tick();

private:
    int m_width;
    int m_height;

    std::unique_ptr<OpenGLShape> m_quad;

    /** ID for the shader program. */
    GLuint m_program;
    GLuint m_hdrProgram;
    GLuint m_blurProgram;
    GLuint m_dofProgram;

    /** FBO for HDR colors. */
    std::unique_ptr<FBO> m_hdrFBO;

    /** FBOs for Bloom blurring */
    FBOPtr m_blurFBO1, m_blurFBO2;
    FBOPtr m_dfoFBO1, m_dfoFBO2;

    /** Timer calls tick() 60 times per second. */
    QTimer m_timer;
    float m_fps;
    QTime m_time;

    int m_numActiveBalls;
    float m_totalTime;
    float m_timeDelta;

    std::vector<glm::vec3> m_colors;
    std::vector<float> m_fields;
};

#endif // GLVERTWIDGET_H
