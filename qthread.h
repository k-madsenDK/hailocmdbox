#pragma once

#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<unistd.h>
#include<fcntl.h>
#include<sys/stat.h>


//c++
#include <cstring>
#include <chrono>
#include <iostream>
#include <cstdio>
#include "comport.h"
#include <climits>
//#include <bits/stdc++.h>

#define max_count 10

#include <QThread>
#include <QSettings>

class Worker : public QObject {
    Q_OBJECT
private:
std::string pipename = "", label = "";
int pipeindifier = -1 , id = -1 , countnumber = 0 ;
comport::comportACM0 *cport;
std::thread piperead ;

bool pipethreadisrunning = false;
std::mutex mtx , mtxx , mtlabel;
bool runmode = true , labeldetekt = false ,newframe = false, trackperson = true;
ulong framecount = 0;
int width = 0 , heigth = 0;
float confidence = 0.0 ,lastconfidence = 0.0, center_x = 0.0 , center_y = 0.0 , xmin = 0.0 , ymin = 0.0 , xmax = 0.0 , ymax = 0.0;
int person_count = 0;
bool light_on = false , person_label = false;
std::queue<std::string> *resiverstrings = new std::queue<std::string>;
int person_track_id = INT_MAX;
float person_track_x , person_track_y , person_track_xmax , person_track_ymax;
bool person_track_detect = false;
bool autotrack = true , showHailoTxtString = false;
std::string detval = "person"; // remember mutex mtlabel on access to this string
QSettings labelsettings;

void init(void);
void pipereadthread( void);
void detection_string(void);
void label_do_something(void);
void new_frame(void);
void newstdinputline(void);
void checkpicotext(std::string *txt);

public:
    Worker();
    ~Worker();
    void run(void);
    void gettrackstring(QString *txt);

public slots:
    void process();
    void errorstopthread();
    void sendcomporttxt(QString txt);
    void autotrackoff(void);
    void autotrackon(void);
    void showhailotxt();
    void hidehailotxt();
    void stopthread(){runmode = false;}
    void settrackingstring(QString txt);

signals:
    void finished();
    void Qerror(QString err);
    void print(QString txt);
    void newpicox(int posx);
    void newpicoy(int posy);
    void statusbarinsert(QString txt);

 };//end class
