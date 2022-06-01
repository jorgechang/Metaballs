#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QButtonGroup>
#include <QMainWindow>
#include <QList>
#include "settings.h"
#include "glwidget.h"

namespace Ui {
    class MainWindow;
}

class DataBinding;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void settingsChanged();
    void addMetaballButtonClicked();
    void removeMetaballButtonClicked();

protected:
    // Overridden from QWidget
    void closeEvent(QCloseEvent *event);

private:
    Ui::MainWindow *ui;
    QList<DataBinding *> m_bindings;
    GLWidget *m_Canvas;

    void dataBind();
};

#endif // MAINWINDOW_H
