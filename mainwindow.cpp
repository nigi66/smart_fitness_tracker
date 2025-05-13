#include "mainwindow.h"
#include "ui_mainwindow.h"


#include <QFileDialog>
#include <QMessageBox>
#include <QTableWidgetItem>
#include <QTextStream>
#include <QPushButton>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->btnLoadCSV, &QPushButton::clicked, this, &MainWindow::on_btnLoadCSV_clicked);



}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_btnLoadCSV_clicked(){
    QString fileName = QFileDialog::getOpenFileName(this, "Open Activity File", "", "CSV Files (*.csv)");
    if (fileName.isEmpty()) return;

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Error", "Could not open file.");
        return;
    }

    QTextStream in(&file);
    ui->activityTable->setRowCount(0);
    ui->activityTable->setColumnCount(0);

    // 🔤 Set headers dynamically
    QString headerLine = in.readLine().trimmed();
    QStringList headers = headerLine.split(',', Qt::SkipEmptyParts);
    ui->activityTable->setColumnCount(headers.size());
    ui->activityTable->setHorizontalHeaderLabels(headers);

    int row = 0;

    while(!in.atEnd()){
        QString line = in.readLine().trimmed();
        if(line.isEmpty()) continue;

        QStringList tokens = line.split(',', Qt::SkipEmptyParts);
        if (tokens.size() != headers.size()) continue;

        ui->activityTable->insertRow(row);
        for(int col = 0; col < tokens.size(); col++){
            ui->activityTable->setItem(row, col, new QTableWidgetItem(tokens[col].trimmed()));
        }
        row++;
    }
    file.close();
    QMessageBox::information(this, "Loaded", QString("Loaded %1 activities").arg(row));
}


void MainWindow::on_btnShowSummary_clicked(){

}




