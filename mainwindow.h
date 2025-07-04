#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "mainwindow.h"
#include <QSettings>
#include <QLineEdit>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:

    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

signals:
    void sendmytxt(QString name);
    void stopthread(void);
    void stopautofocus(void);
    void startautofocus(void);
    void changeSearchlabel(QString label);

private slots:
    void on_textBrowser_anchorClicked(const QUrl &arg1);
    void on_Qtimerrun(void);
    void on_Threadmsg(QString err);
    void text_return(void);
    void camerahorisontal(void);
    void cameravertical(void);
    void newHpos(int value);
    void newVpos(int value);
    void camreset(void);
    void cameraboth(void);
    void piconewposx(int posx);
    void piconewposy(int posx);
    void shiftsearchlabel(void);
    void sendnewsearchlabel(QString label);
    void sndtxtstatusbar(QString txt);

private:
    Ui::MainWindow *ui;
    bool threadrunning = true;
    void threadfinish(){ threadrunning = false;}
    int camposx = 0 , camposy = 0;
    QSettings labelsettings;
    QLineEdit* mbaredit = new QLineEdit(this); //menu bar edit showing searchlabel
};
#endif // MAINWINDOW_H
