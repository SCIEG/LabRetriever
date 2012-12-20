#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "utils/FileReaderUtil.h"
#include "utils/VectorUtil.h"
#include "datawidget.h"

#include <algorithm>

#include <QFile>
#include <QFileDialog>
#include <QIODevice>
#include <QMessageBox>
#include <QTextStream>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    vector< vector<string> > csvFile =
            LabRetriever::readRawCsv("data/LocusAbbreviationMappings.csv");

    for (unsigned int index = 0; index < csvFile.size(); index++) {
        string longName = csvFile[index][0];
        string abbr = csvFile[index][1];
        this->lociAbbrs[longName] = abbr;
        this->lociOrdering[abbr] = index;

        DataWidget* dataWidget = new DataWidget();
        dataWidget->setText(abbr.c_str());
        ui->listLayout->addWidget(dataWidget);
        this->dataWidgets.push_back(dataWidget);
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::resetData() {
    ui->excludeSampleListWidget->clear();
    ui->exportSampleListWidget->clear();
    this->sampleData.clear();
}

void MainWindow::on_browseButton_clicked()
{
    QString inputText = QFileDialog::getOpenFileName();
    if (inputText.isEmpty()) return;
    ui->inputFileLineEdit->setText(inputText);
}

void MainWindow::on_loadButton_clicked()
{
    // Check for a valid file.
    QString qFileName = ui->inputFileLineEdit->text();
    if (qFileName.isEmpty()) return;

    string fileName = qFileName.toStdString();
    vector< vector<string> > csvData = LabRetriever::readRawCsv(fileName);

    if (csvData.size() == 0) return;

    // Find the indexes of the data.
    vector<string>& header = csvData[0];
    int sampleNameIndex = -1, markerIndex = -1, alleleIndex = -1,
            sampleFileIndex = -1;
    for (unsigned int i = 0; i < header.size(); i++) {
        if (header[i] == "Sample Name") sampleNameIndex = i;
        else if (header[i] == "Sample File") sampleFileIndex = i;
        else if (header[i] == "Marker") markerIndex = i;
        else if (header[i] == "Allele 1") alleleIndex = i;
    }
    // Use the unique file name over the sample name.
    if (sampleFileIndex != -1) sampleNameIndex = sampleFileIndex;

    if (sampleNameIndex == -1 || markerIndex == -1 || alleleIndex == -1) {
        QMessageBox::critical(this, "Error parsing file!",
                        "Cannot parse file. File must have 'Sample Name'/'Sample File', "
                        "'Marker', and end with a sequence of 'Allele X', where X's are numbers.");
        return;
    }

    // If file is valid, clear current data.
    resetData();

    for (unsigned int rowIndex = 1; rowIndex < csvData.size(); rowIndex++) {
        if (csvData[rowIndex].size() < 3) continue;

        QString sampleName = QString::fromStdString(csvData[rowIndex][sampleNameIndex]);
        string locus = csvData[rowIndex][markerIndex];
        if (locus == "AMEX") continue;

        string locusAbbr = this->lociAbbrs[locus];

        map<string, vector<string> >& lociToDetectedAlleles = this->sampleData[sampleName];
        for (unsigned int colIndex = alleleIndex; colIndex < csvData[rowIndex].size(); colIndex += 2) {
            string allele = csvData[rowIndex][colIndex];
            if (allele.size() == 0) break;

            lociToDetectedAlleles[locusAbbr].push_back(allele);
        }

        for (map<string, vector<string> >::iterator iter = lociToDetectedAlleles.begin();
             iter != lociToDetectedAlleles.end(); iter++) {
            vector<string>& detectedAlleles = iter->second;
            sort(detectedAlleles.begin(), detectedAlleles.end());
        }
    }

    for (map<QString, map<string, vector<string> > >::const_iterator iter =
         this->sampleData.begin(); iter != this->sampleData.end(); iter++) {
        QString sampleName = iter->first;
        ui->excludeSampleListWidget->addItem(sampleName);
    }

    if (ui->excludeSampleListWidget->count() > 0) {
        ui->excludeSampleListWidget->setCurrentRow(0);
        loadSampleData(ui->excludeSampleListWidget->item(0)->text());
    }

    int indexOfExtension = qFileName.lastIndexOf('.');
    if (indexOfExtension == -1) indexOfExtension = qFileName.size();
    QString outputFileName = qFileName.left(indexOfExtension) + "_LRinput.csv";

    ui->outputFileLineEdit->setText(outputFileName);
}

void MainWindow::loadSampleData(const QString& sampleName) {
    if (this->curSampleName == sampleName) return;

    this->curSampleName = sampleName;
    map<string, vector<string> >& lociToDetectedAlleles =
            this->sampleData[sampleName];
    for (map<string, vector<string> >::const_iterator iter = lociToDetectedAlleles.begin();
         iter != lociToDetectedAlleles.end(); iter++) {
        const string& locus = iter->first;
        const vector<string>& alleles = iter->second;
        this->dataWidgets[this->lociOrdering[locus]]->setDetectedAlleles(alleles);
    }
}

void MainWindow::on_exportSampleListWidget_itemClicked(QListWidgetItem *item)
{
    ui->excludeSampleListWidget->clearSelection();
    loadSampleData(item->text());
}

void MainWindow::on_excludeSampleListWidget_itemClicked(QListWidgetItem *item)
{
    ui->exportSampleListWidget->clearSelection();
    loadSampleData(item->text());
}


void swapListWidgets(QListWidget* from, QListWidget* to, QListWidgetItem *item) {
    from->takeItem(from->row(item));
    from->clearSelection();

    to->addItem(item);
    to->setCurrentItem(item);
}

void MainWindow::on_exportSampleListWidget_itemDoubleClicked(QListWidgetItem *item)
{
    swapListWidgets(ui->exportSampleListWidget, ui->excludeSampleListWidget, item);
}

void MainWindow::on_excludeSampleListWidget_itemDoubleClicked(QListWidgetItem *item)
{
    swapListWidgets(ui->excludeSampleListWidget, ui->exportSampleListWidget, item);
}

void MainWindow::on_exportButton_clicked()
{
    QString fileName = ui->outputFileLineEdit->text().replace('\\', '/');
    QFile file(fileName);
    if (file.exists()) file.remove();

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "Cannot create output file!",
            "Cannot create output file! Perhaps the file is open by another program?");
        return;
    }

    QTextStream out(&file);

    // Parameter stuff that can be filled in later:
    out << "alpha," << endl;
    out << "Drop-in rate," << endl;
    out << "Drop-out rate," << endl;
    out << "Race," << endl;
    out << "IBD Probs,1,0,0" << endl;

    for (map<string, string>::const_iterator iter = this->lociAbbrs.begin();
         iter != this->lociAbbrs.end(); iter++) {
        if (iter->first == "AMEL") continue;
        const string& locusAbbr = iter->second;
        DataWidget* dataWidget = this->dataWidgets[this->lociOrdering[locusAbbr]];
        const vector<QString>& assumedAlleles = dataWidget->getAssumedAlleles();
        const vector<QString>& suspectedAlleles = dataWidget->getSuspectedAlleles();
        QString locusAbbrQString = QString::fromStdString(locusAbbr).trimmed();

        out << locusAbbrQString << "-Assumed";
        for (unsigned int i = 0; i < assumedAlleles.size(); i++) {
            out << "," << assumedAlleles[i];
        }
        out << endl;
        out << locusAbbrQString << "-Suspected";
        for (unsigned int i = 0; i < suspectedAlleles.size(); i++) {
            out << "," << suspectedAlleles[i];
        }
        out << endl;

        QString unattribAlleleHeader = locusAbbrQString + "-Unattributed";

        for (unsigned int i = 0; i < ui->exportSampleListWidget->count(); i++) {
            const QString& sampleName = ui->exportSampleListWidget->item(i)->text();
            const vector<string>& alleles = this->sampleData[sampleName][locusAbbr];
            if (alleles.size() != 0) {
                out << unattribAlleleHeader;
                for (unsigned int j = 0; j < alleles.size(); j++) {
                    QString allele = QString::fromStdString(alleles[j]);
                    if (vectorFind(assumedAlleles, allele)) {
                        continue;
                    }
                    out << "," << allele;
                }
                out << endl;
            }
        }
    }
    file.close();

    QMessageBox::information(this, "Done!",
                             "Data has been exported to " + fileName + ".");
}
