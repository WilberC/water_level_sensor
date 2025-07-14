#include <Arduino.h>
#include <WiFi.h>
#include <WiFiManager.h>
#include <Preferences.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

// Pin definitions for ESP32
#define TRIG_PIN 13
#define ECHO_PIN 12
#define CONFIG_BUTTON_PIN 0  // GPIO0 (BOOT button on most ESP32 boards)

// WiFi Manager
WiFiManager wifiManager;
Preferences preferences;
bool wifiConnected = false;
bool forceConfigPortal = false;

// HTTP Configuration
const char* homeAssistantUrl = "https://domain.com";  // Your Home Assistant domain
const char* sensorEntityId = "sensor.water_level_sensor";  // Entity ID for the sensor

// Device Configuration - Change these for each sensor
const char* deviceName = "Water Level Sensor";  // Change to: "3rd Floor Tank" or "1st Floor Tank"
const char* deviceLocation = "tank_3f";  // Change to: "tank_3f" or "tank_1f"
const char* deviceId = "water_sensor_3f";  // Change to: "water_sensor_3f" or "water_sensor_1f"

// Custom parameters for WiFiManager
WiFiManagerParameter custom_ha_url("ha_url", "Home Assistant URL", homeAssistantUrl, 100);
WiFiManagerParameter custom_ha_token("ha_token", "Home Assistant Access Token", "", 200);
WiFiManagerParameter custom_ha_entity("ha_entity", "Sensor Entity ID", sensorEntityId, 50);
WiFiManagerParameter custom_device_name("device_name", "Device Name", deviceName, 50);
WiFiManagerParameter custom_device_location("device_location", "Device Location", deviceLocation, 20);

// Calibration data
struct CalibrationData {
  float tankHeight;      // Total tank height in cm
  float emptyDistance;   // Distance when tank is empty
  float fullDistance;    // Distance when tank is full
  float tankDiameter;    // Tank diameter in cm (for volume calculation)
  bool isCalibrated;     // Calibration status
};

CalibrationData calibration;

// Water consumption monitoring
struct WaterConsumption {
  float currentVolume;           // Current volume in liters
  float previousVolume;          // Previous volume reading
  float consumption15min;        // Consumption in last 15 minutes (L/15min)
  float consumption30min;        // Consumption in last 30 minutes (L/30min)
  float hourlyConsumption;       // Consumption in last hour (L/hour)
  float dailyConsumption;        // Consumption in last 24 hours (L/day)
  float averageDailyConsumption; // Average daily consumption
  unsigned long lastUpdateTime;  // Last update timestamp
  unsigned long last15minTime;   // Last 15-minute calculation
  unsigned long last30minTime;   // Last 30-minute calculation
  unsigned long lastHourTime;    // Last hourly calculation
  unsigned long lastDayTime;     // Last daily calculation
  bool leakDetected;             // Leak detection flag
  bool lowLevelAlert;            // Low level alert flag
  bool highConsumptionAlert;     // High consumption alert flag
  bool rapidLeakAlert;           // Rapid leak alert flag (15min)
  bool moderateLeakAlert;        // Moderate leak alert flag (30min)
};

WaterConsumption consumption;

// Alert thresholds
struct AlertThresholds {
  float lowLevelThreshold;       // Low level alert threshold (%)
  float rapidLeakThreshold;      // Rapid leak detection (L/15min)
  float moderateLeakThreshold;   // Moderate leak detection (L/30min)
  float highConsumptionThreshold; // High consumption alert (L/hour)
  float leakDetectionThreshold;   // Leak detection threshold (L/hour)
  int alertCooldownMinutes;      // Alert cooldown period
};

AlertThresholds alerts;

// Sensor data
struct SensorData {
  int distance;
  int waterLevel;
  int percentage;
  float volume;          // Volume in liters
  float signalStrength;
  bool isConnected;
  unsigned long timestamp;
};

SensorData currentData;

// Function declarations
bool connectToWiFi();
void setupWiFi();
void startConfigPortal();
bool sendToHomeAssistant();
void setupHTTP();
void saveCustomParameters();
void loadCustomParameters();
void checkConfigButton();
void clearAllSettings();
void loadCalibration();
void calculateWaterLevel();
void createCalibrationEntities();
void monitorWaterConsumption();
void checkAlerts();
void createAlertEntities();
void sendAlert(String alertType, String message);
void testUltrasonicSensor();

