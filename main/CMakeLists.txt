###############################################################################
# main/CMakeLists.txt   —— 语音交互 + BLE 四足机器狗  (ESP-IDF 5.3.x)
###############################################################################

# --------------------------------------------------------------------------- #
# 1. 基础源码
# --------------------------------------------------------------------------- #
set(SOURCES
    "audio_codecs/audio_codec.cc"
    "audio_codecs/no_audio_codec.cc"
    "audio_codecs/box_audio_codec.cc"
    "audio_codecs/es8311_audio_codec.cc"
    "audio_codecs/es8374_audio_codec.cc"
    "audio_codecs/es8388_audio_codec.cc"
    "led/single_led.cc"
    "led/circular_strip.cc"
    "led/gpio_led.cc"
    "display/display.cc"
    "display/lcd_display.cc"
    "display/oled_display.cc"
    "protocols/protocol.cc"
    "protocols/mqtt_protocol.cc"
    "protocols/websocket_protocol.cc"
    "iot/thing.cc"
    "iot/thing_manager.cc"
    "mcp_server.cc"
    "system_info.cc"
    "application.cc"
    "ota.cc"
    "settings.cc"
    "background_task.cc"
    "main.cc"

    # === 四足机器狗新增 ===
    "servo/servo_control.cpp"
    "servo/ble_servo_server.cpp"
)

# --------------------------------------------------------------------------- #
# 2. 头文件搜索路径
# --------------------------------------------------------------------------- #
set(INCLUDE_DIRS
    "."
    "display"
    "audio_codecs"
    "protocols"
    "audio_processing"
    "servo"            # ★ 新增：舵机驱动头文件
)

# --------------------------------------------------------------------------- #
# 3. IOT、板级公共文件（保持原逻辑）
# --------------------------------------------------------------------------- #
file(GLOB IOT_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/iot/things/*.cc)
list(APPEND SOURCES ${IOT_SOURCES})

file(GLOB BOARD_COMMON_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/boards/common/*.cc)
list(APPEND SOURCES ${BOARD_COMMON_SOURCES})
list(APPEND INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/boards/common)

# --------------------------------------------------------------------------- #
# 4. 根据 Kconfig 选择板型（原逻辑完整保留）
# --------------------------------------------------------------------------- #
# …… 这里是你上传文件中的超长 BOARD_TYPE 判断块 ……

# --------------------------------------------------------------------------- #
# 5. 板型专用源码
# --------------------------------------------------------------------------- #
file(GLOB BOARD_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/boards/${BOARD_TYPE}/*.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/boards/${BOARD_TYPE}/*.c
)
list(APPEND SOURCES ${BOARD_SOURCES})

# --------------------------------------------------------------------------- #
# 6. 音频处理 / 唤醒词选配
# --------------------------------------------------------------------------- #
if(CONFIG_USE_AUDIO_PROCESSOR)
    list(APPEND SOURCES "audio_processing/afe_audio_processor.cc")
else()
    list(APPEND SOURCES "audio_processing/dummy_audio_processor.cc")
endif()

if(CONFIG_USE_WAKE_WORD_DETECT)
    list(APPEND SOURCES "audio_processing/wake_word_detect.cc")
endif()

# --------------------------------------------------------------------------- #
# 7. 语言资源（原逻辑完整保留）
# --------------------------------------------------------------------------- #
# 根据 Kconfig 选择语言目录
if(CONFIG_LANGUAGE_ZH_CN)
    set(LANG_DIR "zh-CN")
elseif(CONFIG_LANGUAGE_ZH_TW)
    set(LANG_DIR "zh-TW")
elseif(CONFIG_LANGUAGE_EN_US)
    set(LANG_DIR "en-US")
elseif(CONFIG_LANGUAGE_JA_JP)
    set(LANG_DIR "ja-JP")
elsif(CONFIG_BOARD_TYPE_DEVKIT_ESP32S3_ST7789)
    set(BOARD_TYPE "devkit-esp32s3-st7789")
    set(BOARD_NAME "ESP32-S3-DevKitC-1 ST7789")
endif()

# 定义生成路径
set(LANG_JSON   "${CMAKE_CURRENT_SOURCE_DIR}/assets/${LANG_DIR}/language.json")
set(LANG_HEADER "${CMAKE_CURRENT_SOURCE_DIR}/assets/lang_config.h")
file(GLOB LANG_SOUNDS   ${CMAKE_CURRENT_SOURCE_DIR}/assets/${LANG_DIR}/*.p3)
file(GLOB COMMON_SOUNDS ${CMAKE_CURRENT_SOURCE_DIR}/assets/common/*.p3)

# --------------------------------------------------------------------------- #
# 8. 针对 ESP32 芯片排除不兼容文件
# --------------------------------------------------------------------------- #
if(CONFIG_IDF_TARGET_ESP32)
    list(REMOVE_ITEM SOURCES
        "audio_codecs/box_audio_codec.cc"
        "audio_codecs/es8388_audio_codec.cc"
        "led/gpio_led.cc"
    )
endif()

# --------------------------------------------------------------------------- #
# 9. 正式注册组件
# --------------------------------------------------------------------------- #
idf_component_register(
    SRCS         ${SOURCES}
    EMBED_FILES  ${LANG_SOUNDS} ${COMMON_SOUNDS}
    INCLUDE_DIRS ${INCLUDE_DIRS}
    WHOLE_ARCHIVE
    REQUIRES     driver freertos      # ★ 新增：NimBLE 依赖
    PRIV_REQUIRES                     # ★ 仅本组件需要，外部不用
        esp_app_format
        app_update
        esp-nimble-cpp             # <-- 新增，解决 NimBLEDevice.h
        efuse                      # <-- 新增，解决 esp_efuse.h
)

# --------------------------------------------------------------------------- #
# 10. BOARD_TYPE / BOARD_NAME 宏
# --------------------------------------------------------------------------- #
if(NOT BOARD_NAME)
    set(BOARD_NAME ${BOARD_TYPE})
endif()
target_compile_definitions(${COMPONENT_LIB}
    PRIVATE BOARD_TYPE=\"${BOARD_TYPE}\" BOARD_NAME=\"${BOARD_NAME}\"
)

# --------------------------------------------------------------------------- #
# 11. 语言头文件自动生成
# --------------------------------------------------------------------------- #
add_custom_command(
    OUTPUT  ${LANG_HEADER}
    COMMAND python ${PROJECT_DIR}/scripts/gen_lang.py
            --input  "${LANG_JSON}"
            --output "${LANG_HEADER}"
    DEPENDS
        ${LANG_JSON}
        ${PROJECT_DIR}/scripts/gen_lang.py
    COMMENT "Generating ${LANG_DIR} language config"
)

add_custom_target(lang_header ALL
    DEPENDS ${LANG_HEADER}
)
###############################################################################
