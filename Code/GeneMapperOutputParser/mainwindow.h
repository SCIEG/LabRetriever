#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "datawidget.h"

#include <map>
#include <string>

#include <QListWidgetItem>
#include <QMainWindow>
#include <QtCore/QString>

using namespace std;

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
    void on_browseButton_clicked();

    void on_loadButton_clicked();

    void on_exportSampleListWidget_itemClicked(QListWidgetItem *item);

    void on_exportSampleListWidget_itemDoubleClicked(QListWidgetItem *item);

    void on_excludeSampleListWidget_itemClicked(QListWidgetItem *item);

    void on_excludeSampleListWidget_itemDoubleClicked(QListWidgetItem *item);

    void on_exportButton_clicked();

private:
    void loadSampleData(const QString&);
    void resetData();

    Ui::MainWindow *ui;
    vector<DataWidget*> dataWidgets;

    /** Maps the full locus name to its abbreviation */
    map<string, string> lociAbbrs;

    /** Maps the locus abbreviation to its position in dataWidgets */
    map<string, int> lociOrdering;

    /** Maps a sample string to [a map of (loci abbreviations) to (a vector of alleles).] */
    map<QString, map<string, vector<string> > > sampleData;
    QString curSampleName;
};

#endif // MAINWINDOW_H