void setup() {
  Serial.begin(115200);
  delay(2000);
  
  Serial.println("=== ESP32 Water Level Sensor - Smart Monitoring ===");
  Serial.println("Step 1: Serial initialized");
  
  // Configure pins step by step
  Serial.println("Step 2: Configuring ECHO pin...");
  pinMode(ECHO_PIN, INPUT);
  Serial.println("Step 3: Configuring TRIG pin...");
  pinMode(TRIG_PIN, OUTPUT);
  digitalWrite(TRIG_PIN, LOW);
  Serial.println("Step 4: Configuring CONFIG button pin...");
  pinMode(CONFIG_BUTTON_PIN, INPUT_PULLUP);
  Serial.println("Step 5: Pins configured");
  
  // Check for configuration button press
  Serial.println("Step 6: Checking for configuration button...");
  checkConfigButton();
  
  // Initialize preferences
  Serial.println("Step 7: Initializing preferences...");
  preferences.begin("wifi-config", false);
  
  // Setup WiFi with secure manager
  Serial.println("Step 8: Setting up secure WiFi...");
  setupWiFi();
  
  // Setup HTTP
  Serial.println("Step 9: Setting up HTTP communication...");
  setupHTTP();
  
  // Load calibration data
  Serial.println("Step 10: Loading calibration data...");
  loadCalibration();
  
  // Load alert thresholds
  Serial.println("Step 11: Loading alert thresholds...");
  loadAlertThresholds();
  
  // Create calibration entities in Home Assistant
  Serial.println("Step 12: Creating calibration entities...");
  createCalibrationEntities();
  
  // Create alert entities in Home Assistant
  Serial.println("Step 13: Creating alert entities...");
  createAlertEntities();
  
  Serial.println("✅ Sensor initialized successfully!");
  Serial.println("Starting smart water level monitoring with alerts...");
  Serial.println();
}

void loop() {
  static int counter = 0;
  static bool sensorTested = false;
  
  Serial.print("Measurement #");
  Serial.println(counter++);
  
  // Check WiFi connection
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("⚠️  WiFi disconnected, attempting to reconnect...");
    connectToWiFi();
  } else {
    Serial.print("📶 WiFi Connected - IP: ");
    Serial.println(WiFi.localIP());
  }
  
  // Run sensor test on first iteration
  if (!sensorTested) {
    testUltrasonicSensor();
    sensorTested = true;
    Serial.println("---");
  }
  
  // Simple distance measurement
  int distance = measureDistance();
  
  if (distance > 0) {
    Serial.print("Distance: ");
    Serial.print(distance);
    Serial.println(" cm");
    
    // Calculate water level using calibration
    calculateWaterLevel(distance);
    
    Serial.print("Water Level: ");
    Serial.print(currentData.percentage);
    Serial.print("% (");
    Serial.print(currentData.waterLevel);
    Serial.println(" cm)");
    
    if (calibration.isCalibrated) {
      Serial.print("Volume: ");
      Serial.print(currentData.volume);
      Serial.println(" L");
    }
    
    // Monitor water consumption and check alerts
    monitorWaterConsumption();
    checkAlerts();
    
    // Send to Home Assistant
    sendToHomeAssistant();
    
  } else {
    Serial.print("❌ Measurement failed! Error code: ");
    Serial.println(distance);
    
    // Provide specific error messages
    switch (distance) {
      case -1:
        Serial.println("   → No echo received (timeout)");
        Serial.println("   → Check sensor wiring and power");
        break;
      case -2:
        Serial.println("   → Echo too short");
        Serial.println("   → Check sensor connections");
        break;
      case -3:
        Serial.println("   → Echo too long");
        Serial.println("   → Check for interference or sensor malfunction");
        break;
      default:
        Serial.println("   → Unknown error");
        break;
    }
    
    currentData.isConnected = false;
  }
  
  Serial.println("---");
  delay(3000); // 3 second delay
}

void checkConfigButton() {
  Serial.println("🔘 Checking for configuration button press...");
  Serial.println("📝 Hold BOOT button (GPIO0) for 3 seconds to start config portal");
  Serial.println("📝 Or hold for 10 seconds to clear all settings");
  
  unsigned long startTime = millis();
  bool buttonPressed = false;
  
  // Wait for button press or timeout
  while (millis() - startTime < 5000) { // 5 second window
    if (digitalRead(CONFIG_BUTTON_PIN) == LOW) {
      buttonPressed = true;
      break;
    }
    delay(100);
  }
  
  if (buttonPressed) {
    Serial.println("🔘 Button pressed! Checking duration...");
    startTime = millis();
    
    // Wait for button release or timeout
    while (digitalRead(CONFIG_BUTTON_PIN) == LOW && (millis() - startTime) < 15000) {
      delay(100);
    }
    
    unsigned long pressDuration = millis() - startTime;
    Serial.print("🔘 Button held for: ");
    Serial.print(pressDuration);
    Serial.println(" ms");
    
    if (pressDuration >= 10000) {
      Serial.println("🗑️  Clearing all settings...");
      clearAllSettings();
      forceConfigPortal = true;
    } else if (pressDuration >= 3000) {
      Serial.println("⚙️  Starting configuration portal...");
      forceConfigPortal = true;
    } else {
      Serial.println("⏱️  Button press too short, continuing normally");
    }
  } else {
    Serial.println("⏭️  No button press detected, continuing normally");
  }
}

void clearAllSettings() {
  preferences.clear();
  Serial.println("🗑️  All settings cleared");
  Serial.println("🔄 Restarting ESP32...");
  delay(2000);
  ESP.restart();
}

