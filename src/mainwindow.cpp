#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QSettings>
#include <assert.h>
#include "Databinding.h"
#include "settings.h"
#include <QColorDialog>
#include <iostream>
#include <QGridLayout>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    QGLFormat qglFormat;
    qglFormat.setVersion(4,0);
    qglFormat.setProfile(QGLFormat::CoreProfile);
    ui->setupUi(this);

    // canvas for vertex shaders
    QGridLayout *gridLayoutVert = new QGridLayout(ui->vertCanvas);
    m_Canvas = new GLWidget(qglFormat, this);
    m_Canvas->setMinimumSize(50, 50);
    gridLayoutVert->addWidget(m_Canvas, 0, 1);

    settings.loadSettingsOrDefaults();
    dataBind();

    // Restore the UI settings
    QSettings qtSettings("CSCI1230", "Final Project");
//    restoreGeometry(qtSettings.value("geometry").toByteArray());
//    restoreState(qtSettings.value("windowState").toByteArray());
}

MainWindow::~MainWindow()
{
    foreach (DataBinding *b, m_bindings) {
        delete b;
    }
    delete ui;
    delete m_Canvas;
}

void MainWindow::dataBind() {
//#define BIND(b) { DataBinding *_b = (b); m_bindings.push_back(_b); assert(connect(_b, SIGNAL(dataChanged()), this, SLOT(settingsChanged()))); }
#define BIND(b) { \
    DataBinding *_b = (b); \
    m_bindings.push_back(_b); \
    assert(connect(_b, SIGNAL(dataChanged()), this, SLOT(settingsChanged()))); \
}

//    BIND(FloatBinding::bindSliderAndTextbox(ui->floatSlider1, ui->Slider1Edit,
//                                            settings.floatSlider1,
//                                            0.f,    // minValue
//                                            100.f)); // maxValue

//    BIND(FloatBinding::bindSliderAndTextbox(ui->floatSlider2, ui->Slider2Edit,
//                                            settings.floatSlider2,
//                                            0.f,    // minValue
//                                            100.f)); // maxValue

//    BIND(FloatBinding::bindSliderAndTextbox(ui->floatSlider3, ui->Slider3Edit,
//                                            settings.floatSlider3,
//                                            0.f,    // minValue
//                                            100.f)); // maxValue

    BIND(FloatBinding::bindSliderAndTextbox(ui->ballSlider1, ui->ballSlider1Edit,
                                            settings.ballSlider1,
                                            1.f,    // minValue
                                            6.0f)); // maxValue
    /*BIND(FloatBinding::bindSliderAndTextbox(ui->ballSlider2, ui->ballSlider2Edit,
                                            settings.ballSlider2,
                                            1.f,    // minValue
                                            2.5f)); // maxValue
    BIND(FloatBinding::bindSliderAndTextbox(ui->ballSlider3, ui->ballSlider3Edit,
                                            settings.ballSlider3,
                                            1.f,    // minValue
                                            2.5f)); // maxValue*/

    //colors 3 balls
    BIND(ColorBinding::bindButtonAndTextboxes(ui->ball1ColorButton,
                                              ui->sphereLREdit, ui->sphereLGEdit, ui->sphereLBEdit,
                                              settings.ball1Color));

    /*BIND(ColorBinding::bindButtonAndTextboxes(ui->ball2ColorButton,
                                              ui->sphereMREdit, ui->sphereMGEdit, ui->sphereMBEdit,
                                              settings.ball2Color));

    BIND(ColorBinding::bindButtonAndTextboxes(ui->ball3ColorButton,
                                              ui->sphereRREdit, ui->sphereRGEdit, ui->sphereRBEdit,
                                              settings.ball3Color));*/

    BIND(BoolBinding::bindCheckbox(ui->HDRcheckBox, settings.useHDR))
    BIND(BoolBinding::bindCheckbox(ui->ExposureCheckBox, settings.useExposure));
    BIND(FloatBinding::bindTextbox(ui->ExposureTextbox, settings.exposure))
    BIND(BoolBinding::bindCheckbox(ui->UseGammacheckbox, settings.useGammaCorrection));
    BIND(BoolBinding::bindCheckbox(ui->BloomCheckBox, settings.useBloom));
    BIND(BoolBinding::bindCheckbox(ui->DofCheckBox, settings.useDOF));
    BIND(BoolBinding::bindCheckbox(ui->ReflectionsCheckBox, settings.useReflections));

#undef BIND
}

void MainWindow::addMetaballButtonClicked() {
    auto shouldDisable = m_Canvas->addMetaball();
    ui->AddMetaballButton->setEnabled(!shouldDisable);
}

void MainWindow::removeMetaballButtonClicked() {
    auto shouldEnable = m_Canvas->removeMetaball();
    ui->AddMetaballButton->setEnabled(shouldEnable);
}

void MainWindow::closeEvent(QCloseEvent *event) {
    // Save the settings before we quit
    settings.saveSettings();
    QSettings qtSettings("CSCI1230", "Final Project");
//    qtSettings.setValue("geometry", saveGeometry());
//    qtSettings.setValue("windowState", saveState());
    QMainWindow::closeEvent(event);
}

void MainWindow::settingsChanged() {
    m_Canvas->settingsChanged();
    m_Canvas->update();
}
