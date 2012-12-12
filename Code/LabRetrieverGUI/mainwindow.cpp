#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "lrmain.h"
#include "LikelihoodSolver/LikelihoodSolver.h"
#include "utils/DebugUtil.h"

#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QtCore/QString>
#include <QHeaderView>
#include <QPushButton>
#include <QTextStream>
#include <QMenu>
#include <QMessageBox>
#include <QDateTime>

bool isSeparator(const QChar& c) {
    return c == ' ' || c == ',';
}

QString qstringFromVector(const vector<QString>& myVector) {
    if (myVector.size() == 0) return QString();

    QString retVal = myVector[0];
    for (unsigned int i = 1; i < myVector.size(); i++) {
        retVal += ", " + myVector[i];
    }
    return retVal;
}

QString qstringFromVector(const vector<string>& myVector) {
    if (myVector.size() == 0) return QString();

    QString retVal = QString::fromStdString(myVector[0]);
    for (unsigned int i = 1; i < myVector.size(); i++) {
        retVal += QString::fromStdString(", " + myVector[i]);
    }
    return retVal;
}

QString qstringFromSet(const set<string>& mySet) {
    if (mySet.size() == 0) return QString();

    QString retVal = "";
    for (set<string>::const_iterator iter = mySet.begin();
         iter != mySet.end(); iter++) {
        retVal += QString::fromStdString(*iter + ", ");
    }
    return retVal.left(retVal.length() - 2);
}

vector<QString> vectorFromQString(const QString& qstring) {
    vector<QString> retVal;
    int beginIndex = 0;
    int endIndex = 0;
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

QString getDataFromCell(QTableWidget* table, int row, int col) {
    QTableWidgetItem* it = table->item(row, col);
    if (!it) return "";
    return it->text();
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QHeaderView* vertHeaderView = new QHeaderView(Qt::Vertical);
    vertHeaderView->setDefaultSectionSize(20);
//    vertHeaderView->setCon
    ui->tableWidget->setVerticalHeader(vertHeaderView);

    ui->tableWidget->setColumnCount(4);
    ui->tableWidget->setRowCount(100);

    ui->tableWidget->setColumnWidth(0, 50);
    ui->tableWidget->setColumnWidth(1, 180);
    ui->tableWidget->setColumnWidth(2, 180);
    ui->tableWidget->setColumnWidth(3, 180);

    ui->tableWidget->setHorizontalHeaderLabels(
        QString("Locus;Assumed Alleles;Suspect Alleles;Unattributed Alleles")
                .split(";"));
    ui->tableWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->tableWidget, SIGNAL(customContextMenuRequested(const QPoint&)),
            SLOT(tableWidgetCustomMenu(const QPoint&)));
}

void MainWindow::saveData(const QString& fileName) {
    double alpha = ui->alphaDoubleSpinBox->value();
    double dropinRate = ui->dropInRateDoubleSpinBox->value();
    double dropoutRate = ui->dropOutRateDoubleSpinBox->value();
    QString race = ui->raceComboBox->currentText();

    double zeroAlleleProb = ui->zeroAlleleProbDoubleSpinBox->value();
    double oneAlleleProb = ui->oneAlleleProbDoubleSpinBox->value();
    double twoAlleleProb = ui->twoAlleleProbDoubleSpinBox->value();

    QFile file(fileName);
//    int counter = 2;
    if (file.exists()) {
        file.remove();
    }

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "Cannot save file!",
            "Cannot save file! Perhaps the file is open by another program?");
        return;
    }

    QTextStream out(&file);

    out << "alpha," << alpha << endl;
    out << "Drop-in rate," << dropinRate << endl;
    out << "Drop-out rate," << dropoutRate << endl;
    out << "Race," << race << endl;
    out << "IBD Probs," << zeroAlleleProb << "," <<
           oneAlleleProb << "," << twoAlleleProb << endl;

    QTableWidget* table = ui->tableWidget;

    const QString* curLocus = 0;
    for (unsigned int row = 0; row < table->rowCount(); row++) {
        QString locus = getDataFromCell(table, row, 0).trimmed();
        if (!locus.isEmpty()) {
            curLocus = &locus;

            const QString& assumed = getDataFromCell(table, row, 1);
            const QString& suspect = getDataFromCell(table, row, 2);
            const QString& unattrib = getDataFromCell(table, row, 3);
            out << *curLocus << "-Assumed,";
            out << qstringFromVector(vectorFromQString(assumed)) << endl;
            out << *curLocus << "-Suspected,";
            out << qstringFromVector(vectorFromQString(suspect)) << endl;
            out << *curLocus << "-Unattributed,";
            out << qstringFromVector(vectorFromQString(unattrib)) << endl;
        } else if (curLocus) {
            const QString& unattrib = getDataFromCell(table, row, 3);
            if (!unattrib.isEmpty()) {
                out << *curLocus << "-Unattributed,";
                out << qstringFromVector(vectorFromQString(unattrib)) << endl;
            }
        }
    }

    file.close();
}