void setupWiFi() {
  // Load custom parameters
  loadCustomParameters();
  
  // Add custom parameters to WiFiManager
  wifiManager.addParameter(&custom_ha_url);
  wifiManager.addParameter(&custom_ha_token);
  wifiManager.addParameter(&custom_ha_entity);
  wifiManager.addParameter(&custom_device_name);
  wifiManager.addParameter(&custom_device_location);
  
  // Configure WiFiManager settings
  wifiManager.setConfigPortalTimeout(180); // 3 minutes timeout
  wifiManager.setConnectTimeout(30); // 30 seconds connection timeout
  wifiManager.setConnectRetries(3);
  
  // Set custom AP name and password
  wifiManager.setAPCallback([](WiFiManager *myWiFiManager) {
    Serial.println("📱 Configuration Portal Started");
    Serial.print("SSID: ");
    Serial.println(myWiFiManager->getConfigPortalSSID());
    Serial.print("IP Address: ");
    Serial.println(WiFi.softAPIP());
    Serial.println("🌐 Connect to the WiFi network above and go to 192.168.4.1");
    Serial.println("🔧 Configure your WiFi and Home Assistant settings");
  });
  
  // Set save parameters callback
  wifiManager.setSaveParamsCallback([]() {
    Serial.println("💾 Parameters saved");
    saveCustomParameters();
  });
  
  // Set custom menu items
  wifiManager.setCustomMenuHTML("<p><a href='/reset'>Reset Settings</a></p>");
  
  Serial.println("🔐 Starting secure WiFi configuration...");
  
  // Force config portal if button was pressed
  if (forceConfigPortal) {
    Serial.println("🔧 Forcing configuration portal...");
    wifiManager.startConfigPortal("WaterLevelSensor_AP", "waterlevel123");
    return;
  }
  
  // Try to connect to saved WiFi
  if (!wifiManager.autoConnect("WaterLevelSensor_AP", "waterlevel123")) {
    Serial.println("❌ Failed to connect and hit timeout");
    Serial.println("🔄 Restarting ESP32...");
    delay(3000);
    ESP.restart();
  }
  
  // If we get here, we're connected
  Serial.println("✅ Connected to WiFi successfully!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  Serial.print("Signal Strength (RSSI): ");
  Serial.print(WiFi.RSSI());
  Serial.println(" dBm");
  
  wifiConnected = true;
  
  // Save connection info
  preferences.putString("last_ssid", WiFi.SSID());
  preferences.putString("last_ip", WiFi.localIP().toString());
}

void saveCustomParameters() {
  preferences.putString("ha_url", custom_ha_url.getValue());
  preferences.putString("ha_token", custom_ha_token.getValue());
  preferences.putString("ha_entity", custom_ha_entity.getValue());
  preferences.putString("device_name", custom_device_name.getValue());
  preferences.putString("device_location", custom_device_location.getValue());
  Serial.println("💾 Home Assistant parameters saved securely");
}

void loadCustomParameters() {
  String savedUrl = preferences.getString("ha_url", homeAssistantUrl);
  String savedToken = preferences.getString("ha_token", "");
  String savedEntity = preferences.getString("ha_entity", sensorEntityId);
  String savedDeviceName = preferences.getString("device_name", deviceName);
  String savedDeviceLocation = preferences.getString("device_location", deviceLocation);
  
  if (savedUrl.length() > 0) {
    custom_ha_url.setValue(savedUrl.c_str(), 100);
    Serial.println("📋 Loaded saved Home Assistant URL");
  } else {
    Serial.println("⚠️  No Home Assistant URL found - configure in portal");
  }
  
  if (savedToken.length() > 0) {
    custom_ha_token.setValue(savedToken.c_str(), 200);
    Serial.println("📋 Loaded saved Home Assistant token");
  } else {
    Serial.println("⚠️  No Home Assistant token found - configure in portal");
  }
  
  if (savedEntity.length() > 0) {
    custom_ha_entity.setValue(savedEntity.c_str(), 50);
    Serial.println("📋 Loaded saved entity ID");
  }
  custom_device_name.setValue(savedDeviceName.c_str(), 50);
  custom_device_location.setValue(savedDeviceLocation.c_str(), 20);
}

bool connectToWiFi() {
  if (WiFi.status() == WL_CONNECTED) {
    return true;
  }
  
  Serial.println("🔄 Attempting to reconnect to WiFi...");
  
  // Try to reconnect using saved credentials
  WiFi.reconnect();
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println();
    Serial.println("✅ WiFi reconnected successfully!");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
    wifiConnected = true;
    return true;
  } else {
    Serial.println();
    Serial.println("❌ WiFi reconnection failed!");
    Serial.println("🔄 Starting configuration portal...");
    startConfigPortal();
    return false;
  }
}

void startConfigPortal() {
  Serial.println("📱 Starting WiFi Configuration Portal...");
  Serial.println("🌐 Connect to 'WaterLevelSensor_AP' WiFi network");
  Serial.println("🔑 Password: waterlevel123");
  Serial.println("🌍 Then go to 192.168.4.1 in your browser");
  
  wifiManager.startConfigPortal("WaterLevelSensor_AP", "waterlevel123");
}

