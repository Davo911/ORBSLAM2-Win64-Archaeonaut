## GoPro Instant Streaming v1.0
##
## By @Sonof8Bits and @KonradIT
##
## 1. Connect your desktop or laptop to your GoPro via WIFI.
## 2. Set the parameters below.
## 3. Run this script.
##
## Supported cameras:
## GoPro HERO5 (incl. Session), HERO4 (incl. Session), HERO+, HERO3+, HERO3, HERO2 w/ WiFi BacPac.
##
## That's all! When done, press CTRL+C to quit this application.
##

import sys
import socket
#from urllib.request import urlopen --> module import error
# https://stackoverflow.com/questions/2792650/python3-error-import-error-no-module-name-urllib2
try:
    # For Python 3.0 and later
    from urllib.request import urlopen
except ImportError:
    # Fall back to Python 2's urllib2
    from urllib2 import urlopen
import subprocess
from time import sleep
import signal
import json
import re
import http
import cv2
import numpy as np
from goprocam import GoProCamera
from goprocam import constants


cascPath="/usr/share/opencv/haarcascades/haarcascade_frontalface_default.xml"
faceCascade = cv2.CascadeClassifier(cascPath)
gpCam = GoProCamera.GoPro()
cap = cv2.VideoCapture("udp://127.0.0.1:10000")


def get_command_msg(id):
	return "_GPHD_:%u:%u:%d:%1lf\n" % (0, 0, 2, 0)

## Parameters:
##
VERBOSE=False
## Sends Record command to GoPro Camera, must be in Video mode!
RECORD=False
##
## Saves the feed to a custom location
SAVE=False
SAVE_FILENAME="goprofeed3"
SAVE_FORMAT="ts"
SAVE_LOCATION="/tmp/"

def gopro_live():
	UDP_IP = "10.5.5.9"
	UDP_PORT = 8554
	KEEP_ALIVE_PERIOD = 2500
	KEEP_ALIVE_CMD = 2

	MESSAGE = get_command_msg(KEEP_ALIVE_CMD)
	URL = "http://10.5.5.9:8080/live/amba.m3u8"
	try:
        # original code - response_raw = urllib.request.urlopen('http://10.5.5.9/gp/gpControl').read().decode('utf-8')
		response_raw = urlopen('http://10.5.5.9/gp/gpControl').read().decode('utf-8')
		jsondata=json.loads(response_raw)
		response=jsondata["info"]["firmware_version"]
	except http.client.BadStatusLine:
		response = urlopen('http://10.5.5.9/camera/cv').read().decode('utf-8')
	if "HD4" in response or "HD3.2" in response or "HD5" in response or "HX" in response or "HD6" in response:
		print("branch HD4")
		print(jsondata["info"]["model_name"]+"\n"+jsondata["info"]["firmware_version"])
		##
		## HTTP GETs the URL that tells the GoPro to start streaming.
		##
		urlopen("http://10.5.5.9/gp/gpControl/execute?p1=gpStream&a1=proto_v2&c1=restart").read()
		if RECORD:
			urlopen("http://10.5.5.9/gp/gpControl/command/shutter?p=1").read()
		print("UDP target IP:", UDP_IP)
		print("UDP target port:", UDP_PORT)
		print("message:", MESSAGE)
		print("Recording on camera: " + str(RECORD))

		## GoPro HERO4 Session needs status 31 to be greater or equal than 1 in order to start the live feed.
		if "HX" in response:
			connectedStatus=False
			while connectedStatus == False:
				req=urlopen("http://10.5.5.9/gp/gpControl/status")
				data = req.read()
				encoding = req.info().get_content_charset('utf-8')
				json_data = json.loads(data.decode(encoding))
				if json_data["status"]["31"] >= 1:
					connectedStatus=True
		##
		## Opens the stream over udp in ffplay. This is a known working configuration by Reddit user hoppjerka:
		## https://www.reddit.com/r/gopro/comments/2md8hm/how_to_livestream_from_a_gopro_hero4/cr1b193
		##
		loglevel_verbose=""
		if VERBOSE==False:
			loglevel_verbose = "-loglevel panic"
		if SAVE == False:
			loglevel_verbose = "-loglevel panic"
			##subprocess.Popen("ffplay " + loglevel_verbose + " -fflags nobuffer -f:v mpegts -probesize 8192 udp://:8554", shell=True)
		else:
			if SAVE_FORMAT=="ts":
				TS_PARAMS = " -acodec copy -vcodec copy "
			else:
				TS_PARAMS = ""
			SAVELOCATION = SAVE_LOCATION + SAVE_FILENAME + "." + SAVE_FORMAT
			print("Recording locally: " + str(SAVE))
			print("Recording stored in: " + SAVELOCATION)
			print("Note: Preview is not available when saving the stream.")
			##subprocess.Popen("ffmpeg -i 'udp://:8554' -fflags nobuffer -f:v mpegts -probesize 8192 " + TS_PARAMS + SAVELOCATION, shell=True)
		if sys.version_info.major >= 3:
			MESSAGE = bytes(MESSAGE, "utf-8")
		print("Press ctrl+C to quit this application.\n")
		while True:
			sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
			sock.sendto(MESSAGE, (UDP_IP, UDP_PORT))
			sleep(KEEP_ALIVE_PERIOD/1000)
	else:
		print("branch hero3"+response)
		if "Hero3" in response or "HERO3+" in response:
			print("branch hero3")
			PASSWORD=urlopen("http://10.5.5.9/bacpac/sd").read()
			print("HERO3/3+/2 camera")
			Password =  str(PASSWORD, 'utf-8')
			text=re.sub(r'\W+', '', Password)
			urlopen("http://10.5.5.9/camera/PV?t=" + text + "&p=%02")
			##subprocess.Popen("ffplay " + URL, shell=True)



def quit_gopro(signal, frame):
	if RECORD:
		urlopen("http://10.5.5.9/gp/gpControl/command/shutter?p=0").read()
	sys.exit(0)

if __name__ == '__main__':
	signal.signal(signal.SIGINT, quit_gopro)
	gopro_live()
