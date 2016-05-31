start_sensors()
{
    if [ -c /dev/sensors ]; then
        mkdir /persist/sensors
        chmod -h 775 /persist/sensors

        if [ ! -a /persist/sensors/sensors_settings ]; then
            # If the settings file not exist, enable sensors HAL
            echo 1 > /persist/sensors/sensors_settings
        fi

        chmod -h 664 /persist/sensors/sensors_settings
        chown -h system.root /persist/sensors/sensors_settings

        start sensors
    fi
}

start_sensors