void setupHTTP() {
  Serial.println("🌐 HTTP communication ready");
  
  // Get Home Assistant URL from preferences
  String haUrl = preferences.getString("ha_url", homeAssistantUrl);
  Serial.print("Home Assistant URL: ");
  Serial.println(haUrl);
  
  // Check if we have a token
  String token = preferences.getString("ha_token", "");
  if (token.length() == 0) {
    Serial.println("⚠️  No Home Assistant token configured");
    Serial.println("🔧 Please configure via WiFi portal");
  } else {
    Serial.println("✅ Home Assistant token configured");
  }
}

bool sendToHomeAssistant() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("❌ WiFi not connected, skipping HTTP request");
    return false;
  }
  
  // Get token and device info from preferences
  String haUrl = preferences.getString("ha_url", homeAssistantUrl);
  String token = preferences.getString("ha_token", "");
  String entityId = preferences.getString("ha_entity", sensorEntityId);
  String deviceName = preferences.getString("device_name", deviceName);
  String deviceLocation = preferences.getString("device_location", deviceLocation);
  
  if (token.length() == 0) {
    Serial.println("❌ No Home Assistant token configured");
    return false;
  }
  
  HTTPClient http;
  
  // Create the API URL
  String url = haUrl + "/api/states/" + entityId;
  
  Serial.print("📤 Sending to: ");
  Serial.println(url);
  
  http.begin(url);
  http.addHeader("Content-Type", "application/json");
  http.addHeader("Authorization", "Bearer " + token);
  
  // Create JSON payload
  StaticJsonDocument<512> doc;
  doc["state"] = currentData.percentage;
  
  JsonObject attributes = doc.createNestedObject("attributes");
  attributes["unit_of_measurement"] = "%";
  attributes["device_class"] = "water";
  attributes["friendly_name"] = deviceName;
  attributes["device_location"] = deviceLocation;
  attributes["distance_cm"] = currentData.distance;
  attributes["water_level_cm"] = currentData.waterLevel;
  attributes["volume_liters"] = currentData.volume;
  attributes["consumption_15min_l_15min"] = consumption.consumption15min;
  attributes["consumption_30min_l_30min"] = consumption.consumption30min;
  attributes["hourly_consumption_l_hour"] = consumption.hourlyConsumption;
  attributes["daily_consumption_l_day"] = consumption.dailyConsumption;
  attributes["average_daily_consumption_l_day"] = consumption.averageDailyConsumption;
  attributes["signal_strength_dbm"] = currentData.signalStrength;
  attributes["is_connected"] = currentData.isConnected;
  attributes["is_calibrated"] = calibration.isCalibrated;
  attributes["tank_height_cm"] = calibration.tankHeight;
  attributes["tank_diameter_cm"] = calibration.tankDiameter;
  attributes["last_update"] = currentData.timestamp;
  
  String jsonString;
  serializeJson(doc, jsonString);
  
  Serial.print("📦 Payload: ");
  Serial.println(jsonString);
  
  // Send POST request
  int httpResponseCode = http.POST(jsonString);
  
  if (httpResponseCode > 0) {
    String response = http.getString();
    Serial.print("✅ HTTP Response code: ");
    Serial.println(httpResponseCode);
    Serial.print("📨 Response: ");
    Serial.println(response);
    http.end();
    return true;
  } else {
    Serial.print("❌ HTTP Error code: ");
    Serial.println(httpResponseCode);
    Serial.print("❌ Error: ");
    Serial.println(http.errorToString(httpResponseCode));
    http.end();
    return false;
  }
}

int measureDistance() {
  // Clear trigger pin
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  
  // Send trigger pulse
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  
  // Measure echo duration with longer timeout for debugging
  long duration = pulseIn(ECHO_PIN, HIGH, 30000); // 30ms timeout for debugging
  
  Serial.print("🔍 Raw duration: ");
  Serial.print(duration);
  Serial.println(" microseconds");
  
  if (duration == 0) {
    Serial.println("❌ No echo received - sensor timeout");
    return -1; // Timeout
  }
  
  if (duration < 100) {
    Serial.println("⚠️  Echo too short - possible sensor issue");
    return -2; // Too short
  }
  
  if (duration > 25000) {
    Serial.println("⚠️  Echo too long - possible sensor issue");
    return -3; // Too long
  }
  
  // Calculate distance (speed of sound = 0.034 cm/microsecond)
  int distance = duration * 0.034 / 2;
  
  Serial.print("📏 Calculated distance: ");
  Serial.print(distance);
  Serial.println(" cm");
  
  return distance;
}

// Add a test function for ultrasonic sensor
void testUltrasonicSensor() {
  Serial.println("🧪 Testing ultrasonic sensor...");
  Serial.println("📋 Pin configuration:");
  Serial.print("  TRIG_PIN: GPIO");
  Serial.println(TRIG_PIN);
  Serial.print("  ECHO_PIN: GPIO");
  Serial.println(ECHO_PIN);
  
  // Test multiple readings
  for (int i = 0; i < 5; i++) {
    Serial.print("Test #");
    Serial.print(i + 1);
    Serial.println(":");
    
    int distance = measureDistance();
    
    if (distance > 0) {
      Serial.print("✅ Valid reading: ");
      Serial.print(distance);
      Serial.println(" cm");
    } else {
      Serial.print("❌ Invalid reading: ");
      Serial.println(distance);
    }
    
    delay(1000);
  }
  
  Serial.println("🧪 Sensor test complete");
}

