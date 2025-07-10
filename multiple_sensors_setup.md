# Multiple Water Level Sensors Setup Guide

This guide explains how to set up multiple water level sensors (e.g., 3rd floor and 1st floor tanks) in Home Assistant without conflicts.

## 🏗️ Setup for Multiple Sensors

### **Sensor 1: 3rd Floor Tank**

1. **Modify the code** for the 3rd floor sensor:
```cpp
// Device Configuration - 3rd Floor
const char* deviceName = "3rd Floor Tank";
const char* deviceLocation = "tank_3f";
const char* deviceId = "water_sensor_3f";
```

2. **Upload to ESP32** for 3rd floor

3. **Configure via WiFi portal**:
   - Connect to `"WaterLevelSensor_AP"` WiFi
   - Go to `192.168.4.1`
   - Set WiFi credentials
   - Set Home Assistant access token
   - Set Entity ID: `sensor.water_level_3f`
   - Set Device Name: `3rd Floor Tank`
   - Set Device Location: `tank_3f`

### **Sensor 2: 1st Floor Tank**

1. **Modify the code** for the 1st floor sensor:
```cpp
// Device Configuration - 1st Floor
const char* deviceName = "1st Floor Tank";
const char* deviceLocation = "tank_1f";
const char* deviceId = "water_sensor_1f";
```

2. **Upload to ESP32** for 1st floor

3. **Configure via WiFi portal**:
   - Connect to `"WaterLevelSensor_AP"` WiFi
   - Go to `192.168.4.1`
   - Set WiFi credentials
   - Set Home Assistant access token
   - Set Entity ID: `sensor.water_level_1f`
   - Set Device Name: `1st Floor Tank`
   - Set Device Location: `tank_1f`

## 📱 Home Assistant Entities Created

### **3rd Floor Tank Entities:**
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

### **1st Floor Tank Entities:**
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

## 🎛️ Dashboard Configuration

### **Main Monitoring Dashboard:**
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
```

### **Calibration Dashboard:**
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

## 🚨 Alert Configuration

### **Automations for Multiple Sensors:**
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

## 🔧 Quick Setup Steps

1. **Prepare two ESP32 boards** with the same code
2. **Modify device configuration** for each sensor
3. **Upload code** to each ESP32
4. **Configure each sensor** via WiFi portal
5. **Check Home Assistant** for new entities
6. **Create dashboards** for monitoring both tanks
7. **Set up automations** for alerts

## ✅ Benefits of This Approach

- **No conflicts** between sensors
- **Unique entity IDs** for each tank
- **Separate calibration** for each tank
- **Individual alert thresholds** for each tank
- **Easy identification** in Home Assistant
- **Independent monitoring** and control
- **Scalable** for more tanks if needed

## 🎯 Tips for Success

- **Use descriptive names** (e.g., "3rd Floor Tank", "1st Floor Tank")
- **Use consistent naming** (e.g., `tank_3f`, `tank_1f`)
- **Test each sensor** individually before deploying
- **Document your configuration** for future reference
- **Set different alert thresholds** based on tank usage patterns 