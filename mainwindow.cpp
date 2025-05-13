#include "mainwindow.h"
#include "ui_mainwindow.h"


#include <QFileDialog>
#include <QMessageBox>
#include <QTableWidgetItem>
#include <QTextStream>
#include <QPushButton>

#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QValueAxis>
using namespace QtCharts;


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


void MainWindow::on_btnShowSummary_clicked() {
    int col = ui->activityTable->currentColumn();
    int rows = ui->activityTable->rowCount();

    if (col == -1 || rows == 0) {
        QMessageBox::information(this, "Summary", "Please select a column with numeric values.");
        return;
    }

    QString columnName = ui->activityTable->horizontalHeaderItem(col)->text();

    QVector<double> values;
    QStringList categories;
    for (int row = 0; row < rows; ++row) {
        QTableWidgetItem *item = ui->activityTable->item(row, col);
        if (!item) continue;

        bool ok;
        double value = item->text().toDouble(&ok);
        if (ok) {
            values.append(value);
            categories << QString::number(row + 1); // use row number as label
        }
    }

    if (values.isEmpty()) {
        QMessageBox::warning(this, "Error", "Selected column does not contain any numeric data.");
        return;
    }

    // Summary calculation
    double sum = 0.0, minVal = values[0], maxVal = values[0];
    for (double val : values) {
        sum += val;
        if (val < minVal) minVal = val;
        if (val > maxVal) maxVal = val;
    }
    double mean = sum / values.size();

    QString summary;
    summary += QString("Column: %1\n").arg(columnName);
    summary += QString("Valid Numeric Entries: %1\n").arg(values.size());
    summary += QString("Mean: %1\n").arg(mean, 0, 'f', 2);
    summary += QString("Min: %1\n").arg(minVal, 0, 'f', 2);
    summary += QString("Max: %1\n").arg(maxVal, 0, 'f', 2);
    QMessageBox::information(this, "Column Summary", summary);

    // 📊 Create the chart
    QBarSet *set = new QBarSet(columnName);
    for (double val : values)
        *set << val;

    QBarSeries *series = new QBarSeries();
    series->append(set);

    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("Data for column: " + columnName);
    chart->setAnimationOptions(QChart::SeriesAnimations);

    QBarCategoryAxis *axisX = new QBarCategoryAxis();
    axisX->append(categories);
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    QValueAxis *axisY = new QValueAxis();
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
    chartView->resize(800, 500);
    chartView->setWindowTitle("Column Chart");
    chartView->show();
}





