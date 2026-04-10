#include <stdio.h>

#include <rcl/rcl.h>
#include <rcl/error_handling.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>
#include <std_msgs/msg/u_int64.h>
#include <rmw_microros/rmw_microros.h>

#include "pico/stdlib.h"
#include "picow_udp_transports.h"
#include "pico/cyw43_arch.h"

// --- WiFi credentials ---
char ssid[] = "SSID";      // Edit this
char pass[] = "PASSWORD";  // Edit this

// --- micro-ROS objects ---
rcl_publisher_t publisher;
std_msgs__msg__UInt64 pub_msg;
rcl_node_t node;
rcl_allocator_t allocator;
rclc_support_t support;
rclc_timer_t timer;
rclc_executor_t executor;

// --- Timer callback: publishes incrementing counter ---
void timer_callback(rcl_timer_t *timer, int64_t last_call_time)
{
    (void) last_call_time;
    if (timer != NULL) {
        rcl_publish(&publisher, &pub_msg, NULL);
        pub_msg.data++;
    }
}

void setup_transport()
{
    rmw_uros_set_custom_transport(
        false,
        &picow_params,
        picow_udp_transport_open,
        picow_udp_transport_close,
        picow_udp_transport_write,
        picow_udp_transport_read
    );
}

void setup_ros()
{
    allocator = rcl_get_default_allocator();
    rclc_support_init(&support, 0, NULL, &allocator);
    rclc_node_init_default(&node, "pico_node", "", &support);

    rclc_publisher_init_best_effort(
        &publisher,
        &node,
        ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, UInt64),
        "pico_counter"
    );

    // Publish every 1000ms
    rclc_timer_init_default(&timer, &support, RCL_MS_TO_NS(1000), timer_callback);

    rclc_executor_init(&executor, &support.context, 1, &allocator);
    rclc_executor_add_timer(&executor, &timer);
}

int main()
{
    stdio_init_all();
    cyw43_arch_init_with_country(CYW43_COUNTRY_USA);
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
    cyw43_arch_enable_sta_mode();
    cyw43_arch_wifi_connect_timeout_ms(ssid, pass, CYW43_AUTH_WPA2_AES_PSK, 10000);

    setup_transport();

    // Wait up to 60s for agent
    const int timeout_ms = 1000;
    const uint8_t attempts = 60;
    rcl_ret_t ret = 0;
    int loop = 0;

    for (; loop < attempts; loop++) {
        ret = rmw_uros_ping_agent(timeout_ms, 1);
        if (ret == RCL_RET_OK) break;
    }
    if (loop == attempts) return ret;

    pub_msg.data = 0;
    setup_ros();

    while (true) {
        rclc_executor_spin_some(&executor, RCL_MS_TO_NS(10));
    }

    cyw43_arch_deinit();
    return 0;
}
