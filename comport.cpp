#include "comport.h"


int  comport::comportACM0::resvsize(void){
        return resiverstring->size();
        }
bool comport::comportACM0::resvempty (void){
        return this->resiverstring->empty();
        }// end resvempty
		
bool comport::comportACM0::isresvrunning(void){
        return resiverthreadrunning;
        }//end isre**
			
int comport::comportACM0::hentdatalinie(std::string *data){
    if (not this->resiverstring->empty()){
        mtx.lock();
        *data = this->resiverstring->front();
        this->resiverstring->pop();
        mtx.unlock();
        }//end if
        return data->size();
    }//end hent data linie
			
int comport::comportACM0::senddatalinie(std::string *data){
    int ret = 0;
    if(serial_port > 0)
        ret = write(serial_port, data->c_str(), data->size());
    return ret;
    }//end senddata

int comport::comportACM0::portsetup(void){
         // Open the serial port. Change device path as needed (currently set to an standard FTDI USB-UART cable type device)+
    serial_port = open(port.c_str(), O_RDWR );
    int error = flock(serial_port , LOCK_EX);
    // Create new termios struct, we call it 'tty' for convention
    QString txt = "portnavn : ";
    txt += port.c_str();
    txt += " Serial port : ";
    txt += serial_port;
    emit print(txt);
    //std::cout << "portnavn : " << port << " Serial port : " << serial_port << std::endl;
			
    struct termios tty;
			
    // Read in existing settings, and handle any error
    if(tcgetattr(serial_port, &tty) != 0) {
        std::cerr << "Error from tcgetattr: " << errno << std::endl;
        return 1;
        }

    tty.c_cflag &= ~PARENB; // Clear parity bit, disabling parity (most common)
    tty.c_cflag &= ~CSTOPB; // Clear stop field, only one stop bit used in communication (most common)
    tty.c_cflag &= ~CSIZE; // Clear all bits that set the data size
    tty.c_cflag |= CS8; // 8 bits per byte (most common)
    tty.c_cflag &= ~CRTSCTS; // Disable RTS/CTS hardware flow control (most common)
    tty.c_cflag |= CREAD | CLOCAL; // Turn on READ & ignore ctrl lines (CLOCAL = 1)

    tty.c_lflag &= ~ICANON;
    tty.c_lflag &= ~ECHO; // Disable echo
    tty.c_lflag &= ~ECHOE; // Disable erasure
    tty.c_lflag &= ~ECHONL; // Disable new-line echo
    tty.c_lflag &= ~ISIG; // Disable interpretation of INTR, QUIT and SUSP
    tty.c_iflag &= ~(IXON | IXOFF | IXANY); // Turn off s/w flow ctrl
    tty.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL); // Disable any special handling of received bytes

    tty.c_oflag &= ~OPOST; // Prevent special interpretation of output bytes (e.g. newline chars)
    tty.c_oflag &= ~ONLCR; // Prevent conversion of newline to carriage return/line feed
    // tty.c_oflag &= ~OXTABS; // Prevent conversion of tabs to spaces (NOT PRESENT ON LINUX)
    // tty.c_oflag &= ~ONOEOT; // Prevent removal of C-d chars (0x004) in output (NOT PRESENT ON LINUX)
    tty.c_cc[VTIME] = 10;    // Wait for up to 1s (10 deciseconds), returning as soon as any data is received.
    tty.c_cc[VMIN] = 0;

    // Set in/out baud rate to be 115200
    cfsetspeed(&tty, B115200);
    //cfsetospeed(&tty, B115200);
    //resv= std::thread([this]() { resiverthread(); });
    //resv = std::thread(&comportACM0::resiverthread, this);
    // Save tty settings, also checking for error
    if (tcsetattr(serial_port, TCSANOW, &tty) != 0) {
        txt = "Error from tcsetattr: ";
        txt += errno;
        //std::cerr << "Error from tcsetattr: " << errno << std::endl;
        emit Qerror(txt);
        emit errorstopthread();
        return 1;
        }
    return 0;
    }

void comport::comportACM0::resiverthread()
{
    if(portsetup() == 0)
        this->resiverthreadrunning = true;
    char buffer ;
    std::string modtagerlinie = "";
    int modtaget = 0;
    while(modtaget >= 0){
        //int num_bytes = read(serial_port, &read_buf, sizeof(read_buf));
        modtaget = read(this->serial_port , &buffer , 1);
        if(modtaget > 0){
        modtagerlinie += buffer;
        //std::cout << buffer;
            if(buffer == '\n'){
                mtx.lock();
                this->resiverstring->push(modtagerlinie);
                mtx.unlock();
                modtagerlinie = "";
                }//end if
            }//end if
        }//end while
    QString txt = "Error reading: ";
    txt +=  errno;
    txt += " \nResiver thread stopper";
    emit Qerror(txt);
    emit errorstopthread();
    //std::cout << "Error reading: " << errno << " \nResiver thread stopper " << std::endl;
}//end resiverthread()

void comport::comportACM0::sendmytext(QString txt){
    std::string stxt = txt.toStdString();
    stxt += "\n";
    this->senddatalinie(&stxt);
}//end sendtext