void loadCalibration() {
  calibration.tankHeight = preferences.getFloat("tank_height", 100.0);
  calibration.emptyDistance = preferences.getFloat("empty_dist", 5.0);
  calibration.fullDistance = preferences.getFloat("full_dist", 95.0);
  calibration.tankDiameter = preferences.getFloat("tank_diameter", 50.0);
  calibration.isCalibrated = preferences.getBool("is_calibrated", false);
  
  Serial.println("📏 Calibration Data:");
  Serial.print("  Tank Height: ");
  Serial.print(calibration.tankHeight);
  Serial.println(" cm");
  Serial.print("  Empty Distance: ");
  Serial.print(calibration.emptyDistance);
  Serial.println(" cm");
  Serial.print("  Full Distance: ");
  Serial.print(calibration.fullDistance);
  Serial.println(" cm");
  Serial.print("  Tank Diameter: ");
  Serial.print(calibration.tankDiameter);
  Serial.println(" cm");
  Serial.print("  Calibrated: ");
  Serial.println(calibration.isCalibrated ? "Yes" : "No");
}

void loadAlertThresholds() {
  alerts.lowLevelThreshold = preferences.getFloat("low_level_threshold", 20.0);
  alerts.rapidLeakThreshold = preferences.getFloat("rapid_leak_threshold", 5.0);
  alerts.moderateLeakThreshold = preferences.getFloat("moderate_leak_threshold", 8.0);
  alerts.highConsumptionThreshold = preferences.getFloat("high_consumption_threshold", 50.0);
  alerts.leakDetectionThreshold = preferences.getFloat("leak_detection_threshold", 10.0);
  alerts.alertCooldownMinutes = preferences.getInt("alert_cooldown_min", 30);
  
  Serial.println("🚨 Alert Thresholds:");
  Serial.print("  Low Level: ");
  Serial.print(alerts.lowLevelThreshold);
  Serial.println("%");
  Serial.print("  Rapid Leak (15min): ");
  Serial.print(alerts.rapidLeakThreshold);
  Serial.println(" L/15min");
  Serial.print("  Moderate Leak (30min): ");
  Serial.print(alerts.moderateLeakThreshold);
  Serial.println(" L/30min");
  Serial.print("  High Consumption: ");
  Serial.print(alerts.highConsumptionThreshold);
  Serial.println(" L/hour");
  Serial.print("  Leak Detection: ");
  Serial.print(alerts.leakDetectionThreshold);
  Serial.println(" L/hour");
  Serial.print("  Alert Cooldown: ");
  Serial.print(alerts.alertCooldownMinutes);
  Serial.println(" minutes");
}

void monitorWaterConsumption() {
  unsigned long currentTime = millis();
  
  // Initialize if first reading
  if (consumption.lastUpdateTime == 0) {
    consumption.previousVolume = currentData.volume;
    consumption.lastUpdateTime = currentTime;
    consumption.last15minTime = currentTime;
    consumption.last30minTime = currentTime;
    consumption.lastHourTime = currentTime;
    consumption.lastDayTime = currentTime;
    return;
  }
  
  // Calculate volume change
  float volumeChange = consumption.previousVolume - currentData.volume;
  if (volumeChange < 0) volumeChange = 0; // No negative consumption (filling)
  
  // Update 15-minute consumption (every 15 minutes)
  if (currentTime - consumption.last15minTime >= 900000) { // 15 minutes
    consumption.consumption15min = volumeChange;
    consumption.last15minTime = currentTime;
    
    Serial.print("📊 15-Min Consumption: ");
    Serial.print(consumption.consumption15min);
    Serial.println(" L/15min");
  }
  
  // Update 30-minute consumption (every 30 minutes)
  if (currentTime - consumption.last30minTime >= 1800000) { // 30 minutes
    consumption.consumption30min = volumeChange;
    consumption.last30minTime = currentTime;
    
    Serial.print("📊 30-Min Consumption: ");
    Serial.print(consumption.consumption30min);
    Serial.println(" L/30min");
  }
  
  // Update hourly consumption (every hour)
  if (currentTime - consumption.lastHourTime >= 3600000) { // 1 hour
    consumption.hourlyConsumption = volumeChange;
    consumption.lastHourTime = currentTime;
    
    Serial.print("📊 Hourly Consumption: ");
    Serial.print(consumption.hourlyConsumption);
    Serial.println(" L/hour");
  }
  
  // Update daily consumption (every 24 hours)
  if (currentTime - consumption.lastDayTime >= 86400000) { // 24 hours
    consumption.dailyConsumption = volumeChange;
    consumption.lastDayTime = currentTime;
    
    // Calculate average daily consumption
    consumption.averageDailyConsumption = (consumption.averageDailyConsumption + consumption.dailyConsumption) / 2.0;
    
    Serial.print("📊 Daily Consumption: ");
    Serial.print(consumption.dailyConsumption);
    Serial.println(" L/day");
  }
  
  consumption.previousVolume = currentData.volume;
  consumption.lastUpdateTime = currentTime;
}

