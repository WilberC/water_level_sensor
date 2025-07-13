# Water Level Sensor Home Assistant Configuration

This folder contains the organized configuration files for the Water Level Sensor system.

## File Structure

```
config/
├── automations/
│   └── water_level_sensor_automations.yaml    # All automations for alerts and notifications
├── templates/
│   └── water_level_sensor_templates.yaml      # Template sensors for calculations
├── lovelace/
│   └── water_level_dashboard.yaml             # Dashboard views for monitoring
├── configuration_integration.yaml             # How to include files in main config
└── README.md                                  # This file
```

## Installation Instructions

### Step 1: Copy Files to Home Assistant
Copy the entire `config` folder to your Home Assistant configuration directory:
```
homeassistant/
├── configuration.yaml
├── automations/
│   └── water_level_sensor_automations.yaml
├── templates/
│   └── water_level_sensor_templates.yaml
└── lovelace/
    └── water_level_dashboard.yaml
```

### Step 2: Update Main Configuration
Add these lines to your `configuration.yaml`:

```yaml
# Include water level sensor automations
automation: !include automations/water_level_sensor_automations.yaml

# Include water level sensor template sensors
template: !include templates/water_level_sensor_templates.yaml
```

### Step 3: Add Dashboard (Choose One Option)

#### Option A: Include in Main Configuration
Add this to your `configuration.yaml`:

```yaml
lovelace:
  mode: yaml
  dashboards:
    water-level:
      mode: yaml
      title: Water Level Monitoring
      icon: mdi:water
      show_in_sidebar: true
      source: !include lovelace/water_level_dashboard.yaml
```

#### Option B: Import Manually in Lovelace
1. Go to your Lovelace dashboard
2. Click the three dots menu → "Manage Dashboards"
3. Click the "+" to add a new dashboard
4. Choose "YAML" mode
5. Copy the contents of `lovelace/water_level_dashboard.yaml`

### Step 4: Restart Home Assistant
After making changes, restart Home Assistant to load the new configuration.

## Required Custom Components

Make sure you have these custom components installed:

1. **Mini Graph Card**: For consumption graphs
   - HACS: `custom:mini-graph-card`
   - Or manual installation

2. **Grid Layout**: For dashboard layout
   - HACS: `custom:grid-layout`
   - Or manual installation

## Entity Names

The configuration expects these entities (created automatically by ESP32):

### 3rd Floor Tank:
- `sensor.water_level_3f`
- `sensor.water_alert_tank_3f`
- `input_number.tank_height_tank_3f`
- `input_number.tank_diameter_tank_3f`
- `input_number.empty_distance_tank_3f`
- `input_number.full_distance_tank_3f`
- `input_button.calibrate_sensor_tank_3f`
- `input_number.low_level_threshold_tank_3f`
- `input_number.rapid_leak_threshold_tank_3f`
- `input_number.moderate_leak_threshold_tank_3f`
- `input_number.high_consumption_threshold_tank_3f`
- `input_number.leak_detection_threshold_tank_3f`
- `input_number.alert_cooldown_tank_3f`

### 1st Floor Tank:
- `sensor.water_level_1f`
- `sensor.water_alert_tank_1f`
- `input_number.tank_height_tank_1f`
- `input_number.tank_diameter_tank_1f`
- `input_number.empty_distance_tank_1f`
- `input_number.full_distance_tank_1f`
- `input_button.calibrate_sensor_tank_1f`
- `input_number.low_level_threshold_tank_1f`
- `input_number.rapid_leak_threshold_tank_1f`
- `input_number.moderate_leak_threshold_tank_1f`
- `input_number.high_consumption_threshold_tank_1f`
- `input_number.leak_detection_threshold_tank_1f`
- `input_number.alert_cooldown_tank_1f`

## Features

### Automations
- Low water level alerts
- Rapid leak detection (15-minute intervals)
- Moderate leak detection (30-minute intervals)
- Daily consumption reports
- Calibration notifications

### Template Sensors
- Days until empty calculation
- Water usage trend analysis
- Leak severity classification

### Dashboard Views
- **Water Level Monitoring**: Real-time graphs and gauges
- **Tank Calibration**: Setup and calibration interface
- **Alert Monitoring**: Threshold configuration and alert status

## Troubleshooting

1. **Configuration Errors**: Check YAML syntax in each file
2. **Missing Entities**: Ensure ESP32 is running and connected
3. **Dashboard Not Showing**: Verify custom components are installed
4. **Automations Not Working**: Check entity names match your setup

## Support

If you encounter issues:
1. Check Home Assistant logs for errors
2. Verify all entity names match your ESP32 configuration
3. Ensure custom components are properly installed
4. Restart Home Assistant after configuration changes 