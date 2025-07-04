#include <QtCore>
#include <iostream>
#include "qthread.h"
#include <QThread>
#include <QSettings>

Worker::Worker() { // Constructor
    // you could copy data from constructor arguments to internal variables here.
    this->pipename = "/tmp/hailo";
    //this->cport = new comport::comportACM0((std::string) "/dev/ttyACM1" , (int) B115200);
    this->cport = new comport::comportACM0; // default /dev/ttyacm0 B115200
    init();
    if(pipeindifier != -1){
        piperead = std::thread(&Worker::pipereadthread, this);
        labelsettings.beginGroup("hailo_settings");
        const auto  mylabel = labelsettings.value("srclabel").toString();
        if(not mylabel.isEmpty()){
            mtlabel.lock();
            detval = mylabel.toStdString();
            mtlabel.unlock();
        }//end if
         labelsettings.endGroup();
    }else runmode = false; //quit
}

Worker::~Worker() { // Destructor
    // free resources
    delete cport;
    delete resiverstrings;
}

void Worker::process() { // Process. Start processing data.
    // allocate resources using new here
    this->run();
    emit Qerror("Comport resiver clossed ");
    emit finished();
}

void Worker::errorstopthread(){

}
void Worker::gettrackstring(QString *txt){
    mtlabel.lock();
    *txt = detval.c_str();
    mtlabel.unlock();
}//end gett.....

void Worker::settrackingstring(QString txt){
    mtlabel.lock();
    detval = txt.toStdString();
    mtlabel.unlock();
    labelsettings.beginGroup("hailo_settings");
    labelsettings.setValue("srclabel",txt);
    labelsettings.endGroup();
}//end slot

void Worker::showhailotxt(){
    showHailoTxtString = true;
    QString txt = "Show Hailo txt string";
    emit print(txt);
}//end slot

void Worker::hidehailotxt(){
    showHailoTxtString = false;
    QString txt = "Hide Hailo txt string";
    emit print(txt);
}//end slot

void Worker::autotrackoff(void){
    autotrack = false;
    QString txt = "Autotrack off";
    emit print(txt);
}//end slot

void Worker::autotrackon(void){
    autotrack = true;
    QString txt = "Autotrack on";
    emit print(txt);
}//end slot


void Worker::sendcomporttxt(QString txt){
    cport->sendmytext(txt);
    //emit print(txt);
}//end sendcomporttxt(QString txt)

void Worker::init(void){
    mkfifo(this->pipename.c_str(), 0666);
    this->pipeindifier = open(this->pipename.c_str(), O_RDONLY);
    }//end init

void Worker::pipereadthread( void) {
    mtx.lock();
    this->pipethreadisrunning = true;
    mtx.unlock();
    char buffer = 0;
    std::string resiverline = "";
    int readnumber = 0;

    while(pipeindifier >=0){
        readnumber =read(pipeindifier, &buffer, sizeof(buffer));
        if(readnumber > 0){
            resiverline += buffer;
            if(buffer == '\n'){
                mtx.lock();
                resiverstrings->push(resiverline);
                mtx.unlock();
                resiverline = "";
                }//end if
            }else {
                std::this_thread::sleep_for(std::chrono::microseconds(10));
                }//end if
            }//end while
    }//end piperead thread


