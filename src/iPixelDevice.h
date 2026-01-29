#pragma once


#include <Arduino.h>
#include <ArduinoJson.h> // Nodig voor het parsen van de JSON commando's
#include <NimBLEDevice.h>
#include "Constants.h"
#include <cstdint>
#include <map>
#include <queue>
#include <string>
#include <vector>
#include "utils/TimeMeter.h"

extern NimBLEUUID serviceUUID;
extern NimBLEUUID charUUID;

class iPixelDevice : public NimBLEClientCallbacks {
private:
	int timerSeconds = 0;
	bool isRunning = false;
	unsigned long lastTick = 0;
	bool LEDstate = true ;



private:
	std::queue<std::string> commandQueue; // De queue is hier gedeclareerd
	enum DeviceState { IDLE, CONNECTING, WAITING_FOR_POST, READY, ERROR };
	DeviceState _state = IDLE;

	enum KookwekkkerState { WEKKERIDLE, SETTING, RUNNING, ALARM };
	KookwekkkerState _kookwekkkerState = WEKKERIDLE;

public:
    NimBLEAddress address;
    NimBLEClient *client = nullptr;
    NimBLERemoteService *service = nullptr;
    NimBLERemoteCharacteristic *characteristic = nullptr;
    boolean connected = false;          // BLE connection
    boolean connecting = false;         // Connect started
    boolean dopostconnect = false ;
    void* context_data;
    // 14 Dec Optimalisatie: MTU bepalen
    uint16_t chunkSize = 200 ;

    // properties
    MatrixMode mode = MODE_NONE ;

	TimeMeter functionmeter;
	TimeMeter blemeter;

	uint32_t lastNodeRedID;

	boolean timerRunning = false ;
	boolean timerSetting = false ;
	uint16_t timer = 0 ;
	uint16_t timersettime = 0 ;
	unsigned long starttimertime = 0 ;
	unsigned long lastactivity = 0 ;

	uint16_t lastdisplaytime = 999 ;


    explicit iPixelDevice(NimBLEAddress pAddress) ;
    void printPrefix();

    // queue with received commands
    void enqueueCommand(const JsonDocument& doc);

    //BLEClientCallbacks
    //void onConnect(NimBLEClient *pClient);
    void onConnect(NimBLEClient *pClient);
    void onDisconnect(NimBLEClient *pClient);
    void onConnectFail(NimBLEClient* pClient, int reason);

    //Methods
    void connectAsync();
    void postconnect() ;

    void update(); // Deze roep je aan in de hoofd-loop
    //Queue
    std::vector<std::vector<uint8_t>> queue;
    void queueTick();
    void queuePush(std::vector<uint8_t> command);

    // command queue
    void processQueue() ;

    // kookwekker
	void handleButtons() ;
	void processTimerCommand(StaticJsonDocument<4096>& doc) ;


	// wekker/kookwekker display functions
    void handleTimerLogic() ;
	void handleWekker() ;

    void showTime( int seconds ) ;
    void showClock( int hour, int min, int seconds ) ;


    //Commands
    void sendImage();
    void setTime(int hour, int minute, int second);
    void setFunMode(bool value);
    void setOrientation(int orientation);
    void clear();
    void setBrightness(int brightness);
    void setSpeed(int speed);
    void setLED(boolean on);
    void deleteScreen(int screen);
    void setPixel(int x, int y, uint8_t r, uint8_t g, uint8_t b);
    void setClockMode(int style, int dayOfWeek, int year, int month, int day, bool showDate, bool format24);
    void setRhythmLevelMode(int style, const int levels[11]);
    void setRhythmAnimationMode(int style, int frame);
    void sendText(const String& text, int animation, int save_slot, int speed, uint8_t colorR, uint8_t colorG, uint8_t colorB, int rainbow_mode, int matrix_height);
    void sendPNG(const std::vector<uint8_t> &pngData);
    void sendGIF(const std::vector<uint8_t> &gifData);

};