void checkAlerts() {
  static unsigned long lastAlertTime = 0;
  unsigned long currentTime = millis();
  
  // Check cooldown period
  if (currentTime - lastAlertTime < (alerts.alertCooldownMinutes * 60000)) {
    return;
  }
  
  // Low level alert
  if (currentData.percentage <= alerts.lowLevelThreshold && !consumption.lowLevelAlert) {
    String message = "Water level is low: " + String(currentData.percentage) + "%";
    sendAlert("low_level", message);
    consumption.lowLevelAlert = true;
    lastAlertTime = currentTime;
  } else if (currentData.percentage > alerts.lowLevelThreshold) {
    consumption.lowLevelAlert = false;
  }
  
  // Rapid leak detection (15 minutes)
  if (consumption.consumption15min > alerts.rapidLeakThreshold && !consumption.rapidLeakAlert) {
    String message = "🚨 RAPID LEAK DETECTED: " + String(consumption.consumption15min) + " L/15min";
    sendAlert("rapid_leak", message);
    consumption.rapidLeakAlert = true;
    lastAlertTime = currentTime;
  } else if (consumption.consumption15min <= alerts.rapidLeakThreshold) {
    consumption.rapidLeakAlert = false;
  }
  
  // Moderate leak detection (30 minutes)
  if (consumption.consumption30min > alerts.moderateLeakThreshold && !consumption.moderateLeakAlert) {
    String message = "⚠️ MODERATE LEAK DETECTED: " + String(consumption.consumption30min) + " L/30min";
    sendAlert("moderate_leak", message);
    consumption.moderateLeakAlert = true;
    lastAlertTime = currentTime;
  } else if (consumption.consumption30min <= alerts.moderateLeakThreshold) {
    consumption.moderateLeakAlert = false;
  }
  
  // High consumption alert (1 hour)
  if (consumption.hourlyConsumption > alerts.highConsumptionThreshold && !consumption.highConsumptionAlert) {
    String message = "High water consumption detected: " + String(consumption.hourlyConsumption) + " L/hour";
    sendAlert("high_consumption", message);
    consumption.highConsumptionAlert = true;
    lastAlertTime = currentTime;
  } else if (consumption.hourlyConsumption <= alerts.highConsumptionThreshold) {
    consumption.highConsumptionAlert = false;
  }
  
  // Leak detection (continuous low consumption over time)
  if (consumption.hourlyConsumption > alerts.leakDetectionThreshold && !consumption.leakDetected) {
    String message = "Possible leak detected: " + String(consumption.hourlyConsumption) + " L/hour";
    sendAlert("leak_detected", message);
    consumption.leakDetected = true;
    lastAlertTime = currentTime;
  } else if (consumption.hourlyConsumption <= alerts.leakDetectionThreshold) {
    consumption.leakDetected = false;
  }
}

void sendAlert(String alertType, String message) {
  Serial.print("🚨 ALERT (");
  Serial.print(alertType);
  Serial.print("): ");
  Serial.println(message);
  
  // Send alert to Home Assistant
  String haUrl = preferences.getString("ha_url", homeAssistantUrl);
  String token = preferences.getString("ha_token", "");
  String deviceLocation = preferences.getString("device_location", deviceLocation);
  String deviceName = preferences.getString("device_name", deviceName);
  if (token.length() == 0) return;
  
  HTTPClient http;
  String url = haUrl + "/api/states/sensor.water_alert_" + deviceLocation;
  
  http.begin(url);
  http.addHeader("Content-Type", "application/json");
  http.addHeader("Authorization", "Bearer " + token);
  
  StaticJsonDocument<512> doc;
  doc["state"] = alertType;
  
  JsonObject attributes = doc.createNestedObject("attributes");
  attributes["friendly_name"] = deviceName + " - Water Alert";
  attributes["alert_message"] = message;
  attributes["alert_type"] = alertType;
  attributes["water_level"] = currentData.percentage;
  attributes["current_volume"] = currentData.volume;
  attributes["hourly_consumption"] = consumption.hourlyConsumption;
  attributes["timestamp"] = millis();
  
  String jsonString;
  serializeJson(doc, jsonString);
  http.POST(jsonString);
  http.end();
}

