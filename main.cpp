#include "widget.h"

#include <QApplication>
#include <QComboBox>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QMainWindow>
#include <QPushButton>
#include <QColorDialog>
#include <QDoubleSpinBox>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    auto* MyWidget = new Widget;

    auto* comboBoxView = new QComboBox;
    comboBoxView->addItem("Ортогональная");
    comboBoxView->addItem("Перспективная");
    comboBoxView->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    auto* comboBoxRendering = new QComboBox;
    comboBoxRendering->addItem("Точки");
    comboBoxRendering->addItem("Треугольники");
    comboBoxRendering->addItem("Линии");
    comboBoxRendering->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    auto* formLayout = new QFormLayout;
    formLayout->addRow(comboBoxView);
    formLayout->addRow(comboBoxRendering);

    QPushButton* button[3];
    for (int i = 0; i < 3; i++) {
        button[i] = new QPushButton();
        button[i]->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        QPalette pal = button[i]->palette();
        pal.setColor(QPalette::Button, QColor((i == 0) * 255, (i == 1) * 255, (i == 2) * 255));
        button[i]->setAutoFillBackground(true);
        button[i]->setPalette(pal);
        button[i]->setFlat(true);
        formLayout->addRow(button[i]);
        QObject::connect(button[i], &QPushButton::clicked, MyWidget, [ MyWidget, i, button]() {
            QColor color = QColorDialog::getColor(Qt::yellow);
            int c[3] = { color.red(),color.green(),color.blue()
            };
            MyWidget->setColor(c, i);
            QPalette pal = button[i]->palette();
            pal.setColor(QPalette::Button, color);
            button[i]->setPalette(pal);
            button[i]->update();
            });
    }

   QDoubleSpinBox* spinbox = new QDoubleSpinBox();
   spinbox->setSingleStep(0.2);
   formLayout->addRow(spinbox);

    auto* panel = new QWidget;
    panel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    panel->setLayout(formLayout);

    auto* hlayout = new QHBoxLayout;
    hlayout->addWidget(MyWidget);
    hlayout->addWidget(panel);

    auto centralWidget = new QWidget;
    centralWidget->setLayout(hlayout);

    QMainWindow window;
    window.setCentralWidget(centralWidget);
    window.resize(1440,720);
    window.show();

    QObject::connect(comboBoxView, &QComboBox::currentTextChanged, MyWidget,[MyWidget,comboBoxView]()
    {
        MyWidget->setTypeOfView(comboBoxView->currentText());
    });
    QObject::connect(comboBoxRendering, &QComboBox::currentTextChanged, MyWidget,[MyWidget,comboBoxRendering]()
    {
        MyWidget->setTypeOfRendering(comboBoxRendering->currentText());
    });
    QObject::connect(spinbox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), MyWidget, [MyWidget](double n) {
        MyWidget->setStep(n);
        });



    return a.exec();
}
