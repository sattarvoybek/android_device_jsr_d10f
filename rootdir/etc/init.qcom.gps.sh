#!/system/bin/sh

baseband=`getprop ro.baseband`
izat_premium_enablement=`getprop ro.qc.sdk.izat.premium_enabled`
izat_service_mask=`getprop ro.qc.sdk.izat.service_mask`

let "izat_service_gtp_wifi=$izat_service_mask & 2#1"
let "izat_service_gtp_wwan_lite=($izat_service_mask & 2#10)>>1"
let "izat_service_pip=($izat_service_mask & 2#100)>>2"

if [ "$izat_premium_enablement" -ne 1 ]; then
    if [ "$izat_service_gtp_wifi" -ne 0 ]; then
# GTP WIFI bit shall be masked by the premium service flag
        let "izat_service_gtp_wifi=0"
    fi
fi

if [ "$izat_service_gtp_wwan_lite" -ne 0 ] ||
   [ "$izat_service_gtp_wifi" -ne 0 ] ||
   [ "$izat_service_pip" -ne 0 ]; then
# OS Agent would also be started under the same condition
    start location_mq
fi

if [ "$izat_service_gtp_wwan_lite" -ne 0 ] ||
   [ "$izat_service_gtp_wifi" -ne 0 ]; then
# start GTP services shared by WiFi and WWAN Lite
    start xtwifi_inet
    start xtwifi_client
fi

if [ "$izat_service_gtp_wifi" -ne 0 ] ||
   [ "$izat_service_pip" -ne 0 ]; then
# advanced WiFi scan service shared by WiFi and PIP
    start lowi-server
fi

if [ "$izat_service_pip" -ne 0 ]; then
# PIP services
    start quipc_main
    start quipc_igsn
fi
