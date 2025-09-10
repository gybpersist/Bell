idf_component_register(
    SRCS 
        "main.c"
        "Com/Com_Debug.c"
        "Int/Int_ES8311.c"
        "Int/Int_WS2812.c"
        "Dri/Dri_WIFI.c"
        "Dri/Dri_MQTT.c"
        "Dri/Dri_OV2640"
        "App/App_WS.c"

    INCLUDE_DIRS 
        "."
        "App"
        "Int"
        "Dri"
        "Com"
)
