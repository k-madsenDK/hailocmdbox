#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "./ui_horizonslider.h"
#include "./ui_verticalslider.h"
#include "./ui_bothsliders.h"
#include "./ui_searchlabel.h"
#include <QTimer>
#include <QtCore>
#include <QThread>
#include <iostream>
#include "qthread.h"
#include <QDialog>
#include <QSlider>
#include <QWidgetAction>

MainWindow::MainWindow(QWidget *parent )
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{   QThread* thread;
    Worker* worker;
    thread = new QThread();
    worker = new Worker();
    ui->setupUi(this);
    QWidget::setWindowTitle("QtHailoPipe");
    mbaredit->setDisabled(true);
    labelsettings.beginGroup("hailo_settings");
    const auto  mylabel = labelsettings.value("srclabel").toString();
    if(not mylabel.isEmpty()){
        mbaredit->setText(mylabel);
    }//end if
    labelsettings.endGroup();
    ui->menubar->setCornerWidget(mbaredit, Qt::TopRightCorner);
    ui->statusbar->setVisible(true);
    ui->statusbar->showMessage(mylabel,10000);
    connect(ui->actionAutotrack_off, SIGNAL(triggered()), worker, SLOT(autotrackoff()),Qt::DirectConnection);//autotrack off
    connect(ui->actionAutotrack_on, SIGNAL(triggered()), this, SLOT(camreset()));// reset pico
    connect(ui->actionAutotrack_on, SIGNAL(triggered()), worker, SLOT(autotrackon()),Qt::DirectConnection);// au.. on
    connect(ui->actionHide_text, SIGNAL(triggered()), worker, SLOT(hidehailotxt()),Qt::DirectConnection);//show hailotxt off
    connect(ui->actionshow_text, SIGNAL(triggered()), worker, SLOT(showhailotxt()),Qt::DirectConnection);//sh .. on
    connect(ui->actionboth, SIGNAL(triggered()), this, SLOT(cameraboth()));//control both cammera directions
    connect(ui->actionHorsontal,SIGNAL(triggered()),this,SLOT(camerahorisontal())); //move camera horisontal
    connect(ui->actionVertical,SIGNAL(triggered()),this,SLOT(cameravertical())); //move camera vertical
    connect(ui->actionReset, SIGNAL(triggered()), this, SLOT(camreset()));// reset pico
    connect(ui->actionShift_search_label, SIGNAL(triggered()), this, SLOT(shiftsearchlabel()));// insert new search label in worker thread
    connect(this, SIGNAL(stopautofocus()), worker, SLOT(autotrackoff()),Qt::DirectConnection);//autotrack off
    connect(this, SIGNAL(stopthread()), worker, SLOT(stopthread()),Qt::DirectConnection); //stop worker thread
    connect(this, SIGNAL(startautofocus()), worker, SLOT(autotrackon()),Qt::DirectConnection);//start autofocus
    connect(worker, SIGNAL(newpicox(int)), this, SLOT(piconewposx(int)),Qt::DirectConnection);//new cam pos x
    connect(worker, SIGNAL(newpicoy(int)), this, SLOT(piconewposy(int)),Qt::DirectConnection); //new cam pos y
    connect(worker, SIGNAL(statusbarinsert(QString)), this, SLOT(sndtxtstatusbar(QString))); //status bar insert QString
    connect(this, SIGNAL(changeSearchlabel(QString)), worker, SLOT(settrackingstring(QString)),Qt::DirectConnection);//autotrack off

    //qtimer
    QTimer *timer = new QTimer(this);
    //connect(timer, &QTimer::timeout,  this , [=](){hailo->Qttimer(); });
    connect(timer, &QTimer::timeout,  this , &MainWindow::on_Qtimerrun);//timer not used pt
    timer->start(100);
    //Qthread
    worker->moveToThread(thread);//diverse worker thread slots
    connect( worker, &Worker::Qerror, this, &MainWindow::on_Threadmsg);
    connect( worker, &Worker::print, this, &MainWindow::on_Threadmsg); // print rou
    connect( thread, &QThread::started, worker, &Worker::process);
    connect( worker, &Worker::finished, thread, &QThread::quit);
    connect( worker, &Worker::finished, worker, &Worker::deleteLater);
    connect( worker, &Worker::finished, this, &MainWindow::threadfinish);
    connect( thread, &QThread::finished, thread, &QThread::deleteLater);
    thread->start();
    //connect ui
    connect(ui->lineEdit,SIGNAL(returnPressed()),this,SLOT(text_return()));// commandline text ready
    ui->plainTextEdit->setReadOnly(true);
    connect(this, &MainWindow::sendmytxt , worker , &Worker::sendcomporttxt,Qt::DirectConnection);// send text to the pico comport
}