void createAlertEntities() {
  String haUrl = preferences.getString("ha_url", homeAssistantUrl);
  String token = preferences.getString("ha_token", "");
  String deviceLocation = preferences.getString("device_location", deviceLocation);
  String deviceName = preferences.getString("device_name", deviceName);
  if (token.length() == 0) return;
  
  HTTPClient http;
  
  // Create alert threshold inputs
  String url = haUrl + "/api/states/input_number.low_level_threshold_" + deviceLocation;
  http.begin(url);
  http.addHeader("Content-Type", "application/json");
  http.addHeader("Authorization", "Bearer " + token);
  
  StaticJsonDocument<512> doc;
  doc["state"] = alerts.lowLevelThreshold;
  
  JsonObject attributes = doc.createNestedObject("attributes");
  attributes["unit_of_measurement"] = "%";
  attributes["friendly_name"] = deviceName + " - Low Level Threshold";
  attributes["min"] = 5;
  attributes["max"] = 50;
  attributes["step"] = 1;
  attributes["mode"] = "slider";
  
  String jsonString;
  serializeJson(doc, jsonString);
  http.POST(jsonString);
  http.end();
  
  // Rapid leak threshold (15 minutes)
  url = haUrl + "/api/states/input_number.rapid_leak_threshold_" + deviceLocation;
  http.begin(url);
  http.addHeader("Content-Type", "application/json");
  http.addHeader("Authorization", "Bearer " + token);
  
  doc.clear();
  doc["state"] = alerts.rapidLeakThreshold;
  
  attributes = doc.createNestedObject("attributes");
  attributes["unit_of_measurement"] = "L/15min";
  attributes["friendly_name"] = deviceName + " - Rapid Leak Threshold (15min)";
  attributes["min"] = 1;
  attributes["max"] = 20;
  attributes["step"] = 0.5;
  attributes["mode"] = "slider";
  
  jsonString = "";
  serializeJson(doc, jsonString);
  http.POST(jsonString);
  http.end();
  
  // Moderate leak threshold (30 minutes)
  url = haUrl + "/api/states/input_number.moderate_leak_threshold_" + deviceLocation;
  http.begin(url);
  http.addHeader("Content-Type", "application/json");
  http.addHeader("Authorization", "Bearer " + token);
  
  doc.clear();
  doc["state"] = alerts.moderateLeakThreshold;
  
  attributes = doc.createNestedObject("attributes");
  attributes["unit_of_measurement"] = "L/30min";
  attributes["friendly_name"] = deviceName + " - Moderate Leak Threshold (30min)";
  attributes["min"] = 2;
  attributes["max"] = 30;
  attributes["step"] = 1;
  attributes["mode"] = "slider";
  
  jsonString = "";
  serializeJson(doc, jsonString);
  http.POST(jsonString);
  http.end();
  
  // High consumption threshold
  url = haUrl + "/api/states/input_number.high_consumption_threshold_" + deviceLocation;
  http.begin(url);
  http.addHeader("Content-Type", "application/json");
  http.addHeader("Authorization", "Bearer " + token);
  
  doc.clear();
  doc["state"] = alerts.highConsumptionThreshold;
  
  attributes = doc.createNestedObject("attributes");
  attributes["unit_of_measurement"] = "L/hour";
  attributes["friendly_name"] = deviceName + " - High Consumption Threshold";
  attributes["min"] = 10;
  attributes["max"] = 200;
  attributes["step"] = 5;
  attributes["mode"] = "slider";
  
  jsonString = "";
  serializeJson(doc, jsonString);
  http.POST(jsonString);
  http.end();
  
  // Leak detection threshold
  url = haUrl + "/api/states/input_number.leak_detection_threshold_" + deviceLocation;
  http.begin(url);
  http.addHeader("Content-Type", "application/json");
  http.addHeader("Authorization", "Bearer " + token);
  
  doc.clear();
  doc["state"] = alerts.leakDetectionThreshold;
  
  attributes = doc.createNestedObject("attributes");
  attributes["unit_of_measurement"] = "L/hour";
  attributes["friendly_name"] = deviceName + " - Leak Detection Threshold";
  attributes["min"] = 1;
  attributes["max"] = 50;
  attributes["step"] = 1;
  attributes["mode"] = "slider";
  
  jsonString = "";
  serializeJson(doc, jsonString);
  http.POST(jsonString);
  http.end();
  
  // Alert cooldown
  url = haUrl + "/api/states/input_number.alert_cooldown_" + deviceLocation;
  http.begin(url);
  http.addHeader("Content-Type", "application/json");
  http.addHeader("Authorization", "Bearer " + token);
  
  doc.clear();
  doc["state"] = alerts.alertCooldownMinutes;
  
  attributes = doc.createNestedObject("attributes");
  attributes["unit_of_measurement"] = "minutes";
  attributes["friendly_name"] = deviceName + " - Alert Cooldown";
  attributes["min"] = 5;
  attributes["max"] = 120;
  attributes["step"] = 5;
  attributes["mode"] = "slider";
  
  jsonString = "";
  serializeJson(doc, jsonString);
  http.POST(jsonString);
  http.end();
  
  Serial.println("🚨 Alert entities created in Home Assistant");
}

