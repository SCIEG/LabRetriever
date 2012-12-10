#include "datawidget.h"
#include "ui_datawidget.h"
#include "utils/VectorUtil.h"

#include <algorithm>

#include <QSizePolicy>
#include <QtCore/QChar>

DataWidget::DataWidget(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::DataWidget)
{
    ui->setupUi(this);
//    ui->locusLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
}

DataWidget::~DataWidget()
{
    delete ui;
}

void DataWidget::setText(const QString& text) {
    ui->locusLabel->setText(text);
    ui->locusLabel->setFixedSize(45, 20);
}

void DataWidget::setDetectedAlleles(const vector<string>& detected) {
    vector<QString> newDetectedAlleles;
    for (unsigned int i = 0; i < detected.size(); i++) {
        newDetectedAlleles.push_back(QString::fromStdString(detected[i]));
    }
    this->detectedAlleles = newDetectedAlleles;
    ui->detectedLineEdit->setText(qstringFromVector(newDetectedAlleles));

    update();
}

const vector<QString>& DataWidget::getAssumedAlleles() {
    return this->assumedAlleles;
}

const vector<QString>& DataWidget::getSuspectedAlleles() {
    return this->suspectedAlleles;
}

void DataWidget::update() {
    this->assumedAlleles = vectorFromQString(ui->assumedLineEdit->text());
    this->suspectedAlleles = vectorFromQString(ui->supectedLineEdit->text());

    this->unattributedAlleles.clear();

    // N^2 algorithm
    for (unsigned int i = 0; i < this->detectedAlleles.size(); i++) {
        const QString& allele = this->detectedAlleles[i];
        if (!vectorFind(this->assumedAlleles, allele)) {
            this->unattributedAlleles.push_back(allele);
        }
    }

    ui->unattributedLineEdit->setText(qstringFromVector(this->unattributedAlleles));
}

QString DataWidget::qstringFromVector(const vector<QString>& vector) {
    if (vector.size() == 0) return QString();

    QString retVal = vector[0];
    for (unsigned int i = 1; i < vector.size(); i++) {
        retVal += ", " + vector[i];
    }
    return retVal;
}


bool isSeparator(const QChar& c) {
    return c == ' ' || c == ',';
}

vector<QString> DataWidget::vectorFromQString(const QString& qstring) {
    vector<QString> retVal;
    unsigned int beginIndex = 0;
    unsigned int endIndex = 0;
    for (; endIndex < qstring.length(); endIndex++) {
        if (isSeparator(qstring[endIndex])) {
            if (beginIndex != endIndex) {
                retVal.push_back(qstring.mid(beginIndex, endIndex - beginIndex));
            }
            beginIndex = endIndex + 1;
        }
    }

    if (beginIndex != endIndex) {
        retVal.push_back(qstring.mid(beginIndex, endIndex - beginIndex));
    }

    return retVal;
}

void DataWidget::on_assumedLineEdit_editingFinished()
{
    update();
}

void DataWidget::on_supectedLineEdit_editingFinished()
{
//    update();
}

void DataWidget::on_assumedLineEdit_textEdited(const QString &arg1)
{
    update();
}
