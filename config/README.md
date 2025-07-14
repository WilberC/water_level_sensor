# Water Level Sensor Configuration

This directory contains the complete Home Assistant configuration for the water level sensor system.

## 🚨 Recent Fixes (2025-07-13)

### Issues Resolved:
1. **Automation Validation Error**: Fixed nested list structure in automation configuration
2. **Template Errors**: Added safe attribute access with fallback values
3. **Missing Attributes**: Templates now handle cases where sensor attributes don't exist yet

### Changes Made:
- Fixed `configuration_example.yaml` to properly include automations
- Updated `automations.yaml` to include water level sensor automations
- Added `default()` filters to all template sensors for safe attribute access
- Updated automation notifications to handle missing attributes gracefully

## 📁 Configuration Files

### Main Configuration
- `configuration_example.yaml` - Main Home Assistant configuration file
- `automations.yaml` - Main automations file (includes water level automations)
- `scripts.yaml` - Home Assistant scripts
- `scenes.yaml` - Home Assistant scenes

### Water Level Sensor Specific
- `automations/water_level_sensor_automations.yaml` - Water level alert automations
- `templates/water_level_sensor_templates.yaml` - Template sensors for calculations
- `lovelace/water_level_dashboard.yaml` - Dashboard configuration

## 🔧 Setup Instructions

### 1. Copy Configuration Files
Copy the contents of `configuration_example.yaml` to your main `configuration.yaml` file, or include it directly:

```yaml
# In your main configuration.yaml
!include config/configuration_example.yaml
```

### 2. Verify Sensor Entities
Ensure your water level sensors are properly configured and sending data. The system expects:
- `sensor.water_level_3f` - 3rd floor water level sensor
- `sensor.water_level_1f` - 1st floor water level sensor
- `sensor.water_alert_tank_3f` - 3rd floor alert sensor
- `sensor.water_alert_tank_1f` - 1st floor alert sensor

### 3. Check Sensor Attributes
The Arduino sensors should send these attributes:
- `volume_liters` - Current volume in liters
- `consumption_15min_l_15min` - 15-minute consumption rate
- `consumption_30min_l_30min` - 30-minute consumption rate
- `hourly_consumption_l_hour` - Hourly consumption rate
- `daily_consumption_l_day` - Daily consumption
- `average_daily_consumption_l_day` - Average daily consumption

### 4. Restart Home Assistant
After copying the configuration files, restart Home Assistant to load the new configuration.

## 🛠️ Template Sensors

The system creates these calculated sensors:

### 3rd Floor Sensors
- `sensor.3rd_floor_days_until_empty` - Days until tank is empty
- `sensor.3rd_floor_water_usage_trend` - Usage trend (High/Normal/Low)
- `sensor.3rd_floor_leak_severity` - Leak severity (Critical/Moderate/None)

### 1st Floor Sensors
- `sensor.1st_floor_days_until_empty` - Days until tank is empty
- `sensor.1st_floor_water_usage_trend` - Usage trend (High/Normal/Low)
- `sensor.1st_floor_leak_severity` - Leak severity (Critical/Moderate/None)

## 🚨 Alert System

### Alert Types
- **Low Water Level**: When tank level drops below threshold
- **Rapid Leak (15min)**: Critical leak detection
- **Moderate Leak (30min)**: Moderate leak detection
- **Daily Report**: Daily consumption summary

### Alert Channels
- Persistent notifications in Home Assistant
- Mobile app notifications (if configured)

## 📊 Dashboard

The system includes a comprehensive dashboard with:
- Real-time water level graphs
- Consumption monitoring
- Alert status
- Calibration controls
- Quick actions

Access the dashboard via the sidebar or navigate to `/lovelace/water-level`.

## 🔍 Troubleshooting

### Common Issues

1. **Template Errors**: If you see template errors, ensure the water level sensors are properly configured and sending data.

2. **Missing Attributes**: The template sensors now handle missing attributes gracefully, but you may see "0" or "Normal" values until the sensors are properly calibrated.

3. **Automation Not Working**: Check that the alert sensors (`sensor.water_alert_tank_3f` and `sensor.water_alert_tank_1f`) are being created by the Arduino sensors.

### Debug Steps
1. Check Home Assistant logs for specific error messages
2. Verify sensor entities exist in Developer Tools > States
3. Test template sensors in Developer Tools > Template
4. Ensure Arduino sensors are connected and sending data

## 📝 Configuration Notes

- All template sensors use safe attribute access with `default()` filters
- Automations include proper error handling for missing attributes
- The system is designed to work even if some attributes are missing
- Dashboard will show placeholder values until sensors are fully configured

## 🔄 Updates

This configuration is designed to work with the ESP32 water level sensor firmware. If you update the Arduino code, ensure the entity IDs and attributes match what's expected by this configuration. 