void calculateWaterLevel(int distance) {
  currentData.distance = distance;
  currentData.signalStrength = WiFi.RSSI();
  currentData.isConnected = true;
  currentData.timestamp = millis();
  
  if (calibration.isCalibrated) {
    // Use calibration data for accurate calculation
    float distanceRange = calibration.fullDistance - calibration.emptyDistance;
    float currentRange = distance - calibration.emptyDistance;
    
    if (currentRange < 0) currentRange = 0;
    if (currentRange > distanceRange) currentRange = distanceRange;
    
    float percentage = (currentRange / distanceRange) * 100.0;
    currentData.percentage = round(percentage);
    currentData.waterLevel = round((percentage / 100.0) * calibration.tankHeight);
    
    // Calculate volume (assuming cylindrical tank)
    float radius = calibration.tankDiameter / 2.0;
    float volumeCm3 = 3.14159 * radius * radius * currentData.waterLevel;
    currentData.volume = volumeCm3 / 1000.0; // Convert to liters
    
  } else {
    // Fallback to simple calculation
    int waterHeight = 100 - distance;
    if (waterHeight < 0) waterHeight = 0;
    if (waterHeight > 100) waterHeight = 100;
    
    currentData.percentage = waterHeight;
    currentData.waterLevel = waterHeight;
    currentData.volume = 0.0; // Unknown without calibration
  }
}

void createCalibrationEntities() {
  // Create input helpers for calibration in Home Assistant
  String haUrl = preferences.getString("ha_url", homeAssistantUrl);
  String token = preferences.getString("ha_token", "");
  String deviceLocation = preferences.getString("device_location", deviceLocation);
  String deviceName = preferences.getString("device_name", deviceName);
  if (token.length() == 0) return;
  
  HTTPClient http;
  
  // Create tank height input
  String url = haUrl + "/api/states/input_number.tank_height_" + deviceLocation;
  http.begin(url);
  http.addHeader("Content-Type", "application/json");
  http.addHeader("Authorization", "Bearer " + token);
  
  StaticJsonDocument<512> doc;
  doc["state"] = calibration.tankHeight;
  
  JsonObject attributes = doc.createNestedObject("attributes");
  attributes["unit_of_measurement"] = "cm";
  attributes["friendly_name"] = deviceName + " - Tank Height";
  attributes["min"] = 10;
  attributes["max"] = 500;
  attributes["step"] = 1;
  attributes["mode"] = "slider";
  
  String jsonString;
  serializeJson(doc, jsonString);
  http.POST(jsonString);
  http.end();
  
  // Create empty distance input
  url = haUrl + "/api/states/input_number.empty_distance_" + deviceLocation;
  http.begin(url);
  http.addHeader("Content-Type", "application/json");
  http.addHeader("Authorization", "Bearer " + token);
  
  doc.clear();
  doc["state"] = calibration.emptyDistance;
  
  attributes = doc.createNestedObject("attributes");
  attributes["unit_of_measurement"] = "cm";
  attributes["friendly_name"] = deviceName + " - Empty Distance";
  attributes["min"] = 1;
  attributes["max"] = 50;
  attributes["step"] = 0.5;
  attributes["mode"] = "slider";
  
  jsonString = "";
  serializeJson(doc, jsonString);
  http.POST(jsonString);
  http.end();
  
  // Create full distance input
  url = haUrl + "/api/states/input_number.full_distance_" + deviceLocation;
  http.begin(url);
  http.addHeader("Content-Type", "application/json");
  http.addHeader("Authorization", "Bearer " + token);
  
  doc.clear();
  doc["state"] = calibration.fullDistance;
  
  attributes = doc.createNestedObject("attributes");
  attributes["unit_of_measurement"] = "cm";
  attributes["friendly_name"] = deviceName + " - Full Distance";
  attributes["min"] = 50;
  attributes["max"] = 200;
  attributes["step"] = 0.5;
  attributes["mode"] = "slider";
  
  jsonString = "";
  serializeJson(doc, jsonString);
  http.POST(jsonString);
  http.end();
  
  // Create tank diameter input
  url = haUrl + "/api/states/input_number.tank_diameter_" + deviceLocation;
  http.begin(url);
  http.addHeader("Content-Type", "application/json");
  http.addHeader("Authorization", "Bearer " + token);
  
  doc.clear();
  doc["state"] = calibration.tankDiameter;
  
  attributes = doc.createNestedObject("attributes");
  attributes["unit_of_measurement"] = "cm";
  attributes["friendly_name"] = deviceName + " - Tank Diameter";
  attributes["min"] = 10;
  attributes["max"] = 200;
  attributes["step"] = 1;
  attributes["mode"] = "slider";
  
  jsonString = "";
  serializeJson(doc, jsonString);
  http.POST(jsonString);
  http.end();
  
  // Create calibration button
  url = haUrl + "/api/states/input_button.calibrate_sensor_" + deviceLocation;
  http.begin(url);
  http.addHeader("Content-Type", "application/json");
  http.addHeader("Authorization", "Bearer " + token);
  
  doc.clear();
  doc["state"] = "idle";
  
  attributes = doc.createNestedObject("attributes");
  attributes["friendly_name"] = deviceName + " - Calibrate Sensor";
  
  jsonString = "";
  serializeJson(doc, jsonString);
  http.POST(jsonString);
  http.end();
  
  Serial.println("📋 Calibration entities created in Home Assistant");
} 