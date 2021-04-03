#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QtSql/QSqlQuery>
#include <QtSql/QSqlRecord>
#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>
#include <QSqlTableModel>
#include <QtSql/QSqlDatabase>

QSqlDatabase db;
QSqlTableModel *model;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->table_view->setEditTriggers(QTableView::NoEditTriggers);
    ui->table_view->setSortingEnabled(true);
    db= QSqlDatabase::addDatabase("QSQLITE");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_browse_button_clicked()
{
    QString root_path;
    root_path = QFileDialog::getExistingDirectory(this, tr("Select Backup Location"));
    qDebug() << root_path;
    ui->path_label->setText(root_path);
}

void MainWindow::on_open_button_clicked()
{
    QString dbpath = ui->path_label->text() +"/Manifest.db";
    qDebug()<< dbpath;
    QFileInfo dbfile = QFileInfo(dbpath);
    if(dbfile.exists() && dbfile.isFile())
    {
        if(db.isOpen())
        {
            db.close();
        }
        db.setDatabaseName(dbpath);
        db.open();
        model=new QSqlTableModel(NULL,db);
        model->setTable("Files");
        model->select();
        ui->table_view->setModel(model);
    }
    else
    {
        QMessageBox msgbox;
        msgbox.setIcon(QMessageBox::Critical);
        msgbox.setWindowTitle("Error");
        msgbox.setText("Database File Missing");
        msgbox.setStandardButtons(QMessageBox::Ok);
        msgbox.exec();
    }
}

void MainWindow::on_table_view_doubleClicked(const QModelIndex &index)
{
    QVariant fileID = index.sibling(index.row(),0).data();
    qDebug() << fileID.toString();
    QVariant relativePath = index.sibling(index.row(),2).data();
    qDebug() << relativePath.toString();
    QVariant flags = index.sibling(index.row(),3).data();
    qDebug() << flags.toUInt();

    if(1==flags.toUInt())
    {
        QString backupfilepath = ui->path_label->text()+"/"+fileID.toString()[0]+fileID.toString()[1]+"/"+fileID.toString();
        qDebug() << backupfilepath;
        if(QFileInfo(backupfilepath).exists())
        {
            QString savepath=QFileDialog::getExistingDirectory(this, tr("Select Save Location"));
            QFileInfo fileinfo(relativePath.toString());
            QString filename=fileinfo.fileName();
            savepath+="/"+filename;
            qDebug() << savepath;
            QFile::copy(backupfilepath,savepath);
        }
        else
        {
            QMessageBox msgbox;
            msgbox.setIcon(QMessageBox::Warning);
            msgbox.setText("The file do not exists in the backup files");
            msgbox.setStandardButtons(QMessageBox::Ok);
            msgbox.exec();
        }
    }
    else
    {
        QMessageBox msgbox;
        msgbox.setIcon(QMessageBox::Information);
        msgbox.setText("The record selected is not a file");
        msgbox.setStandardButtons(QMessageBox::Ok);
        msgbox.exec();
    }
}
