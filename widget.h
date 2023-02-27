#ifndef WIDGET_H
#define WIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QMouseEvent>
#include <QWheelEvent>

#include "surface.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QOpenGLWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    void setStep(double value);
    void setColor(int color[3], int value);

    void setTypeOfRendering(const QString &value);
    void setTypeOfView(const QString &value);

private:
    void wheelEvent(QWheelEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;

    void paintGL() override;
    void initializeGL() override;
    void resizeGL(int w, int h) override;

    double coordinateScale(double value);
    void DrawSurface();
    void makeIndex();
    void rotateGraph();
    void scalingGraph();
    void CreateShaders();

    QString typeOfRendering = "Точки";
    QString typeOfView = "Ортогоналная";

    Surface surface;
    QPointF mousePosition;

    QOpenGLShaderProgram program;
    QMatrix4x4 matrix;

    GLuint* indexArray;

    std::string vertexShader;
    std::string fragmentShader;
    GLfloat colors[9] = {1, 0, 0, 0, 1, 0, 0, 0, 1};

    float step;
    int countOfPointInLine;
    int countOfIndex;
    double wheelScroll = 1.0;
    double rotateX = 0;
    double rotateY = 0;

    void setProgramGL();
};
#endif // WIDGET_H
