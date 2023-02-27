#include "widget.h"

Widget::Widget(QWidget *parent)
    : QOpenGLWidget(parent)
{
    countOfPointInLine = sqrt(10000);
    countOfIndex = pow(countOfPointInLine - 1, 2) * 6;
    makeIndex();
}

void Widget::initializeGL()
{
        this->setMouseTracking(true);
        QOpenGLFunctions* f = QOpenGLContext::currentContext()->functions();
        f->glClearColor(102.0 / 255, 102.0 / 255, 102.0 / 255, 1);

        glEnable(GL_DEPTH_TEST);
        glEnableClientState(GL_VERTEX_ARRAY);
        CreateShaders();
}

void Widget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_TEST);

    matrix.setToIdentity();

    if (typeOfView == "Ортогональная")
    {
        matrix.ortho(-1 , 1, -1, 1, -1, 1);
    }
    else if (typeOfView == "Перспективная")
    {
        matrix.frustum(-1, 1, -1, 1, 1, 3);
        matrix.translate(0, 0, -2);
    }

    scalingGraph();
    rotateGraph();

    DrawSurface();

    update();
}

void Widget::resizeGL(int w, int h)
{
    QOpenGLFunctions* f = QOpenGLContext::currentContext()->functions();
        f->glViewport(0, 0, w, h);
}

void Widget::DrawSurface()
{
    int vertexLocation = program.attributeLocation("vertex");
    program.setAttributeArray(vertexLocation, surface.arrayOfPoint, 3);

    int colorsLocation = program.uniformLocation("colors");
    program.setUniformValueArray(colorsLocation, colors, 9, 1);

    int matrixLocation = program.uniformLocation("matrix");
    program.setUniformValue(matrixLocation, matrix);

    int stepLocation = program.uniformLocation("step");
    program.setUniformValue(stepLocation, step);

    int heightLocation = program.uniformLocation("height");
    program.setUniformValue(heightLocation, abs(surface.getMaxZ()) + abs(surface.getMinZ()));

    glVertexPointer(3, GL_FLOAT, 0, surface.arrayOfPoint);
    glColor3f(0.59f,0.26f, 0.43f);
    if (typeOfRendering == "Точки")
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
    }
    else if (typeOfRendering == "Треугольники")
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
    else if (typeOfRendering == "Линии")
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    glDrawElements(GL_TRIANGLES, countOfIndex, GL_UNSIGNED_INT, indexArray);

    program.disableAttributeArray(vertexLocation);
}

void Widget::makeIndex()
{
        indexArray = new GLuint[countOfIndex];
        int j = 0;
        for (int i = 0; i < countOfIndex; i += 6)
        {
            indexArray[i] = j;
            indexArray[i + 1] = j + countOfPointInLine;
            indexArray[i + 2] = j + 1;

            indexArray[i + 4] = j + countOfPointInLine;
            indexArray[i + 3] = j + 1;
            indexArray[i + 5] = j + 1 + countOfPointInLine;
            (j + 1) % countOfPointInLine == countOfPointInLine - 1 ? j += 2 : j++;
        }
}

void Widget::CreateShaders()
{
    QOpenGLShader vertexShader(QOpenGLShader::Vertex);
    vertexShader.compileSourceCode(
                "attribute highp vec4 vertex;\n"
                "uniform highp mat4 matrix;\n"
                "varying  vec4 v;"
                "void main(void)\n"
                "{\n"
                "   gl_Position = matrix * vertex;\n"
                "	v=vertex;"
                "}");
    program.addShader(&vertexShader);

    QOpenGLShader fragmentShader(QOpenGLShader::Fragment);
    fragmentShader.compileSourceCode(
                "uniform float height;"
                "uniform float step;"
                "varying  vec4 v;"
                "uniform float colors[9];"
                "void main(void)\n"
                "{\n"
                "vec3 c1=vec3(colors[0],colors[1],colors[2]);"
                "vec3 c2=vec3(colors[3],colors[4],colors[5]);"
                "vec3 c3=vec3(colors[6],colors[7],colors[8]);"
                "vec4 color;"
                "float k=(v.z+1)/height;"
                "if (k<0.5) color=vec4(mix(c3,c2,k*2),1);"
                "else color=vec4(mix(c2,c1,(k-0.5)*2),1);"
                "if (step!=0){"
                "float n=(v.z+1)/step;"
                "int c=int(n);"
                "if (n-c<1e-2) color=vec4(0,0,0,0);"
                "}"
                "gl_FragColor = color; \n"
                "}");
    program.addShader(&fragmentShader);

    program.link();
    program.bind();
}

void Widget::rotateGraph()
{
    matrix.rotate(-rotateX, 1, 0, 0);
    matrix.rotate(-rotateY, 0, 1, 0);
}

void Widget::setStep(double value)
{
    step = value;
    update();
}

void Widget::setColor(int color[3], int value)
{
    for (int i = 3 * value; i < 3 + 3 * value; i++)
    {
        colors[i] = color[i % 3] / 255.0;
    }
    update();
}

void Widget::scalingGraph()
{
    if (wheelScroll > 0)
            matrix.scale(wheelScroll, wheelScroll, wheelScroll);
        else
            matrix.scale(1 / -wheelScroll, 1 / -wheelScroll, 1 / -wheelScroll);
}

void Widget::setTypeOfView(const QString &value)
{
    typeOfView = value;
}

void Widget::setTypeOfRendering(const QString &value)
{
    typeOfRendering = value;
}

double Widget::coordinateScale(double value)
{
    if (wheelScroll < 0)
        return value * -wheelScroll;
        else if (wheelScroll > 0)
            return  value / wheelScroll;
}

void Widget::wheelEvent(QWheelEvent *event)
{
    event->accept();
    int delta = event->angleDelta().y() / 32;
    wheelScroll += delta;
    update();
}

void Widget::mouseMoveEvent(QMouseEvent *event)
{
    event->accept();
    if (event->buttons() & Qt::LeftButton)
    {
            qreal mouseMove = mousePosition.y() - event->localPos().y();
            rotateX += 1 / M_PI * (mouseMove);

            mouseMove = mousePosition.x() - event->localPos().x();
            rotateY += 1 / M_PI * (mouseMove);

            mousePosition = event->localPos();
    }
    update();
}
void Widget::mousePressEvent(QMouseEvent* event)
{
    event->accept();
    mousePosition = event->localPos();
    update();
}



