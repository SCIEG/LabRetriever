#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QList>
#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void tableWidgetCustomMenu(const QPoint &q);

    void on_loadButton_clicked();

    void on_outputButton_clicked();

    void on_inputBrowseButton_clicked();

    void on_outputBrowseButton_clicked();

private:
    void saveData(const QString& fileName);
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
