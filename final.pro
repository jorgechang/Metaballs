QT += core gui opengl

TARGET = final
TEMPLATE = app

QMAKE_CXXFLAGS += -std=c++14
QMAKE_LFLAGS += -no-pie
CONFIG += c++14

unix:!macx {
    LIBS += -lGLU
}
macx {
    QMAKE_CFLAGS_X86_64 += -mmacosx-version-min=10.7
    QMAKE_CXXFLAGS_X86_64 = $$QMAKE_CFLAGS_X86_64
    CONFIG += c++11
}
win32 {
    DEFINES += GLEW_STATIC
    LIBS += -lopengl32 -lglu32
}

INCLUDEPATH += src cs123_lib glm glew-1.10.0/include
DEPENDPATH += src cs123_lib glm glew-1.10.0/include

SOURCES += src/settings.cpp \
    src/Databinding.cpp \
    src/mainwindow.cpp \
    src/main.cpp \
    src/glwidget.cpp \
    src/openglshape.cpp \
    cs123_lib/resourceloader.cpp \
    cs123_lib/errorchecker.cpp \
    src/gl/datatype/VBO.cpp \
    src/gl/datatype/VBOAttribMarker.cpp \
    src/gl/datatype/VAO.cpp \
    src/gl/datatype/IBO.cpp \
    src/gl/datatype/FBO.cpp \
    src/gl/textures/Texture.cpp \
    src/gl/textures/Texture2D.cpp \
    src/gl/textures/TextureParameters.cpp \
    src/gl/textures/TextureParametersBuilder.cpp \
    src/gl/textures/RenderBuffer.cpp \
    src/gl/textures/DepthBuffer.cpp \
    src/gl/GLDebug.cpp \
    glew-1.10.0/src/glew.c


HEADERS += \
    src/Databinding.h \
    src/settings.h \
    src/mainwindow.h \
    src/glwidget.h \
    src/openglshape.h \
    cs123_lib/resourceloader.h \
    cs123_lib/errorchecker.h \
    cs123_lib/sphere.h \
    src/gl/datatype/VBO.h \
    src/gl/datatype/VBOAttribMarker.h \
    src/gl/datatype/VAO.h \
    src/gl/datatype/IBO.h \
    src/gl/datatype/FBO.h \
    src/gl/shaders/ShaderAttribLocations.h \
    src/gl/textures/Texture.h \
    src/gl/textures/Texture2D.h \
    src/gl/textures/TextureParameters.h \
    src/gl/textures/TextureParametersBuilder.h \
    src/gl/textures/RenderBuffer.h \
    src/gl/textures/DepthBuffer.h \
    src/gl/GLDebug.h \
    glew-1.10.0/include/GL/glew.h

FORMS += src/mainwindow.ui

OTHER_FILES +=

RESOURCES += \
    shaders/shaders.qrc

DISTFILES += \
    shaders/blur.frag \
    shaders/dof.frag \
    shaders/hdr.frag \
    shaders/hdr.vert \
    shaders/phong.frag \
    shaders/phong.vert \
    shaders/shader.frag \
    shaders/shader.vert
