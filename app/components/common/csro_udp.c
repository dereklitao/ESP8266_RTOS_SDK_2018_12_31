#include "csro_common.h"
#include "cJSON.h"

static EventGroupHandle_t   wifi_event_group;
static const EventBits_t    CONNECTED_BIT   = BIT0;
int                         udp_sock;


static void process_udp_message(char *udp_buf)
{
    cJSON *serv_ip, *serv_mac, *time_info;
    cJSON *json = cJSON_Parse(udp_buf);
    if (json != NULL) 
    {
        serv_ip     = cJSON_GetObjectItem(json, "ip");
        serv_mac    = cJSON_GetObjectItem(json, "mac");
        time_info   = cJSON_GetObjectItem(json, "time");

        if ((serv_ip != NULL) && (serv_mac != NULL) && (serv_ip->type == cJSON_String) && (serv_mac->type == cJSON_String)) 
        {
            if (strlen(serv_ip->valuestring)>=4 && strlen(serv_mac->valuestring)>=10) 
            {
                if ((strcmp((char *)serv_ip->valuestring, (char *)mqtt.broker) != 0) || (strcmp((char *)serv_mac->valuestring, (char *)mqtt.prefix) != 0)) 
                {
                    strcpy((char *)mqtt.broker, (char *)serv_ip->valuestring);
                    strcpy((char *)mqtt.prefix, (char *)serv_mac->valuestring);
                    mqtt.client.isconnected = 0;
                }
            }
        }
        if ((time_info != NULL) && (time_info->type == cJSON_String)) { csro_datetime_set(time_info->valuestring); }
    }
    cJSON_Delete(json);
}


static esp_err_t wifi_event_handler(void *ctx, system_event_t *event)
{
    if (event->event_id == SYSTEM_EVENT_STA_START) {
        tcpip_adapter_set_hostname(TCPIP_ADAPTER_IF_STA, sysinfo.host_name);
        esp_wifi_connect();
    }
    else if (event->event_id == SYSTEM_EVENT_STA_GOT_IP) {
        xEventGroupSetBits(wifi_event_group, CONNECTED_BIT);
    }
    else if (event->event_id == SYSTEM_EVENT_STA_DISCONNECTED) {
        esp_wifi_connect();
        xEventGroupClearBits(wifi_event_group, CONNECTED_BIT);
    }
    return ESP_OK;
}


static bool create_udp_server(void)
{
    udp_sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
    if (udp_sock < 0) { return false; }
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(5000);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(udp_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) 
    {
        close(udp_sock);
        return false;
    }
    return true;
}

void csro_udp_receive_task(void *pvParameters)
{
    csro_system_set_status(NORMAL_START_NOWIFI);
    wifi_event_group = xEventGroupCreate();
    tcpip_adapter_init();
    esp_event_loop_init(wifi_event_handler, NULL);

    wifi_init_config_t config = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&config);
    wifi_config_t wifi_config;
    strcpy((char *)wifi_config.sta.ssid, (char *)sysinfo.router_ssid);
    strcpy((char *)wifi_config.sta.password, (char *)sysinfo.router_pass);

    esp_wifi_set_mode(WIFI_MODE_STA);
    esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config);
    esp_wifi_start();

    static char udp_rx_buffer[512];
    while(true)
    {
        bool sock_status = false;
        while(sock_status == false)
        {
            vTaskDelay(1000 / portTICK_RATE_MS);
            sock_status = create_udp_server();
        }
        while(true)
        {
            struct sockaddr_in source_addr;
            socklen_t socklen = sizeof(source_addr);
            bzero(udp_rx_buffer, 512);
            int len = recvfrom(udp_sock, udp_rx_buffer, sizeof(udp_rx_buffer) - 1, 0, (struct sockaddr *)&source_addr, &socklen);
            if (len < 0) { break; }
            process_udp_message(udp_rx_buffer);
        }
    }
    vTaskDelete(NULL);
}