void Worker::detection_string(void){
    mtx.lock();
    std::string resvtxt = resiverstrings->front();
    resiverstrings->pop();
    mtx.unlock();
            //std::cout << resvtxt  ;
    if(showHailoTxtString){
        QString txt = resvtxt.c_str();
        txt.resize(txt.size()-1);
        emit print(txt);
    }//end if
    std::string substring = "Frame count:";
    int found = resvtxt.find(substring);
    if(found == 0){
        QString txt = resvtxt.c_str();
        emit statusbarinsert(txt);
        newframe = true;
        int from = substring.size() + 1;
        substring = "Width:";
        found = resvtxt.find(substring ,from);
        std::string number = resvtxt.substr(from , found - 1 );
        this->framecount = std::stoi(number);
        from  = found + substring.size() + 1;
        substring = "Heigth:";
        found = resvtxt.find(substring , from);
        number = resvtxt.substr(from , found - 1 );
        this->width = std::stoi(number);
        from  = found + substring.size() + 1;
        number = resvtxt.substr(from , resvtxt.size()-1);
        this->heigth = std::stoi(number);
        //std::cout << "Framenumber : " << this->framecount << " width : " << this->width << " heigth : " << this->heigth << std::endl;
            }else{
            substring = "Label:";
            found = resvtxt.find(substring);
            int from = substring.size() + 1;
            if(found == 0){
                labeldetekt = true;
                from = substring.size() + 1;
                substring = "ID:";
                found = resvtxt.find(substring );
                this->label = resvtxt.substr(from , (found - 1) - from );
                from  = found + substring.size() + 1;
                substring = "Confidence:";
                found = resvtxt.find(substring , from);
                std::string number = resvtxt.substr(from , found - 1 );
                this->id = std::stoi(number);
                from  = found + substring.size() + 1;
                substring = "Detection count:";
                found = resvtxt.find(substring, from);
                number = resvtxt.substr(from , found - 1 );
                this->confidence = std::stof(number);
                from  = found + substring.size() + 1;
                substring = "Position:";
                found = resvtxt.find(substring , from);
                number = resvtxt.substr(from , found - 1 );
                this->countnumber = std::stoi(number);
                substring = "center=(";
                found = resvtxt.find(substring , from);
                from  = found + substring.size() + 1;
                substring = ",";
                found = resvtxt.find(substring , from);
                number = resvtxt.substr(from , found - 1 );
                this->center_x = std::stof(number);
                from = found + 1;
                substring = ")";
                found = resvtxt.find(substring , from);
                number = resvtxt.substr(from , found - 1 );
                this->center_y = std::stof(number);
                from = found+1;
                substring = "xmin=";
                found = resvtxt.find(substring , from);
                from = found + substring.size();
                substring = ",";
                found = resvtxt.find(substring , from);
                number = resvtxt.substr(from , found - 1 );
                this->xmin = std::stof(number);
                from = found+1;
                substring = "ymin=";
                found = resvtxt.find(substring , from);
                from = found + substring.size();
                substring = ",";
                found = resvtxt.find(substring , from);
                number = resvtxt.substr(from , found - 1 );
                this->ymin = std::stof(number);
                from = found+1;
                substring = "xmax=";
                found = resvtxt.find(substring , from);
                from = found + substring.size();
                substring = ",";
                found = resvtxt.find(substring , from);
                number = resvtxt.substr(from , found - 1 );
                this->xmax = std::stof(number);
                from = found+1;
                substring = "ymax=";
                found = resvtxt.find(substring , from);
                from = found + substring.size();
                number = resvtxt.substr(from , resvtxt.size());
                this->ymax = std::stof(number);
                        //std::cout << "label : " << this->label << " id : " << this->id << " conf : " << this->confidence << " Detection count number : " << this->countnumber << " center x : " << this->center_x << " center y : " << this->center_y << " xmin : " << this->xmin << " ymin : " << this->ymin << " xmax : " << this->xmax << " ymax : " << this->ymax << std::endl << std::endl;
                } else {
                    substring = "Shutting down...";
                    found = resvtxt.find(substring);
                    if(found == 0){
                        runmode = false; //quit
                        std::cout <<"\nend of program \n";
                        return; // end of program
                    }//end if
            }//end if
        }//end if
} //enddetection_string


