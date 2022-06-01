#include "settings.h"
#include <QFile>
#include <QSettings>

Settings settings;


/**
  Loads the application settings, or, if no saved settings are available,
  loads default values for the settings. You can change the defaults here.
**/
void Settings::loadSettingsOrDefaults() {
    // Set the default values below
    QSettings s("CSCI1230, Final Project");
    floatSlider1 = s.value("floatSlider1", 75.0f).toFloat();
    floatSlider2 = s.value("floatSlider2", 75.0f).toFloat();
    floatSlider3 = s.value("floatSlider3", 75.0f).toFloat();

    ballSlider1 = s.value("ballSlider1", 2.5f).toFloat();
    ballSlider2 = s.value("ballSlider2", 1.5f).toFloat();
    ballSlider3 = s.value("ballSlider3", 2.0f).toFloat();


    ball1Color = s.value("ball1Color", QColor(0, 170, 255)).value<QColor>();
    ball2Color = s.value("ball2Color", QColor(255, 100, 230)).value<QColor>();
    ball3Color = s.value("ball3Color", QColor(170, 255, 0)).value<QColor>();


    useHDR = s.value("useHDR", true).toBool();
    useGammaCorrection = s.value("useGammaCorrection", false).toBool();
    useExposure = s.value("useExposure", true).toBool();
    exposure = s.value("exposure", 1.1f).toFloat();
    useBloom = s.value("useBloom", true).toBool();
    useDOF = s.value("useDOF", false).toBool();
    useReflections = s.value("useReflections", true).toBool();
//    lightColor = s.value("lightColor", QColor(255, 255, 255)).value<QColor>();
//    lightIntensity = s.value("lightIntensity", 5.f).toFloat();
}

void Settings::saveSettings() {
    QSettings s("CSCI1230, Final Project");
}
