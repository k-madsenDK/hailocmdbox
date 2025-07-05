#include <queue>
#include <string>
#include <SimpleTimer.h>
#include <107-Arduino-Servo-RP2040.h>

#define maxposx 2000    //max range for servos
#define maxposy 2000
#define minposx 1000
#define minposy 1000
#define minautoposy 1650 //max range for autotracking
#define minautoposx 1000
#define maxautoposy 2000
#define maxautoposx 2000
#define timertimeout 30 
#define maxresetframecount 30

static _107_::Servo servo_0, servo_1;
SimpleTimer simpeltimer;
int autoreset = 0; //counter for autoreset
int servox = 1500 , servoy = 1900;

class hailotxt{
 
    private:
      int width = 0 , height = 0 , track_posx = 0 , track_posy = 0, newposx = 0 , newposy = 0;
      float cx = 0.0 , cy = 0.0;
      bool newpos = false ;
      
      int do_hailotrack(std::string *txt){
        std::string mysubstring = "Width:";
        int found = txt->find(mysubstring);
        if(found != std::string::npos){
            int from = found + mysubstring.size();
            std::string mysubstring = "Heigth:";
            found = txt->find(mysubstring ,from);
            std::string number = txt->substr(from , found - 1 );
            this->width = std::stoi(number);
            from = found + mysubstring.size();
            mysubstring = "cx:";
            found = txt->find(mysubstring ,from);
            number = txt->substr(from , found - 1 );
            this->height = std::stoi(number);
            from = found + mysubstring.size() + 1;
            mysubstring = "cy:";
            found = txt->find(mysubstring ,from);
            number = txt->substr(from , found - 1 );
            this->cx = std::stof(number);
            from = found + mysubstring.size() + 1;
            mysubstring = "bx:";
            found = txt->find(mysubstring ,from);
            number = txt->substr(from , found - 1 );
            this->cy = std::stof(number);
            track_posx = cx * width;
            track_posy = cy * height;
            int hwidth = width / 2 , hheight = height / 2;
            newposx = track_posx - hwidth;
            newposy = track_posy - hheight;
            newpos = true;
            //Serial.println("Ok");           
            return 0;
          }//end if
        return -1;
        }//end do_ha..
        

     void newHorizontalpos(std::string *txt){
        std::string number = txt->substr(9 ,txt->size()-1);
        int pos = std::stoi(number);
        servox = ((pos+1) * 10) + 1000;//servo got 1000 step from 1000 to 2000 and numbers is from 0 to 99
        if(servox < minposx )
          servox = minposx;
        if(servox > maxposx)
          servox = maxposx;
        servo_0.writeMicroseconds(servox);
        manualTracking = true;
      }//end newHorizontalpos

     void newVerticalpos(std::string *txt){
        std::string number = txt->substr(9 ,txt->size()-1);
        int pos = std::stoi(number);
        servoy = ((pos+1) * 10) + 1000;//servo got 1000 step from 1000 to 2000 and numbers is from 0 to 99
        if(servoy < minposy )
          servoy = minposy;
        if(servoy > maxposy)
          servoy = maxposy;
        servo_1.writeMicroseconds(servoy);
        manualTracking = true;
      }//end newVerticalpos

      
    public:
    
      void printcampos(void){
        String txt = "Ncpos:";
          txt += ((servox-minposx)/10)-1;
          txt +=":";
          txt += ((servoy-minposy)/10)-1;
          Serial.println(txt);
        }//end printcampos

     bool  manualTracking = false;
     
     hailotxt(void){}
    //return -1 if nothing done
     bool lost_track = false;
     
     int dohailotxt(std::string *txt){
      if(txt->find("Track:") != std::string::npos and not manualTracking ){
          this->do_hailotrack(txt);
          return 0;
        }//end if
      if(txt->find("New Hpos:") != std::string::npos){
            newHorizontalpos(txt);
            printcampos();
            Serial.println("Ok");
            return 0;
          }//end if
      if(txt->find("New Vpos:") != std::string::npos){
            newVerticalpos(txt);
            printcampos();
            Serial.println("Ok");
            return 0;
          }//end if
      if(txt->find("light on") != std::string::npos){
          digitalWrite(LED_BUILTIN , 1);
          Serial.println("Ok");
          return 0;
        }//end if
      if(txt->find("light off") != std::string::npos){
          digitalWrite(LED_BUILTIN , 0);
          Serial.println("Ok");
          lost_track = true;
          return 0;
        }//end if
      if (txt->find("reset") != std::string::npos){
          lost_track = true;
          manualTracking = false;
          Serial.println("Ok");
          return 0;
        }//end if
        return -1;
      }//end dohailotxt

     bool newposready(void){
        return(newpos);
      }//end newposready
      
     void getnewpos(int *x , int *y){   
      *x = newposx;
      *y = newposy;
      newpos = false;
      }// end getnewpos
  }; //end hailotxt

 void timerint (void){
    ++autoreset;   
   }//end timer int

hailotxt *hailo = new hailotxt();

 
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(LED_BUILTIN , OUTPUT);
  digitalWrite(LED_BUILTIN , LOW);
  servo_0.attach(2);
  servo_1.attach(3);
  simpeltimer.setInterval(timertimeout, timerint);
  servo_0.writeMicroseconds(servox);
  servo_1.writeMicroseconds(servoy);
}

int testnr = 0;
std::string buffer = "";
std::queue<std::string> *resiverstring = new std::queue<std::string>;

void readchar(void){
  char karakter;
  while(Serial.available() > 0){
     karakter = Serial.read();
     buffer += karakter;
     if(karakter == '\n'){
        resiverstring->push(buffer);
        buffer = "";
      }//end if
    }//end while  
  }//end readchar


void loop() {
    simpeltimer.run();
  // put your main code here, to run repeatedly:
    if(Serial.available())
      readchar();
    while(not resiverstring->empty()){
        std::string str = resiverstring->front();
        resiverstring->pop();
        hailo->dohailotxt(&str);
      }//end while
    if(hailo->newposready()){
      int x , y;
        autoreset = 0; //autoreset
        hailo->getnewpos(&x , &y);
        servox += x /15;
        servoy += y /15;
        if(servox < minautoposx )
          servox = minautoposx;
        if(servox > maxautoposx)
          servox = maxautoposx;
        if(servoy < minautoposy )
          servoy = minautoposy;
        if(servoy > maxautoposy)
          servoy = maxautoposy;
       /* std::string serialout = "servox = ";
        serialout += std::to_string(servox);
        serialout += " servoy = ";
        serialout += std::to_string(servoy);
        serialout += "\n";
        Serial.print(serialout.c_str());*/
        servo_0.writeMicroseconds(servox);
        servo_1.writeMicroseconds(servoy);
        hailo->printcampos();
      }//end if
      if((hailo->lost_track or autoreset > maxresetframecount )and not hailo->manualTracking){
        autoreset = 0;//autoreset
        servox = 1500;
        servoy = 1900;
        hailo->lost_track = false;
        servo_0.writeMicroseconds(servox);
        servo_1.writeMicroseconds(servoy);
        hailo->printcampos();
        //Serial.println("lost track or autoreset");
        }//end if
   delay(1);                       // waits 15ms for the servo to reach the position
}