MainWindow::~MainWindow()
{
    emit stopthread();
    delete ui;
}


void MainWindow::on_textBrowser_anchorClicked(const QUrl &arg1)
{

}

void MainWindow::on_Qtimerrun(void)
{
    //qDebug("virker Timer \n");
}

void MainWindow::camreset(void){
    emit sendmytxt("reset");
    emit startautofocus();
}//end slot

void MainWindow::on_Threadmsg(QString err)
{
    ui->plainTextEdit->appendPlainText(err);
}

void MainWindow::text_return(void){
    QString txt = ui->lineEdit->text();
    ui->lineEdit->selectAll();
    ui->lineEdit->del();
    

    //qDebug(qPrintable(txt));
    emit sendmytxt(txt);
    //ui->plainTextEdit->appendPlainText(txt);
}//end

void MainWindow::camerahorisontal(void){
    emit stopautofocus(); //stop autofocus
    QDialog *dialog = new  QDialog;
    Ui::horizonslider ui;
    ui.setupUi(dialog);
    QObject::connect(ui.horizontalSlider,  SIGNAL(sliderMoved(int)), this, SLOT(newHpos(int)));
    ui.horizontalSlider->setValue(camposx);
    ui.Poslabel->setNum(camposx);
    newHpos(camposx); //stop cammera moved
    dialog->setWindowTitle("Control of horizontel camera position");
    dialog->exec();
}//end slot

void MainWindow::cameravertical(void){
    emit stopautofocus(); //stop autofocus
    QDialog *dialog = new  QDialog;
    Ui::verticalslider ui;
    ui.setupUi(dialog);
    QObject::connect(ui.verticalSlider,  SIGNAL(sliderMoved(int)), this, SLOT(newVpos(int)));
    ui.verticalSlider->setValue(camposy);
    ui.pos->setNum(camposy);
    newVpos(camposy);
    dialog->setWindowTitle("Control of horizontel camera position");
    dialog->exec();
    }//end slot

void MainWindow::cameraboth(void){
    emit stopautofocus(); //stop autofocus
    QDialog *dialog = new  QDialog;
    Ui::bothsliders ui;
    ui.setupUi(dialog);
    QObject::connect(ui.horizontalSlider,  SIGNAL(sliderMoved(int)), this, SLOT(newHpos(int)));
    QObject::connect(ui.verticalSlider,  SIGNAL(sliderMoved(int)), this, SLOT(newVpos(int)));
    ui.horizontalSlider->setValue(camposx);
    ui.verticalSlider->setValue(camposy);
    ui.horlabel->setNum(camposx);
    ui.verlabel->setNum(camposy);
    newVpos(camposy);
    newHpos(camposx); //stop cammera moved
    dialog->setWindowTitle("Control of camera position");
    dialog->exec();
    }//end slot

void MainWindow::newHpos(int value){

    QString txt = "New Hpos:";
    txt +=QLocale().toString(value);
    emit sendmytxt(txt);
    //ui->plainTextEdit->appendPlainText(txt);
}//end slot

void MainWindow::newVpos(int value){

    QString txt = "New Vpos:";
    txt +=QLocale().toString(value);
    emit sendmytxt(txt);
    //ui->plainTextEdit->appendPlainText(txt);
}//end slot

void MainWindow::piconewposx(int posx){
    camposx = posx;
}//end slot

void MainWindow::piconewposy(int posy){
    camposy = posy;
}//end slot

void MainWindow::shiftsearchlabel(void){
    QDialog *dialog = new  QDialog;
    Ui::searchlabel ui;
    ui.setupUi(dialog);
    QObject::connect(ui.lineEdit,  SIGNAL(textChanged(QString)), this , SLOT(sendnewsearchlabel(QString)));
    dialog->setWindowTitle("Insert new Search label");
    dialog->exec();

}//end shiftsearchlabel

void MainWindow::sendnewsearchlabel(QString label){
    emit changeSearchlabel(label);
    mbaredit->setText(label);
}//end slot

void MainWindow::sndtxtstatusbar(QString txt){
    ui->statusbar->showMessage(txt,1000);
}//end slot
