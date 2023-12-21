# add_library(pico_servo servo.h servo.c)

# target_link_libraries(pico_servo pico_stdlib hardware_pwm hardware_clocks)

# target_include_directories(pico_servo PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")


add_library(pico_sr04 distance_sensor.h distance_sensor.cpp)

pico_generate_pio_header(pico_sr04 ${CMAKE_CURRENT_LIST_DIR}/sensor.pio)

target_link_libraries(pico_sr04 
	pico_stdlib 
	hardware_pwm 
	hardware_pio
	hardware_irq
)

target_include_directories(pico_sr04 PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")





