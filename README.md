# ESP32 Smart Water Level Sensor

A complete ESP32-based water level monitoring system with Home Assistant integration, advanced leak detection, and multi-tank support. This system is designed to be completely configurable through a web portal - no code changes required!

## 🌟 Features

- **Smart WiFi Configuration** with secure credential storage
- **Home Assistant Integration** via HTTP REST API
- **Multi-tank Support** with unique device identification
- **Advanced Calibration** with persistent storage
- **Smart Alerting** with multiple consumption monitoring intervals
- **Leak Detection** with configurable thresholds
- **Volume Calculation** based on tank dimensions
- **Real-time Monitoring** with consumption tracking
- **Mobile Notifications** for critical alerts

## 📋 Table of Contents

- [Hardware Requirements](#hardware-requirements)
- [Pin Connections](#pin-connections)
- [Quick Start Guide](#quick-start-guide)
- [Configuration](#configuration)
- [Multiple Sensor Setup](#multiple-sensor-setup)
- [Calibration](#calibration)
- [Alert System](#alert-system)
- [Home Assistant Integration](#home-assistant-integration)
- [Home Assistant Configuration](#home-assistant-configuration)
- [Dashboard Configuration](#dashboard-configuration)
- [Troubleshooting](#troubleshooting)
- [Advanced Features](#advanced-features)
- [Security](#security)
- [Maintenance](#maintenance)

## 🔧 Hardware Requirements

- **ESP32 development board** (any ESP32 variant)
- **AJ-SR04M ultrasonic sensor**
- **Jumper wires**
- **Power supply** (USB or 5V)
- **Mounting hardware** for the sensor

## 🔌 Pin Connections

| Component | ESP32 Pin | Description |
|-----------|-----------|-------------|
| AJ-SR04M TRIG | GPIO 13 | Trigger signal |
| AJ-SR04M ECHO | GPIO 12 | Echo signal |
| BOOT Button | GPIO 0 | Configuration button |

## 🚀 Quick Start Guide

### 1. Upload the Code

1. **Install PlatformIO** or use Arduino IDE
2. **Clone/download** this project
3. **Connect your ESP32** via USB
4. **Upload the code** to your ESP32

### 2. First Boot Configuration

On first boot, the ESP32 will:
1. Create a WiFi access point named `WaterLevelSensor_AP`
2. Password: `waterlevel123`
3. IP address: `192.168.4.1`

### 3. Access Configuration Portal

1. **Connect** to the `WaterLevelSensor_AP` WiFi network
2. **Open a web browser** and go to `192.168.4.1`
3. **Configure** all settings through the web interface

## ⚙️ Configuration

### WiFi Settings
- **WiFi SSID**: Your home WiFi network name
- **WiFi Password**: Your home WiFi password

### Home Assistant Settings
- **Home Assistant URL**: Your Home Assistant instance URL
  - Example: `https://domain.com`
  - Example: `http://192.168.1.100:8123` (local network)
  - Example: `https://your-ha-instance.duckdns.org`

- **Home Assistant Access Token**: Long-lived access token
  - Go to Home Assistant → Profile → Long-Lived Access Tokens
  - Create a new token with a descriptive name
  - Copy the token and paste it here

- **Sensor Entity ID**: Unique identifier for this sensor
  - Example: `sensor.water_level_3f` (3rd floor)
  - Example: `sensor.water_level_1f` (1st floor)
  - Must be unique across all your sensors

### Device Configuration
- **Device Name**: Human-readable name for this sensor
  - Example: `3rd Floor Tank`
  - Example: `1st Floor Tank`
  - Example: `Garden Water Tank`

- **Device Location**: Short identifier for this location
  - Example: `tank_3f`
  - Example: `tank_1f`
  - Example: `garden_tank`
  - Used in entity names and alerts

## 🏗️ Multiple Sensor Setup

This project is configured for **2 sensors**: 3rd floor and 1st floor tanks.

### For 3rd Floor Tank
```
Home Assistant URL: https://domain.com
Access Token: [your-long-lived-token]
Entity ID: sensor.water_level_3f
Device Name: 3rd Floor Tank
Device Location: tank_3f
```

### For 1st Floor Tank
```
Home Assistant URL: https://domain.com
Access Token: [your-long-lived-token]
Entity ID: sensor.water_level_1f
Device Name: 1st Floor Tank
Device Location: tank_1f
```

### Home Assistant Entities Created

#### 3rd Floor Tank Entities:
- `sensor.water_level_3f` - Main water level sensor
- `sensor.water_alert_tank_3f` - Alert status
- `input_number.tank_height_tank_3f` - Tank height
- `input_number.tank_diameter_tank_3f` - Tank diameter
- `input_number.empty_distance_tank_3f` - Empty distance
- `input_number.full_distance_tank_3f` - Full distance
- `input_button.calibrate_sensor_tank_3f` - Calibration button
- `input_number.low_level_threshold_tank_3f` - Low level alert
- `input_number.rapid_leak_threshold_tank_3f` - Rapid leak (15min)
- `input_number.moderate_leak_threshold_tank_3f` - Moderate leak (30min)
- `input_number.high_consumption_threshold_tank_3f` - High consumption
- `input_number.leak_detection_threshold_tank_3f` - Leak detection
- `input_number.alert_cooldown_tank_3f` - Alert cooldown

#### 1st Floor Tank Entities:
- `sensor.water_level_1f` - Main water level sensor
- `sensor.water_alert_tank_1f` - Alert status
- `input_number.tank_height_tank_1f` - Tank height
- `input_number.tank_diameter_tank_1f` - Tank diameter
- `input_number.empty_distance_tank_1f` - Empty distance
- `input_number.full_distance_tank_1f` - Full distance
- `input_button.calibrate_sensor_tank_1f` - Calibration button
- `input_number.low_level_threshold_tank_1f` - Low level alert
- `input_number.rapid_leak_threshold_tank_1f` - Rapid leak (15min)
- `input_number.moderate_leak_threshold_tank_1f` - Moderate leak (30min)
- `input_number.high_consumption_threshold_tank_1f` - High consumption
- `input_number.leak_detection_threshold_tank_1f` - Leak detection
- `input_number.alert_cooldown_tank_1f` - Alert cooldown

## 🎯 Button Controls

### Configuration Portal
- **Hold BOOT button for 3 seconds** → Start configuration portal
- **Hold BOOT button for 10 seconds** → Clear all settings and restart

### When to Use
- **3-second press**: When you need to change WiFi or Home Assistant settings
- **10-second press**: When you want to completely reset the device (factory reset)

## 📏 Calibration Setup

### 1. Physical Setup
1. **Mount the ultrasonic sensor** at the top of your water tank
2. **Ensure the sensor faces downward** toward the water surface
3. **Measure your tank dimensions**:
   - **Tank Height**: Total height from bottom to top (cm)
   - **Tank Diameter**: Width of the tank (cm)

### 2. Calibration Process
1. **Empty Tank Calibration**:
   - Empty the tank completely
   - Note the distance reading from the sensor
   - This is your "Empty Distance"

2. **Full Tank Calibration**:
   - Fill the tank to maximum capacity
   - Note the distance reading from the sensor
   - This is your "Full Distance"

### 3. Home Assistant Calibration
The sensor automatically creates calibration entities in Home Assistant:

- **Tank Height**: Set to your tank's total height in cm
- **Empty Distance**: Distance when tank is empty (cm)
- **Full Distance**: Distance when tank is full (cm)
- **Tank Diameter**: Tank diameter for volume calculation (cm)
- **Calibrate Sensor**: Button to save calibration data

## 🚨 Alert System

### Alert Thresholds
The sensor creates configurable alert thresholds in Home Assistant:

- **Low Level Threshold**: Alert when water level drops below this percentage
- **Rapid Leak Threshold (15min)**: Alert for fast water loss in 15 minutes
- **Moderate Leak Threshold (30min)**: Alert for moderate water loss in 30 minutes
- **High Consumption Threshold**: Alert for high hourly consumption
- **Leak Detection Threshold**: Alert for continuous low-level leaks
- **Alert Cooldown**: Minimum time between alerts (minutes)

### Recommended Settings
```
Low Level: 20% (alert when tank is 20% full)
Rapid Leak (15min): 5 L/15min (emergency leak)
Moderate Leak (30min): 8 L/30min (moderate leak)
High Consumption: 50 L/hour (unusual usage)
Leak Detection: 10 L/hour (slow leak)
Alert Cooldown: 30 minutes
```

### Alert Priority Levels
- 🚨 **CRITICAL**: Rapid leak (15min) - Immediate action required
- ⚠️ **MODERATE**: Moderate leak (30min) - Check soon
- ⚠️ **HIGH**: High consumption (1hour) - Monitor usage
- 🚨 **URGENT**: Low water level - Refill needed

## 🏠 Home Assistant Integration

### Automatic Entity Creation
Each sensor automatically creates these entities:

#### Main Sensor
- **Entity**: `sensor.water_level_[location]`
- **Attributes**: Water level percentage, distance, volume, consumption data

#### Calibration Entities
- `input_number.tank_height_[location]`
- `input_number.empty_distance_[location]`
- `input_number.full_distance_[location]`
- `input_number.tank_diameter_[location]`
- `input_button.calibrate_sensor_[location]`

#### Alert Entities
- `input_number.low_level_threshold_[location]`
- `input_number.rapid_leak_threshold_[location]`
- `input_number.moderate_leak_threshold_[location]`
- `input_number.high_consumption_threshold_[location]`
- `input_number.leak_detection_threshold_[location]`
- `input_number.alert_cooldown_[location]`
- `sensor.water_alert_[location]`

## 🏠 Home Assistant Configuration

### How to Use the YAML Configuration File

The `home_assistant_configuration.yaml` file contains ready-to-use Home Assistant configurations for both sensors. Here's how to use it:

#### Option 1: Include the Entire File
Add this line to your main `configuration.yaml`:
```yaml
# In configuration.yaml
!include home_assistant_configuration.yaml
```

#### Option 2: Copy Specific Sections
Copy the sections you need from the YAML file into your `configuration.yaml`:
- **Dashboard views** → `configuration.yaml` or `lovelace.yaml`
- **Automations** → `automations.yaml`
- **Template sensors** → `configuration.yaml`

#### Option 3: Use as Template
Modify the YAML file for your specific needs and include it.

### File Structure
```
water_level_sensor/
├── README.md                           # Complete documentation and guide
├── home_assistant_configuration.yaml   # Ready-to-use HA config for 2 sensors
├── platformio.ini                      # PlatformIO configuration
└── src/
    └── water_level.ino                 # Main Arduino sketch
```

## 📊 Dashboard Configuration

### Main Monitoring Dashboard (2 Sensors)
```yaml
views:
  - title: Water Level Monitoring
    path: water-level
    type: custom:grid-layout
    badges: []
    cards:
      - type: vertical-stack
        cards:
          - type: custom:mini-graph-card
            name: 3rd Floor Tank
            entity: sensor.water_level_3f
            line_color: "#2196F3"
            fill_color: "#2196F3"
            points_per_hour: 20
            hours_to_show: 24
            min: 0
            max: 100
            
          - type: custom:mini-graph-card
            name: 1st Floor Tank
            entity: sensor.water_level_1f
            line_color: "#FF9800"
            fill_color: "#FF9800"
            points_per_hour: 20
            hours_to_show: 24
            min: 0
            max: 100
            
          - type: gauge
            name: 3rd Floor Level
            entity: sensor.water_level_3f
            min: 0
            max: 100
            severity:
              green: 0
              yellow: 20
              red: 10
            
          - type: gauge
            name: 1st Floor Level
            entity: sensor.water_level_1f
            min: 0
            max: 100
            severity:
              green: 0
              yellow: 20
              red: 10
            
          - type: entities
            title: Current Status
            entities:
              - entity: sensor.water_level_3f
                name: 3rd Floor Tank
              - entity: sensor.water_level_1f
                name: 1st Floor Tank
              - type: attribute
                entity: sensor.water_level_3f
                attribute: volume_liters
                name: 3F Volume (L)
              - type: attribute
                entity: sensor.water_level_1f
                attribute: volume_liters
                name: 1F Volume (L)
              - type: attribute
                entity: sensor.water_level_3f
                attribute: consumption_15min_l_15min
                name: 3F 15-Min Consumption (L/15min)
              - type: attribute
                entity: sensor.water_level_1f
                attribute: consumption_15min_l_15min
                name: 1F 15-Min Consumption (L/15min)
              - type: attribute
                entity: sensor.water_level_3f
                attribute: hourly_consumption_l_hour
                name: 3F Hourly Consumption (L/h)
              - type: attribute
                entity: sensor.water_level_1f
                attribute: hourly_consumption_l_hour
                name: 1F Hourly Consumption (L/h)
```

### Calibration Dashboard (2 Sensors)
```yaml
  - title: Tank Calibration
    path: calibration
    type: custom:grid-layout
    badges: []
    cards:
      - type: vertical-stack
        cards:
          - type: entities
            title: 3rd Floor Tank Calibration
            entities:
              - entity: input_number.tank_height_tank_3f
                name: Tank Height
              - entity: input_number.tank_diameter_tank_3f
                name: Tank Diameter
              - entity: input_number.empty_distance_tank_3f
                name: Empty Distance
              - entity: input_number.full_distance_tank_3f
                name: Full Distance
              - entity: input_button.calibrate_sensor_tank_3f
                name: Save Calibration
              
          - type: entities
            title: 1st Floor Tank Calibration
            entities:
              - entity: input_number.tank_height_tank_1f
                name: Tank Height
              - entity: input_number.tank_diameter_tank_1f
                name: Tank Diameter
              - entity: input_number.empty_distance_tank_1f
                name: Empty Distance
              - entity: input_number.full_distance_tank_1f
                name: Full Distance
              - entity: input_button.calibrate_sensor_tank_1f
                name: Save Calibration
```



### Alert Monitoring Dashboard (2 Sensors)
```yaml
  - title: Alert Monitoring
    path: alerts
    type: custom:grid-layout
    badges: []
    cards:
      - type: vertical-stack
        cards:
          - type: entities
            title: 3rd Floor Alert Thresholds
            entities:
              - entity: input_number.low_level_threshold_tank_3f
                name: Low Level Threshold (%)
              - entity: input_number.rapid_leak_threshold_tank_3f
                name: Rapid Leak Threshold (L/15min)
              - entity: input_number.moderate_leak_threshold_tank_3f
                name: Moderate Leak Threshold (L/30min)
              - entity: input_number.high_consumption_threshold_tank_3f
                name: High Consumption Threshold (L/h)
              - entity: input_number.leak_detection_threshold_tank_3f
                name: Leak Detection Threshold (L/h)
              - entity: input_number.alert_cooldown_tank_3f
                name: Alert Cooldown (minutes)
              
          - type: entities
            title: 1st Floor Alert Thresholds
            entities:
              - entity: input_number.low_level_threshold_tank_1f
                name: Low Level Threshold (%)
              - entity: input_number.rapid_leak_threshold_tank_1f
                name: Rapid Leak Threshold (L/15min)
              - entity: input_number.moderate_leak_threshold_tank_1f
                name: Moderate Leak Threshold (L/30min)
              - entity: input_number.high_consumption_threshold_tank_1f
                name: High Consumption Threshold (L/h)
              - entity: input_number.leak_detection_threshold_tank_1f
                name: Leak Detection Threshold (L/h)
              - entity: input_number.alert_cooldown_tank_1f
                name: Alert Cooldown (minutes)
              
          - type: entities
            title: Current Alerts
            entities:
              - entity: sensor.water_alert_tank_3f
                name: 3rd Floor Alert Status
              - entity: sensor.water_alert_tank_1f
                name: 1st Floor Alert Status
              - type: attribute
                entity: sensor.water_alert_tank_3f
                attribute: alert_message
                name: 3F Alert Message
              - type: attribute
                entity: sensor.water_alert_tank_1f
                attribute: alert_message
                name: 1F Alert Message
              
          - type: custom:mini-graph-card
            name: Multi-Interval Consumption (3rd Floor)
            entities:
              - entity: sensor.water_level_3f
                attribute: consumption_15min_l_15min
                name: 15-Min Consumption
              - entity: sensor.water_level_3f
                attribute: consumption_30min_l_30min
                name: 30-Min Consumption
              - entity: sensor.water_level_3f
                attribute: hourly_consumption_l_hour
                name: Hourly Consumption
            line_color: "#2196F3"
            fill_color: "#2196F3"
            points_per_hour: 4
            hours_to_show: 6
            min: 0
            
          - type: custom:mini-graph-card
            name: Multi-Interval Consumption (1st Floor)
            entities:
              - entity: sensor.water_level_1f
                attribute: consumption_15min_l_15min
                name: 15-Min Consumption
              - entity: sensor.water_level_1f
                attribute: consumption_30min_l_30min
                name: 30-Min Consumption
              - entity: sensor.water_level_1f
                attribute: hourly_consumption_l_hour
                name: Hourly Consumption
            line_color: "#FF9800"
            fill_color: "#FF9800"
            points_per_hour: 4
            hours_to_show: 6
            min: 0
```

## 🔧 Automations

### Alert Automations (2 Sensors)
```yaml
automation:
  # 3rd Floor Tank Automations
  - alias: "Save 3rd Floor Water Level Calibration"
    description: "Save calibration data when 3rd floor calibrate button is pressed"
    trigger:
      platform: state
      entity_id: input_button.calibrate_sensor_tank_3f
      to: "pressed"
    action:
      - service: persistent_notification.create
        data:
          title: "3rd Floor Calibration Saved"
          message: "3rd floor water level sensor calibration has been saved. The sensor will restart to apply new settings."
      - delay: "00:00:02"
      - service: input_button.press
        target:
          entity_id: input_button.calibrate_sensor_tank_3f

  - alias: "3rd Floor Low Water Level Alert"
    description: "Send notification when 3rd floor water level is low"
    trigger:
      platform: state
      entity_id: sensor.water_alert_tank_3f
      to: "low_level"
    action:
      - service: persistent_notification.create
        data:
          title: "🚨 3rd Floor Low Water Level Alert"
          message: "{{ states('sensor.water_alert_tank_3f') }} - 3rd floor water level is critically low!"
      - service: notify.mobile_app
        data:
          title: "🚨 3rd Floor Low Water Level"
          message: "3rd floor water level is at {{ states('sensor.water_level_3f') }}% - Please check your tank!"

  - alias: "3rd Floor Rapid Leak Alert (15min)"
    description: "Send urgent notification for 3rd floor rapid leak detection"
    trigger:
      platform: state
      entity_id: sensor.water_alert_tank_3f
      to: "rapid_leak"
    action:
      - service: persistent_notification.create
        data:
          title: "🚨 3rd Floor RAPID LEAK DETECTED!"
          message: "{{ states('sensor.water_alert_tank_3f') }} - Critical leak detected in 3rd floor tank in 15 minutes!"
      - service: notify.mobile_app
        data:
          title: "🚨 3rd Floor RAPID LEAK DETECTED!"
          message: "3rd floor critical leak: {{ states('sensor.water_level_3f') | attr('consumption_15min_l_15min') }} L/15min - CHECK IMMEDIATELY!"

  - alias: "3rd Floor Moderate Leak Alert (30min)"
    description: "Send notification for 3rd floor moderate leak detection"
    trigger:
      platform: state
      entity_id: sensor.water_alert_tank_3f
      to: "moderate_leak"
    action:
      - service: persistent_notification.create
        data:
          title: "⚠️ 3rd Floor MODERATE LEAK DETECTED"
          message: "{{ states('sensor.water_alert_tank_3f') }} - Moderate leak detected in 3rd floor tank in 30 minutes!"
      - service: notify.mobile_app
        data:
          title: "⚠️ 3rd Floor MODERATE LEAK DETECTED"
          message: "3rd floor moderate leak: {{ states('sensor.water_level_3f') | attr('consumption_30min_l_30min') }} L/30min - Check soon!"

  # 1st Floor Tank Automations
  - alias: "Save 1st Floor Water Level Calibration"
    description: "Save calibration data when 1st floor calibrate button is pressed"
    trigger:
      platform: state
      entity_id: input_button.calibrate_sensor_tank_1f
      to: "pressed"
    action:
      - service: persistent_notification.create
        data:
          title: "1st Floor Calibration Saved"
          message: "1st floor water level sensor calibration has been saved. The sensor will restart to apply new settings."
      - delay: "00:00:02"
      - service: input_button.press
        target:
          entity_id: input_button.calibrate_sensor_tank_1f

  - alias: "1st Floor Low Water Level Alert"
    description: "Send notification when 1st floor water level is low"
    trigger:
      platform: state
      entity_id: sensor.water_alert_tank_1f
      to: "low_level"
    action:
      - service: persistent_notification.create
        data:
          title: "🚨 1st Floor Low Water Level Alert"
          message: "{{ states('sensor.water_alert_tank_1f') }} - 1st floor water level is critically low!"
      - service: notify.mobile_app
        data:
          title: "🚨 1st Floor Low Water Level"
          message: "1st floor water level is at {{ states('sensor.water_level_1f') }}% - Please check your tank!"

  - alias: "1st Floor Rapid Leak Alert (15min)"
    description: "Send urgent notification for 1st floor rapid leak detection"
    trigger:
      platform: state
      entity_id: sensor.water_alert_tank_1f
      to: "rapid_leak"
    action:
      - service: persistent_notification.create
        data:
          title: "🚨 1st Floor RAPID LEAK DETECTED!"
          message: "{{ states('sensor.water_alert_tank_1f') }} - Critical leak detected in 1st floor tank in 15 minutes!"
      - service: notify.mobile_app
        data:
          title: "🚨 1st Floor RAPID LEAK DETECTED!"
          message: "1st floor critical leak: {{ states('sensor.water_level_1f') | attr('consumption_15min_l_15min') }} L/15min - CHECK IMMEDIATELY!"

  - alias: "1st Floor Moderate Leak Alert (30min)"
    description: "Send notification for 1st floor moderate leak detection"
    trigger:
      platform: state
      entity_id: sensor.water_alert_tank_1f
      to: "moderate_leak"
    action:
      - service: persistent_notification.create
        data:
          title: "⚠️ 1st Floor MODERATE LEAK DETECTED"
          message: "{{ states('sensor.water_alert_tank_1f') }} - Moderate leak detected in 1st floor tank in 30 minutes!"
      - service: notify.mobile_app
        data:
          title: "⚠️ 1st Floor MODERATE LEAK DETECTED"
          message: "1st floor moderate leak: {{ states('sensor.water_level_1f') | attr('consumption_30min_l_30min') }} L/30min - Check soon!"

  # Combined Daily Report
  - alias: "Daily Water Consumption Report (Both Tanks)"
    description: "Send daily water consumption summary for both tanks"
    trigger:
      platform: time
      at: "08:00:00"
    condition:
      - condition: or
        conditions:
          - condition: numeric_state
            entity_id: sensor.water_level_3f
            attribute: daily_consumption_l_day
            above: 0
          - condition: numeric_state
            entity_id: sensor.water_level_1f
            attribute: daily_consumption_l_day
            above: 0
    action:
      - service: notify.mobile_app
        data:
          title: "📊 Daily Water Report"
          message: |
            3rd Floor:
            - Yesterday's consumption: {{ states('sensor.water_level_3f') | attr('daily_consumption_l_day') | round(1) }} L
            - Current level: {{ states('sensor.water_level_3f') }}%
            - Average daily: {{ states('sensor.water_level_3f') | attr('average_daily_consumption_l_day') | round(1) }} L
            
            1st Floor:
            - Yesterday's consumption: {{ states('sensor.water_level_1f') | attr('daily_consumption_l_day') | round(1) }} L
            - Current level: {{ states('sensor.water_level_1f') }}%
            - Average daily: {{ states('sensor.water_level_1f') | attr('average_daily_consumption_l_day') | round(1) }} L
```

### Multiple Sensor Automations
```yaml
automation:
  - alias: "3rd Floor Low Water Level Alert"
    description: "Send notification when 3rd floor water level is low"
    trigger:
      platform: state
      entity_id: sensor.water_alert_tank_3f
      to: "low_level"
    action:
      - service: notify.mobile_app
        data:
          title: "🚨 3rd Floor Low Water Level"
          message: "3rd floor tank is at {{ states('sensor.water_level_3f') }}% - Please check!"

  - alias: "1st Floor Low Water Level Alert"
    description: "Send notification when 1st floor water level is low"
    trigger:
      platform: state
      entity_id: sensor.water_alert_tank_1f
      to: "low_level"
    action:
      - service: notify.mobile_app
        data:
          title: "🚨 1st Floor Low Water Level"
          message: "1st floor tank is at {{ states('sensor.water_level_1f') }}% - Please check!"

  - alias: "3rd Floor Rapid Leak Alert"
    description: "Send urgent notification for 3rd floor rapid leak"
    trigger:
      platform: state
      entity_id: sensor.water_alert_tank_3f
      to: "rapid_leak"
    action:
      - service: notify.mobile_app
        data:
          title: "🚨 3rd Floor RAPID LEAK!"
          message: "3rd floor critical leak: {{ states('sensor.water_level_3f') | attr('consumption_15min_l_15min') }} L/15min"

  - alias: "1st Floor Rapid Leak Alert"
    description: "Send urgent notification for 1st floor rapid leak"
    trigger:
      platform: state
      entity_id: sensor.water_alert_tank_1f
      to: "rapid_leak"
    action:
      - service: notify.mobile_app
        data:
          title: "🚨 1st Floor RAPID LEAK!"
          message: "1st floor critical leak: {{ states('sensor.water_level_1f') | attr('consumption_15min_l_15min') }} L/15min"
```

## 🔧 Troubleshooting

### WiFi Connection Issues
1. **Check WiFi credentials** in the configuration portal
2. **Verify signal strength** - move sensor closer to router if needed
3. **Check router settings** - ensure 2.4GHz WiFi is enabled
4. **Use button reset** - hold BOOT button for 10 seconds to clear settings

### Home Assistant Connection Issues
1. **Verify Home Assistant URL** - check for typos and protocol (http/https)
2. **Check access token** - ensure it's a long-lived token with proper permissions
3. **Test network connectivity** - ensure ESP32 can reach your Home Assistant instance
4. **Check firewall settings** - ensure port 8123 (or your HA port) is accessible

### Sensor Reading Issues
1. **Check physical connections** - ensure TRIG and ECHO pins are connected correctly
2. **Verify power supply** - ensure stable 5V power
3. **Check sensor positioning** - ensure sensor faces downward and is mounted securely
4. **Calibrate the sensor** - use the calibration entities in Home Assistant

### Alert Issues
1. **Check alert thresholds** - ensure they're set to reasonable values
2. **Verify alert cooldown** - increase if getting too many alerts
3. **Check consumption calculations** - ensure tank dimensions are set correctly

### Common Issues
- **Sensor not reading**: Check connections and power
- **WiFi disconnects**: Check signal strength and router settings
- **Home Assistant not receiving data**: Check URL, token, and network connectivity
- **Inaccurate readings**: Recalibrate the sensor

## 🚀 Advanced Features

### Consumption Monitoring
The sensor tracks water consumption at multiple intervals:
- **15-minute consumption**: For rapid leak detection
- **30-minute consumption**: For moderate leak detection
- **Hourly consumption**: For usage patterns
- **Daily consumption**: For long-term monitoring

### Leak Detection
The system uses multiple algorithms:
- **Rapid leak**: High consumption in 15 minutes
- **Moderate leak**: Sustained consumption in 30 minutes
- **Slow leak**: Continuous low-level consumption over time

### Data Persistence
All settings are stored in ESP32's non-volatile memory:
- WiFi credentials
- Home Assistant configuration
- Calibration data
- Alert thresholds

### Template Sensors
```yaml
template:
  - sensor:
      - name: "Days Until Empty"
        unique_id: "water_days_until_empty"
        state: >
          {% set current_level = states('sensor.water_level_sensor') | float %}
          {% set daily_usage = states('sensor.water_level_sensor') | attr('average_daily_consumption_l_day') | float %}
          {% if daily_usage > 0 and current_level > 0 %}
            {% set current_volume = states('sensor.water_level_sensor') | attr('volume_liters') | float %}
            {{ (current_volume / daily_usage) | round(1) }}
          {% else %}
            0
          {% endif %}
        unit_of_measurement: "days"
        device_class: "duration"
        
      - name: "Water Usage Trend"
        unique_id: "water_usage_trend"
        state: >
          {% set current_usage = states('sensor.water_level_sensor') | attr('hourly_consumption_l_hour') | float %}
          {% set avg_usage = states('sensor.water_level_sensor') | attr('average_daily_consumption_l_day') | float / 24 %}
          {% if avg_usage > 0 %}
            {% if current_usage > avg_usage * 1.5 %}
              High
            {% elif current_usage < avg_usage * 0.5 %}
              Low
            {% else %}
              Normal
            {% endif %}
          {% else %}
            Normal
          {% endif %}
        device_class: "enum"
        
      - name: "Leak Severity"
        unique_id: "water_leak_severity"
        state: >
          {% set rapid_leak = states('sensor.water_level_sensor') | attr('consumption_15min_l_15min') | float %}
          {% set moderate_leak = states('sensor.water_level_sensor') | attr('consumption_30min_l_30min') | float %}
          {% set rapid_threshold = states('input_number.rapid_leak_threshold') | float %}
          {% set moderate_threshold = states('input_number.moderate_leak_threshold') | float %}
          {% if rapid_leak > rapid_threshold %}
            Critical
          {% elif moderate_leak > moderate_threshold %}
            Moderate
          {% else %}
            None
          {% endif %}
        device_class: "enum"
```

## 🔒 Security Features

### Secure Credential Storage
- WiFi passwords stored in ESP32's secure preferences
- Home Assistant tokens stored securely
- No hardcoded credentials in the code

### Network Security
- HTTPS support for Home Assistant communication
- Long-lived access tokens for authentication
- No default passwords or open endpoints

### Security Benefits of HTTP over MQTT
- No MQTT broker needed
- Uses Home Assistant's built-in authentication
- Access tokens can be revoked easily
- No additional network services exposed
- Simpler firewall configuration
- Uses standard HTTPS/HTTP ports

## 🛠️ Maintenance

### Regular Checks
1. **Monitor sensor readings** for accuracy
2. **Check alert functionality** periodically
3. **Verify WiFi connectivity** after network changes
4. **Update calibration** if tank dimensions change

### Firmware Updates
1. **Backup configuration** before updates
2. **Upload new firmware** via PlatformIO/Arduino IDE
3. **Reconfigure if needed** using the configuration portal

## 📋 Project Structure

```
water_level_sensor/
├── README.md                    # This comprehensive guide
├── platformio.ini              # PlatformIO configuration
└── src/
    └── water_level.ino         # Main Arduino sketch
```

## 🎯 Tips for Success

- **Use descriptive names** (e.g., "3rd Floor Tank", "1st Floor Tank")
- **Use consistent naming** (e.g., `tank_3f`, `tank_1f`)
- **Test each sensor** individually before deploying
- **Document your configuration** for future reference
- **Set different alert thresholds** based on tank usage patterns
- **Monitor consumption patterns** to optimize alert settings

## 🔧 Getting Help

1. Check the troubleshooting section above
2. Verify all configuration parameters
3. Test with the configuration portal
4. Use the button controls for reset if needed

## 📝 Notes

- This system is designed to be completely self-contained and configurable
- No code changes are required for different installations
- Everything can be set up through the web configuration portal
- The system automatically creates all necessary Home Assistant entities
- Multi-interval monitoring provides comprehensive leak detection
- Mobile notifications ensure you never miss critical alerts

---

**Happy monitoring! 🚰📊** 