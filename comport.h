#ifndef COMPORT_H
#define COMPORT_H
#pragma once

#include <iostream>
#include <mutex>
#include <thread>
#include <string>


#include <queue>
#include <chrono>


// Linux headers
#include <fcntl.h> // Contains file controls like O_RDWR
#include <errno.h> // Error integer and strerror() function
#include <termios.h> // Contains POSIX terminal control definitions
#include <unistd.h> // write(), read(), close()
#include <cstring>
#include <sys/file.h>

//qt header
#include <QObject>
#include <QString>

namespace comport {

    class comportACM0 : public QObject{
          Q_OBJECT
        private:
        std::string port ;
        int serial_port = 0;
        int hastighed = 0;
        bool resiverthreadrunning = false;
        std::queue<std::string> *resiverstring = new std::queue<std::string>;
        void signal_handler_IO (int status); //for initialising
        std::thread resv;
        std::mutex mtx;
        int portsetup(void);
        void resiverthread();
        protected:



    public:

        comportACM0(std::string portnavn = "/dev/ttyACM0" , int speed = B115200) { //default ACM0
                this->port = portnavn;
                this->hastighed = speed;
                resv = std::thread(&comportACM0::resiverthread, this);
            }
        ~comportACM0(void) { //default ACM0
            flock(serial_port , LOCK_UN);
            close(this->serial_port);
            delete(resiverstring);
            }
        int  resvsize(void);
        bool resvempty (void);
        bool isresvrunning(void);
        int hentdatalinie(std::string *data);
        int senddatalinie(std::string *data);
        void sendmytext(QString txt);

signals:
        void Qerror(QString err);
        void print(QString txt);
        void errorstopthread();

    };//end class
}//end of namespace
#endif // COMPORT_H
