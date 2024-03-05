#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <Adafruit_Sensor.h>
#include <stdlib.h>
#include <DHT.h>
#include <DHT_U.h>

BLEServer* pServer = NULL;
BLECharacteristic* pCharacteristic = NULL;
bool deviceConnected = false;
bool oldDeviceConnected = false;
unsigned long previousMillis = 0;
const long interval = 1000;  // 每1秒读取一次数据

// DHT11传感器的引脚和类型
#define DHTPIN 3  // 根据您的实际连接修改
#define DHTTYPE DHT11  // 使用的DHT传感器类型
DHT dht(DHTPIN, DHTTYPE);

// 土壤湿度传感器的引脚
#define SOIL_MOISTURE_PIN 1  // 根据您的实际连接修改


#define SERVICE_UUID        "97f8052f-babb-450d-870c-18521bf58b80"
#define CHARACTERISTIC_UUID "f194dd23-4f05-4ae9-903c-8ba0be10d4b0"


class MyServerCallbacks : public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
        deviceConnected = true;
    };

    void onDisconnect(BLEServer* pServer) {
        deviceConnected = false;
    }
};

void setup() {
    Serial.begin(115200);
    dht.begin();
    pinMode(SOIL_MOISTURE_PIN, INPUT);

    BLEDevice::init("XIAO_ESP32S3");
    pServer = BLEDevice::createServer();
    pServer->setCallbacks(new MyServerCallbacks());
    BLEService *pService = pServer->createService(SERVICE_UUID);
    pCharacteristic = pService->createCharacteristic(
        CHARACTERISTIC_UUID,
        BLECharacteristic::PROPERTY_READ |
        BLECharacteristic::PROPERTY_WRITE |
        BLECharacteristic::PROPERTY_NOTIFY
    );
    pCharacteristic->addDescriptor(new BLE2902());
    pService->start();
    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(SERVICE_UUID);
    pAdvertising->setScanResponse(true);
    pAdvertising->setMinPreferred(0x06);
    pAdvertising->setMinPreferred(0x12);
    BLEDevice::startAdvertising();
    Serial.println("Characteristic defined! Now you can read it in your phone!");
}


void loop() {
    // 从DHT11读取温度和湿度
    float humidity = dht.readHumidity();
    float temperature = dht.readTemperature();

    // 从土壤湿度传感器读取湿度值
    int soilMoistureValue = analogRead(SOIL_MOISTURE_PIN);

    // 打印读取到的数据到串口
    Serial.print("Humidity: ");
    Serial.print(humidity);
    Serial.print(" %\t");
    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.println(" *C");
    Serial.print("Soil Moisture: ");
    Serial.println(soilMoistureValue);  
    
    if (deviceConnected) {
        float humidity = dht.readHumidity();
        float temperature = dht.readTemperature();
        int soilMoistureValue = analogRead(SOIL_MOISTURE_PIN);

        // 构造要发送的字符串
        String dataString = String("H: ") + humidity + " %, " + 
                            "T: " + temperature + " *C, " + 
                            "SM: " + soilMoistureValue;

        // 发送数据到BLE客户端
        pCharacteristic->setValue(dataString.c_str());
        pCharacteristic->notify();
        Serial.println("Notify value: " + dataString);

        delay(interval);  // 根据需要调整延迟
    }
    
    // 更新设备连接状态
    if (!deviceConnected && oldDeviceConnected) {
        delay(500);  // give the bluetooth stack the chance to get things ready
        pServer->startAdvertising();  // advertise again
        oldDeviceConnected = deviceConnected;
    }
    if (deviceConnected && !oldDeviceConnected) {
        oldDeviceConnected = deviceConnected;
    }
    delay(1000);
}




// #include <Arduino.h>
// #include <BLEDevice.h>
// #include <BLEServer.h>
// #include <BLEUtils.h>
// #include <BLE2902.h>
// #include <Adafruit_Sensor.h>
// #include <stdlib.h>
// #include <DHT.h>
// #include <DHT_U.h>

// // DHT11传感器的引脚和类型
// #define DHTPIN 3  // 根据您的实际连接修改
// #define DHTTYPE DHT11  // 使用的DHT传感器类型
// DHT dht(DHTPIN, DHTTYPE);

// // 土壤湿度传感器的引脚
// #define SOIL_MOISTURE_PIN 1  // 根据您的实际连接修改

// BLEServer* pServer = NULL;
// BLECharacteristic* pCharacteristic = NULL;
// bool deviceConnected = false;
// bool oldDeviceConnected = false;
// unsigned long previousMillis = 0;
// const long interval = 1000;  // 每1秒读取一次数据

// #define SERVICE_UUID        "97f8052f-babb-450d-870c-18521bf58b80"
// #define CHARACTERISTIC_UUID "f194dd23-4f05-4ae9-903c-8ba0be10d4b0"

// class MyServerCallbacks : public BLEServerCallbacks {
//     void onConnect(BLEServer* pServer) {
//         deviceConnected = true;
//     };

//     void onDisconnect(BLEServer* pServer) {
//         deviceConnected = false;
//     }
// };

// void setup() {
//     Serial.begin(115200);
//     dht.begin();
//     pinMode(SOIL_MOISTURE_PIN, INPUT);

//     BLEDevice::init("XIAO_ESP32S3");
//     pServer = BLEDevice::createServer();
//     pServer->setCallbacks(new MyServerCallbacks());
//     BLEService *pService = pServer->createService(SERVICE_UUID);
//     pCharacteristic = pService->createCharacteristic(
//         CHARACTERISTIC_UUID,
//         BLECharacteristic::PROPERTY_READ |
//         BLECharacteristic::PROPERTY_WRITE |
//         BLECharacteristic::PROPERTY_NOTIFY
//     );
//     pCharacteristic->addDescriptor(new BLE2902());
//     pService->start();
//     BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
//     pAdvertising->addServiceUUID(SERVICE_UUID);
//     pAdvertising->setScanResponse(true);
//     pAdvertising->setMinPreferred(0x06);
//     pAdvertising->setMinPreferred(0x12);
//     BLEDevice::startAdvertising();
//     Serial.println("Characteristic defined! Now you can read it in your phone!");
// }


// void loop() {
//     if (deviceConnected) {
//         float humidity = dht.readHumidity();
//         float temperature = dht.readTemperature();
//         int soilMoistureValue = analogRead(SOIL_MOISTURE_PIN);

//         // 构造要发送的字符串
//         String dataString = String("H: ") + humidity + " %, " + 
//                             "T: " + temperature + " *C, " + 
//                             "SM: " + soilMoistureValue;

//         // 发送数据到BLE客户端
//         pCharacteristic->setValue(dataString.c_str());
//         pCharacteristic->notify();
//         Serial.println("Notify value: " + dataString);

//         delay(interval);  // 根据需要调整延迟
//     }
    
//     // 更新设备连接状态
//     if (!deviceConnected && oldDeviceConnected) {
//         delay(500);  // give the bluetooth stack the chance to get things ready
//         pServer->startAdvertising();  // advertise again
//         oldDeviceConnected = deviceConnected;
//     }
//     if (deviceConnected && !oldDeviceConnected) {
//         oldDeviceConnected = deviceConnected;
//     }
// }