void MainWindow::tableWidgetCustomMenu(const QPoint& q) {
    QPoint globalPos = ui->tableWidget->mapToGlobal(q);
    QMenu myMenu;
    myMenu.addAction("Insert Row");
    myMenu.addAction("Delete Row");
    myMenu.addAction("Delete Contents");

    QAction* selectedItem = myMenu.exec(globalPos);
    if (selectedItem)
    {
        // something was chosen, do stuff
        const QString& text = selectedItem->text();
        if (text == "Insert Row") {
            int row = ui->tableWidget->rowAt(q.y());
            ui->tableWidget->insertRow(row+1);
        } else if (text == "Delete Row") {
            int row = ui->tableWidget->rowAt(q.y());
            ui->tableWidget->removeRow(row);
        } else if (text == "Delete Contents") {
            QTableWidgetItem* item = ui->tableWidget->itemAt(q);
            if (item) {
                item->setText("");
            }
        }
    }
    else
    {
        // nothing was chosen
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_loadButton_clicked()
{
    // Check for a valid file.
    QString qFileName = ui->inputFileLineEdit->text();
    if (qFileName.isEmpty()) return;

    string fileName = qFileName.toStdString();
    vector< vector<string> > inputData = LabRetriever::readRawCsv(fileName);

    if (inputData.size() == 0) return;

    ui->tableWidget->clearContents();

    map<string, vector<string> > locusToSuspectAlleles;
    map<string, set<string> > locusToAssumedAlleles;
    map<string, vector<set<string> > > locusToUnattributedAlleles;

    unsigned int csvIndex = 0;
    for (; csvIndex < inputData.size(); csvIndex++) {
        const vector<string>& row = inputData[csvIndex];
        if (row.size() == 0) continue;

        const string& header = row[0];
        unsigned int index;
        // Hack way to detect input type.
        if ((index = header.find("-Assumed")) != string::npos) {
            string locus = header.substr(0, index);
            set<string> assumedAlleles;
            for (unsigned int i = 1; i < row.size(); i++) {
                string data = row[i];
                if (data.length() != 0) {
                    assumedAlleles.insert(data);
                }
            }
            locusToAssumedAlleles[locus] = assumedAlleles;
        } else if ((index = header.find("-Unattributed")) != string::npos) {
            string locus = header.substr(0, index);
            set<string> unattribAlleles;
            for (unsigned int i = 1; i < row.size(); i++) {
                string data = row[i];
                if (data.length() != 0) {
                    unattribAlleles.insert(data);
                }
            }

            locusToUnattributedAlleles[locus].push_back(unattribAlleles);
        } else if ((index = header.find("-Suspected")) != string::npos) {
            string locus = header.substr(0, index);
            vector<string> suspectAlleles;
            for (unsigned int i = 1; i < row.size(); i++) {
                string data = row[i];
                if (data.length() != 0) {
                    suspectAlleles.push_back(data);
                }
            }
            // If there are no suspected alleles, then there's no point of calculating this.
            if (suspectAlleles.size() == 0) continue;

            locusToSuspectAlleles[locus] = suspectAlleles;
        } else if (header == "alpha") {
            if (row.size() <= 1) continue;
            double value = atof(row[1].c_str());
            if (value != 0) {
                ui->alphaDoubleSpinBox->setValue(value);
            }
        } else if (header == "Drop-in rate") {
            if (row.size() <= 1) continue;
            double value = atof(row[1].c_str());
            if (value != 0) {
                ui->dropInRateDoubleSpinBox->setValue(value);
            }
        } else if (header == "Drop-out rate") {
            if (row.size() <= 1) continue;
            double value = atof(row[1].c_str());
            if (value != 0) {
                ui->dropOutRateDoubleSpinBox->setValue(value);
            }
        } else if (header == "Race") {
            if (row.size() <= 1) continue;
            QString race = QString::fromStdString(row[1]);
            int index = ui->raceComboBox->findText(race);
            if (index != -1) {
                ui->raceComboBox->setCurrentIndex(index);
            }
        } else if (header == "IBD Probs") {
            if (row.size() <= 3 || row[1].size() == 0 ||
                    row[2].size() == 0 || row[3].size() == 0 ) continue;
            ui->zeroAlleleProbDoubleSpinBox->setValue(atof(row[1].c_str()));
            ui->oneAlleleProbDoubleSpinBox->setValue(atof(row[2].c_str()));
            ui->twoAlleleProbDoubleSpinBox->setValue(atof(row[3].c_str()));
        }
    }

    // TODO: check for known loci and alleles.
    set<string> lociToCheck;
    for (map<string, vector<string> >::const_iterator iter = locusToSuspectAlleles.begin();
            iter != locusToSuspectAlleles.end(); iter++) {
        const string& locus = iter->first;
        if (locusToAssumedAlleles.find(locus) != locusToAssumedAlleles.end() &&
                locusToUnattributedAlleles.find(locus) != locusToUnattributedAlleles.end()) {
            lociToCheck.insert(locus);
        }
    }

    int curRow = 0;
    for (set<string>::const_iterator iter =lociToCheck.begin();
            iter != lociToCheck.end(); iter++) {
        string locus = *iter;

        vector<set<string> > unattributedAlleles = locusToUnattributedAlleles[locus];
        set<string> assumedAlleles = locusToAssumedAlleles[locus];
        vector<string> suspectAlleles = locusToSuspectAlleles[locus];

        QTableWidgetItem* locusItem = new QTableWidgetItem(QString::fromStdString(locus));
        QTableWidgetItem* assumedItem = new QTableWidgetItem(qstringFromSet(assumedAlleles));
        QTableWidgetItem* suspectItem = new QTableWidgetItem(qstringFromVector(suspectAlleles));

        ui->tableWidget->setItem(curRow, 0, locusItem);
        ui->tableWidget->setItem(curRow, 1, assumedItem);
        ui->tableWidget->setItem(curRow, 2, suspectItem);

        for (int i = 0; i < unattributedAlleles.size(); i++) {
            QTableWidgetItem* item = new QTableWidgetItem(
                        qstringFromSet(unattributedAlleles[i]));
            ui->tableWidget->setItem(curRow, 3, item);
            curRow++;
        }
    }


    int indexOfExtension = qFileName.lastIndexOf('.');
    if (indexOfExtension == -1) indexOfExtension = qFileName.size();
    QString outputFileName = qFileName.left(indexOfExtension) + "_LRoutput.csv";

    ui->outputFileLineEdit->setText(outputFileName);
}

void MainWindow::on_inputBrowseButton_clicked()
{
    QString inputText = QFileDialog::getOpenFileName();
    if (inputText.isEmpty()) return;
    ui->inputFileLineEdit->setText(inputText);
}

void MainWindow::on_outputButton_clicked()
{
    QString curPath = QDir::currentPath() + "/";
    QString savePath = curPath + "autosave/";
    if (!QDir(savePath).exists()) {
        QDir(curPath).mkdir("autosave");
    }

    QString dateTimeString = QDateTime::currentDateTime().toString("ddd MMMM d yy-hh.mm.ss");
    QString autoSaveInputFile = savePath + dateTimeString + "_input.csv";

    QString fileName = ui->outputFileLineEdit->text().replace('\\', '/');
    if (fileName.isEmpty()) {
        fileName = curPath + dateTimeString + "_output.csv";
    }
    if (!fileName.endsWith(".csv")) {
        fileName += ".csv";
    }
    QFile file(fileName);
    if (file.exists()) file.remove();

    QString tableFolder = "Allele Frequency Tables";
    vector<QString> nonexistantTables;
    QTableWidget* table = ui->tableWidget;
    for (unsigned int row = 0; row < table->rowCount(); row++) {
        QString locus = getDataFromCell(table, row, 0).trimmed();
        if (!locus.isEmpty()) {
            QString tableFileName = locus + "_B.count.csv";
            QString tablePath = tableFolder + "/" + tableFileName;
            ifstream fileStream(tablePath.toStdString().c_str());
            if (!fileStream) {
                nonexistantTables.push_back(tableFileName);
            }
        }
    }

    if (nonexistantTables.size()) {
        stringstream ss;
        ss << "The following tables cannot be found:" << endl
           << endl;
        for (unsigned int iii = 0; iii < nonexistantTables.size(); iii++) {
            ss << "\t" << nonexistantTables[iii].toStdString() << endl;
        }
        ss << endl
           << "Seaching in directory:" << endl
           << endl
           << QDir::currentPath().toStdString() << "/"
                << tableFolder.toStdString() << endl
           << endl
           << "Default values will be used." << endl;

        QMessageBox::information(this, "Missing Tables:",
            QString::fromStdString(ss.str()));
    }

//    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
//        QMessageBox::critical(this, "Cannot create output file!",
//            "Cannot create output file! Perhaps the file is open by another program?");
//        return;
//    }

    QMessageBox::information(this, "Information:",
        "Input has been autosaved as:\n" + autoSaveInputFile + "\n"
        "Output will be saved as:\n" + fileName + "\n"
        "Please be patient while the program is running. A pop-up "
        "will notify you when it is done.", "Click to run.");

    this->saveData(autoSaveInputFile);

    vector<LikelihoodSolver*> solversToUse;

    if (ui->OS0U->isChecked()) {
        solversToUse.push_back(LikelihoodSolver::getSolver(LikelihoodSolver::ONE_SUSPECT_NO_UNKNOWNS));
    }
    if (ui->OS1U->isChecked()) {
        solversToUse.push_back(LikelihoodSolver::getSolver(LikelihoodSolver::ONE_SUSPECT_ONE_UNKNOWN));
    }
    if (ui->OS2U->isChecked()) {
        solversToUse.push_back(LikelihoodSolver::getSolver(LikelihoodSolver::ONE_SUSPECT_TWO_UNKNOWNS));
    }
    if (ui->NS1U->isChecked()) {
        solversToUse.push_back(LikelihoodSolver::getSolver(LikelihoodSolver::NO_SUSPECT_ONE_UNKNOWN));
    }
    if (ui->NS2U->isChecked()) {
        solversToUse.push_back(LikelihoodSolver::getSolver(LikelihoodSolver::NO_SUSPECT_TWO_UNKNOWNS));
    }
    if (ui->NS3U->isChecked()) {
        solversToUse.push_back(LikelihoodSolver::getSolver(LikelihoodSolver::NO_SUSPECT_THREE_UNKNOWNS));
    }

//    for (int i = 0; i < solversToUse.size(); i++) {
//        cout << solversToUse[i]->name << endl;
//    }

    run(autoSaveInputFile.toStdString(), fileName.toStdString(), solversToUse);

    QMessageBox::information(this, "Done!",
                             "Data has been exported to " + fileName + ".");
}


void MainWindow::on_outputBrowseButton_clicked()
{
    QString inputText = QFileDialog::getSaveFileName();
    if (inputText.isEmpty()) return;
    ui->outputFileLineEdit->setText(inputText);
}
