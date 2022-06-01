#include "Databinding.h"
#include <math.h>
#include <QVariant>
#include <QColorDialog>

////////////////////////////////////////////////////////////////////////////////
// class IntBinding
////////////////////////////////////////////////////////////////////////////////

IntBinding* IntBinding::bindSliderAndTextbox(
        QSlider *slider, QLineEdit *textbox, int &value, int minValue, int maxValue)
{
    // Bind the slider, the textbox, and the value together
    IntBinding *binding = new IntBinding(value);
    connect(slider, SIGNAL(valueChanged(int)), binding, SLOT(intChanged(int)));
    connect(slider, SIGNAL(sliderMoved(int)), binding, SLOT(intChanged(int)));
    connect(textbox, SIGNAL(textChanged(QString)), binding, SLOT(stringChanged(QString)));
    connect(binding, SIGNAL(updateInt(int)), slider, SLOT(setValue(int)));
    connect(binding, SIGNAL(updateString(QString)), textbox, SLOT(setText(QString)));

    // Set the range and initial value
    slider->setMinimum(minValue);
    slider->setMaximum(maxValue);
    slider->setValue(value);
    textbox->setText(QString::number(value));

    return binding;
}

IntBinding* IntBinding::bindTextbox(QLineEdit *textbox, int &value) {
    // Bind the the textbox and the value together
    IntBinding *binding = new IntBinding(value);
    connect(textbox, SIGNAL(textChanged(QString)), binding, SLOT(stringChanged(QString)));

    // Set the initial value
    textbox->setText(QString::number(value));

    return binding;
}

void IntBinding::intChanged(int newValue) {
    if (m_value != newValue) {
        m_value = newValue;
        emit updateString(QString::number(m_value));
        emit dataChanged();
    }
}

void IntBinding::stringChanged(QString newValue) {
    int intValue = newValue.toInt();
    if (m_value != intValue) {
        m_value = intValue;
        emit updateInt(m_value);
        emit dataChanged();
    }
}

////////////////////////////////////////////////////////////////////////////////
// class UCharBinding
////////////////////////////////////////////////////////////////////////////////

UCharBinding* UCharBinding::bindSliderAndTextbox(
        QSlider *slider, QLineEdit *textbox, unsigned char &value, int minValue, int maxValue) {
    // Bind the slider, the textbox, and the value together
    UCharBinding *binding = new UCharBinding(value);
    connect(slider, SIGNAL(valueChanged(int)), binding, SLOT(ucharChanged(int)));
    connect(slider, SIGNAL(sliderMoved(int)), binding, SLOT(ucharChanged(int)));
    connect(textbox, SIGNAL(textChanged(QString)), binding, SLOT(stringChanged(QString)));
    connect(binding, SIGNAL(updateInt(int)), slider, SLOT(setValue(int)));
    connect(binding, SIGNAL(updateString(QString)), textbox, SLOT(setText(QString)));

    // Set the range and initial value
    slider->setMinimum(minValue);
    slider->setMaximum(maxValue);
    slider->setValue(value);
    textbox->setText(QString::number(value));

    return binding;
}

UCharBinding* UCharBinding::bindTextbox(QLineEdit *textbox, unsigned char &value) {
    // Bind the the textbox and the value together
    UCharBinding *binding = new UCharBinding(value);
    connect(textbox, SIGNAL(textChanged(QString)), binding, SLOT(stringChanged(QString)));

    // Set the initial value
    textbox->setText(QString::number(value));

    return binding;
}

void UCharBinding::ucharChanged(int newValue) {
    if (m_value != newValue) {
        m_value = static_cast<unsigned char>(newValue);
        emit updateString(QString::number(m_value));
        emit dataChanged();
    }
}

void UCharBinding::stringChanged(QString newValue) {
    int intValue = newValue.toInt();
    if (m_value != intValue) {
        m_value = intValue;
        emit updateInt(static_cast<int>(m_value));
        emit dataChanged();
    }
}

////////////////////////////////////////////////////////////////////////////////
// class FloatBinding
////////////////////////////////////////////////////////////////////////////////

// Sliders can only work in ints, so use 100 slider units for every value unit (two decimals of
// resolution)
inline float mapValue(int i) { return static_cast<float>(i) * 0.01f; }
inline int mapValue(float f) { return (int)ceilf(f * 100 - 0.5f); }

