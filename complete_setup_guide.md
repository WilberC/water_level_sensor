# Complete Water Level Sensor Setup Guide

## Overview
This ESP32-based water level sensor system is designed to be completely configurable through a web portal - no code changes required! The system includes:

- **Smart WiFi Configuration** with secure credential storage
- **Home Assistant Integration** via HTTP REST API
- **Multi-tank Support** with unique device identification
- **Advanced Calibration** with persistent storage
- **Smart Alerting** with multiple consumption monitoring intervals
- **Leak Detection** with configurable thresholds

## Hardware Requirements
- ESP32 development board
- HC-SR04 ultrasonic sensor
- Jumper wires
- Power supply (USB or 5V)

## Pin Connections
- **TRIG_PIN**: GPIO 13
- **ECHO_PIN**: GPIO 12
- **CONFIG_BUTTON**: GPIO 0 (BOOT button)

## Initial Setup

### 1. Upload the Code
1. Open the project in PlatformIO or Arduino IDE
2. Connect your ESP32 via USB
3. Upload the code to your ESP32

### 2. First Boot Configuration
On first boot, the ESP32 will:
1. Create a WiFi access point named `WaterLevelSensor_AP`
2. Password: `waterlevel123`
3. IP address: `192.168.4.1`

### 3. Access Configuration Portal
1. Connect to the `WaterLevelSensor_AP` WiFi network
2. Open a web browser and go to `192.168.4.1`
3. You'll see the configuration form with all settings

## Configuration Parameters

### WiFi Settings
- **WiFi SSID**: Your home WiFi network name
- **WiFi Password**: Your home WiFi password

### Home Assistant Settings
- **Home Assistant URL**: Your Home Assistant instance URL
  - Example: `https://home.nexoralabs.com`
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

## Multiple Sensor Setup

### For 3rd Floor Tank
```
Home Assistant URL: https://home.nexoralabs.com
Access Token: [your-long-lived-token]
Entity ID: sensor.water_level_3f
Device Name: 3rd Floor Tank
Device Location: tank_3f
```

### For 1st Floor Tank
```
Home Assistant URL: https://home.nexoralabs.com
Access Token: [your-long-lived-token]
Entity ID: sensor.water_level_1f
Device Name: 1st Floor Tank
Device Location: tank_1f
```

## Button Controls

### Configuration Portal
- **Hold BOOT button for 3 seconds** → Start configuration portal
- **Hold BOOT button for 10 seconds** → Clear all settings and restart

### When to Use
- **3-second press**: When you need to change WiFi or Home Assistant settings
- **10-second press**: When you want to completely reset the device (factory reset)

## Calibration Setup

### 1. Physical Setup
1. Mount the ultrasonic sensor at the top of your water tank
2. Ensure the sensor faces downward toward the water surface
3. Measure your tank dimensions:
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

## Alert Configuration

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

## Home Assistant Integration

### Automatic Entity Creation
The sensor automatically creates these entities:

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

### Dashboard Example
```yaml
# Example dashboard configuration
views:
  - title: Water Level Monitoring
    cards:
      - type: vertical-stack
        cards:
          - type: gauge
            entity: sensor.water_level_tank_3f
            name: 3rd Floor Tank
            min: 0
            max: 100
            severity:
              green: 0
              yellow: 20
              red: 10
          
          - type: gauge
            entity: sensor.water_level_tank_1f
            name: 1st Floor Tank
            min: 0
            max: 100
            severity:
              green: 0
              yellow: 20
              red: 10
```

## Troubleshooting

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

## Advanced Features

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

## Security Features

### Secure Credential Storage
- WiFi passwords stored in ESP32's secure preferences
- Home Assistant tokens stored securely
- No hardcoded credentials in the code

### Network Security
- HTTPS support for Home Assistant communication
- Long-lived access tokens for authentication
- No default passwords or open endpoints

## Maintenance

### Regular Checks
1. **Monitor sensor readings** for accuracy
2. **Check alert functionality** periodically
3. **Verify WiFi connectivity** after network changes
4. **Update calibration** if tank dimensions change

### Firmware Updates
1. **Backup configuration** before updates
2. **Upload new firmware** via PlatformIO/Arduino IDE
3. **Reconfigure if needed** using the configuration portal

## Support

### Common Issues
- **Sensor not reading**: Check connections and power
- **WiFi disconnects**: Check signal strength and router settings
- **Home Assistant not receiving data**: Check URL, token, and network connectivity
- **Inaccurate readings**: Recalibrate the sensor

### Getting Help
1. Check the troubleshooting section above
2. Verify all configuration parameters
3. Test with the configuration portal
4. Use the button controls for reset if needed

---

**Note**: This system is designed to be completely self-contained and configurable. No code changes are required for different installations - everything can be set up through the web configuration portal. 