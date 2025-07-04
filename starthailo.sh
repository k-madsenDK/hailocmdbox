#!/bin/bash
. setup_env.sh 
#hailopipe/hailopipe &    
hailocmdbox/build-hailocmdbox-Desktop-Release/Qthailopipe &
python3 basic_pipelines/madsen.py --hef-path resources/yolov11s.hef --input rpi --show-fps --disable-sync >> /tmp/hailo 
rm /tmp/hailo



