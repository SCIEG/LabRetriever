#ifndef DATAWIDGET_H
#define DATAWIDGET_H

#include <vector>
#include <string>

#include <QFrame>

using namespace std;

namespace Ui {
class DataWidget;
}

class DataWidget : public QFrame
{
    Q_OBJECT
    
public:
    explicit DataWidget(QWidget *parent = 0);
    ~DataWidget();
    void setText(const QString& text);

    void setDetectedAlleles(const vector<string>&);
    const vector<QString>& getAssumedAlleles();
    const vector<QString>& getSuspectedAlleles();

private slots:
    void on_assumedLineEdit_editingFinished();

    void on_supectedLineEdit_editingFinished();

    void on_assumedLineEdit_textEdited(const QString &arg1);

private:
    void update();
    static QString qstringFromVector(const vector<QString>&);
    static vector<QString> vectorFromQString(const QString&);

    Ui::DataWidget *ui;
    vector<QString> assumedAlleles;
    vector<QString> suspectedAlleles;
    vector<QString> detectedAlleles;
    vector<QString> unattributedAlleles;
};

#endif // DATAWIDGET_H