FloatBinding* FloatBinding::bindSliderAndTextbox(
        QSlider *slider, QLineEdit *textbox, float &value, float minValue, float maxValue) {
    // Bind the slider, the textbox, and the value together
    FloatBinding *binding = new FloatBinding(value);
    connect(slider, SIGNAL(valueChanged(int)), binding, SLOT(intChanged(int)));
    connect(slider, SIGNAL(sliderMoved(int)), binding, SLOT(intChanged(int)));
    connect(textbox, SIGNAL(textChanged(QString)), binding, SLOT(stringChanged(QString)));
    connect(binding, SIGNAL(updateInt(int)), slider, SLOT(setValue(int)));
    connect(binding, SIGNAL(updateString(QString)), textbox, SLOT(setText(QString)));
    //connect(binding, SIGNAL(updateString(QString)), binding, SLOT(dataChanged()));

    // Set the range and initial value
    slider->setMinimum(mapValue(minValue));
    slider->setMaximum(mapValue(maxValue));
    slider->setValue(mapValue(value));
    textbox->setText(QString::number(value));

    return binding;
}
FloatBinding* FloatBinding::bindTextbox(QLineEdit *textbox, float &value) {
    // Bind the the textbox and the value together
    FloatBinding *binding = new FloatBinding(value);
    connect(textbox, SIGNAL(textChanged(QString)), binding, SLOT(stringChanged(QString)));

    // Set the initial value
    textbox->setText(QString::number(value));

    return binding;
}

FloatBinding* FloatBinding::bindDial(
        QDial *dial, float &value, float minValue, float maxValue, bool wrappingExtendsRange) {
    // Bind the dial and the value together
    FloatBinding *binding = new FloatBinding(value);
    connect(dial, SIGNAL(valueChanged(int)), binding, SLOT(intChanged(int)));

    // Store state for computing dial wrapping
    binding->m_maxValue = maxValue;
    binding->m_minValue = minValue;
    binding->m_wrappingExtendsRange = wrappingExtendsRange;

    // Set the initial value
    dial->setMinimum(mapValue(minValue));
    dial->setMaximum(mapValue(maxValue));
    dial->setValue(mapValue(value));

    return binding;
}

void FloatBinding::intChanged(int newValue) {
    float REALValue = mapValue(newValue);

    if (m_wrappingExtendsRange) {
        float lowerBound = m_minValue * 0.75f + m_maxValue * 0.25f;
        float upperBound = m_minValue * 0.25f + m_maxValue * 0.75f;

        // This is a hack to get dials to wrap around infinitely
        //
        //        A           B           C           D
        //  |-----------|-----------|-----------|-----------|
        // min     lowerBound              upperBound      max
        //
        // Any jump from A -> D or D -> A is considered a wrap around

        if (m_value - m_offset < lowerBound && REALValue > upperBound)
            m_offset -= m_maxValue - m_minValue;
        else if (m_value - m_offset > upperBound && REALValue < lowerBound)
            m_offset += m_maxValue - m_minValue;
    }

    if (m_value - m_offset != REALValue) {
        m_value = REALValue + m_offset;
        emit updateString(QString::number(m_value - m_offset));
        emit dataChanged();
    }
}

void FloatBinding::stringChanged(QString newValue) {
    float REALValue = newValue.toFloat();
    if (m_value - m_offset != REALValue) {
        m_value = REALValue + m_offset;
        emit updateInt(mapValue(m_value - m_offset));
        emit dataChanged();
    }
}

////////////////////////////////////////////////////////////////////////////////
// class BoolBinding
////////////////////////////////////////////////////////////////////////////////

BoolBinding* BoolBinding::bindCheckbox(QCheckBox *checkbox, bool &value) {
    // Bind the checkbox and the value together
    BoolBinding *binding = new BoolBinding(value);
    connect(checkbox, SIGNAL(toggled(bool)), binding, SLOT(boolChanged(bool)));

    // Set the initial value
    checkbox->setChecked(value);

    return binding;
}

BoolBinding* BoolBinding::bindDock(QDockWidget *dock, bool &value) {
    // Bind the checkbox and the value together
    BoolBinding *binding = new BoolBinding(value);
    connect(dock, SIGNAL(visibilityChanged(bool)), binding, SLOT(boolChanged(bool)));

    // Set the initial value
    dock->setVisible(value);

    return binding;
}

void BoolBinding::boolChanged(bool newValue) {
    m_value = newValue;
    emit dataChanged();
}

////////////////////////////////////////////////////////////////////////////////
// class ChoiceBinding
////////////////////////////////////////////////////////////////////////////////