void Worker::label_do_something(void){
    labeldetekt = false;
    mtlabel.lock();//detval can be set from main thread
    std::string srclabel = detval;
    mtlabel.unlock();
    if(this->label == srclabel){
        person_label = true;
        if(this->trackperson ){
            if(this->lastconfidence < confidence){
                lastconfidence = confidence;
                this->person_track_detect = true;
                this->person_track_x = this->center_x;
                this->person_track_y = this->center_y ;
                this->person_track_xmax = this->xmax - this->xmin;
                this->person_track_ymax = this->ymax - this->ymin;
            }//end if
        }//end if
    }//end if
}//end label_do_something

void Worker::new_frame(void){
    std::string datatxt = "";
    newframe = false;
    lastconfidence = 0.0;
          //std::cout << "frame number : " << this->framecount << std::endl;
    if(person_label){
        person_label = false;
        if(this->person_count < max_count and this->confidence > 0.40){
            ++this->person_count;
        }else{
            if(this->person_count > 0)
                person_count--;
        }//end if
        if(this->person_count == max_count){
            if(this->light_on == false){
                QString txt = "light on";
                emit print(txt);
                //std::cout << "light on" << std::endl;
                datatxt = "light on \n";
                cport->senddatalinie(&datatxt);
                this->light_on = true;
                mtlabel.lock();//detval can be set from main thread
                txt = detval.c_str();
                mtlabel.unlock();
                txt += " detected";
                emit print(txt);
                //std::cout << "person detected";
            }//  end if
        } //end if
        if(this->person_track_detect && autotrack == true){
            datatxt = "Track:";
            datatxt += "Width:";
            datatxt += std::to_string(this->width);
            datatxt += "Heigth:";
            datatxt += std::to_string(this->heigth);
            datatxt += "cx:";
            datatxt += std::to_string(this->person_track_x);
            datatxt += "cy:";
            datatxt += std::to_string(this->person_track_y );
            datatxt += "bx:";
            datatxt +=std::to_string(this->person_track_xmax);
            datatxt += "by:";
            datatxt += std::to_string(this->person_track_ymax);
            datatxt += "\n";
            cport->senddatalinie(&datatxt);
            //std::cout << datatxt;
        }//end if
    }else{
        if(this->person_count > 0)
            person_count--;
        }//end if
        //std::cout << "Person Count : " << person_count << std::endl;
    if(person_count == 0 and light_on == true){
        QString txt = "light off";
        emit print(txt);
        //std::cout << "light off" << std::endl;
        datatxt = "light off\n";
        cport->senddatalinie(&datatxt);
        this->light_on = false;
        mtlabel.lock();//detval can be set from main thread
        txt = detval.c_str();
        mtlabel.unlock();
        txt += " NOT detected ";
        emit print(txt);
        //std::cout << "person NOT detected \n";
    }//end if
    this->person_track_detect = false;
    int person_track_id = INT_MAX;
}//end new_frame

void Worker::checkpicotext(std::string *txt){
    if(txt->find("Ncpos:") == 0){
        std::string substring = ":";
        int found = txt->find(substring );
        std::string number = txt->substr(6 , found);
        int newposx = std::stoi(number);
        number = txt->substr(found+1 , txt->size()-1);
        int newposy = std::stoi(number);
        emit newpicox(newposx);
        emit newpicoy(newposy);
    }else{
        QString qtxt = "Comport : ";
        txt->resize(txt->size() - 2);//remove \n
        qtxt += txt->c_str();
        emit print(qtxt);
    } //end if
}//end checkpicotext

void Worker::run(void){
    if(pipeindifier == -1) runmode = false;
    while(this->runmode == true){
        while(not resiverstrings->empty()){
            detection_string();
            if(newframe){
                new_frame();
            } else if(labeldetekt){
                label_do_something();
            }//end if
            if(not cport->resvempty()){
                std::string txt = "";
                cport->hentdatalinie(&txt);
                checkpicotext(&txt);
                //std::cout << "Comport : " << txt;
            }//end if
        }//end while
        std::this_thread::sleep_for(std::chrono::microseconds(10));
    }//end while
 }//end run