ChoiceBinding* ChoiceBinding::bindRadioButtons(
        QButtonGroup *buttonGroup, int numRadioButtons, int &value, ...) {
    // Adjust button group from the variable argument list following initialValue
    va_list args;
    va_start(args, value);
    for (int id = 0; id < numRadioButtons; id++)
        buttonGroup->addButton(va_arg(args, QRadioButton *), id);
    va_end(args);

    // Bind the button group and the value together
    ChoiceBinding *binding = new ChoiceBinding(value);
    connect(buttonGroup, SIGNAL(buttonClicked(int)), binding, SLOT(intChanged(int)));

    // Set the initial value
    value = qMax(0, qMin(numRadioButtons - 1, value));
    buttonGroup->button(value)->click();

    return binding;
}

ChoiceBinding* ChoiceBinding::bindTabs(QTabWidget *tabs, int &value) {
    // Bind the tabs and the value together
    ChoiceBinding *binding = new ChoiceBinding(value);
    connect(tabs, SIGNAL(currentChanged(int)), binding, SLOT(intChanged(int)));

    // Set the initial value
    value = qMax(0, qMin(tabs->count() - 1, value));
    tabs->setCurrentIndex(value);

    return binding;
}

void ChoiceBinding::intChanged(int newValue) {
    m_value = newValue;
    emit dataChanged();
}


////////////////////////////////////////////////////////////////////////////////
// class ColorBinding
////////////////////////////////////////////////////////////////////////////////

ColorBinding* ColorBinding::bindButtonAndTextboxes(
        QPushButton *button, QLineEdit *rTextbox, QLineEdit *gTextbox, QLineEdit *bTextbox,
        QColor &value) {
    // Bind the slider, the textbox, and the value together
    ColorBinding *binding = new ColorBinding(value, button);
    connect(button, SIGNAL(pressed()), binding, SLOT(buttonPushed()));
    connect(rTextbox, SIGNAL(textChanged(QString)), binding, SLOT(rStringChanged(QString)));
    connect(gTextbox, SIGNAL(textChanged(QString)), binding, SLOT(gStringChanged(QString)));
    connect(bTextbox, SIGNAL(textChanged(QString)), binding, SLOT(bStringChanged(QString)));
    connect(binding, SIGNAL(updateRString(QString)), rTextbox, SLOT(setText(QString)));
    connect(binding, SIGNAL(updateGString(QString)), gTextbox, SLOT(setText(QString)));
    connect(binding, SIGNAL(updateBString(QString)), bTextbox, SLOT(setText(QString)));
    rTextbox->setText(QString::number(value.redF(), 'f', 2));
    gTextbox->setText(QString::number(value.greenF(), 'f', 2));
    bTextbox->setText(QString::number(value.blueF(), 'f', 2));

    return binding;
}

void ColorBinding::rStringChanged(QString newValue) {
    float floatValue = std::max(0.f, std::min(1.f, newValue.toFloat()));
    if (m_value.redF() != floatValue) {
        m_value.setRedF(floatValue);
        updateButtonColor();
        emit dataChanged();
    }
}

void ColorBinding::gStringChanged(QString newValue) {
    float floatValue = std::max(0.f, std::min(1.f, newValue.toFloat()));
    if (m_value.greenF() != floatValue) {
        m_value.setGreenF(floatValue);
        updateButtonColor();
        emit dataChanged();
    }
}

void ColorBinding::bStringChanged(QString newValue) {
    float floatValue = std::max(0.f, std::min(1.f, newValue.toFloat()));
    if (m_value.blueF() != floatValue) {
        m_value.setBlueF(floatValue);
        updateButtonColor();
        emit dataChanged();
    }
}

void ColorBinding::buttonPushed() {
    QColor color = QColorDialog::getColor(m_value);
    if (color.isValid()) {
        updateButtonColor();
        emit updateRString(QString::number(color.redF(), 'f', 2));
        emit updateGString(QString::number(color.greenF(), 'f', 2));
        emit updateBString(QString::number(color.blueF(), 'f', 2));
        emit dataChanged();
    }
}

void ColorBinding::updateButtonColor() {
    QString styleSheet("background: #"
          + QString(m_value.red() < 16? "0" : "") + QString::number(m_value.red(),16)
          + QString(m_value.green() < 16? "0" : "") + QString::number(m_value.green(),16)
          + QString(m_value.blue() < 16? "0" : "") + QString::number(m_value.blue(),16) + ";");
    QRect r = m_button->geometry();
    m_button->setStyleSheet(styleSheet);
    m_button->setGeometry(r);
}


