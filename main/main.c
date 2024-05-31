#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_event.h>
#include <string.h>

#define COUNTER_MAX         20
#define DATA_EVT_CFULL      "El contador llego al maximo"
#define DATA_EVT_C_MID      "El contador esta en la mitad de la cuenta"
#define DATA_EVT_CLEAR      "El contador fue reiniciado"
// Defino la base del evento

ESP_EVENT_DEFINE_BASE(TEST_EVENT);


typedef enum {
    EVENT_COUNTER_CLEAR,
    EVENT_COUNTER_IN_THE_MIDDLE,
    EVENT_COUNTER_FULL,

} counter_event_t;



static void run_on_event(void* args,esp_event_base_t base, uint32_t id, void* event_data){
    char* message = (char*)event_data;
    printf("[Handler event] %s\n",message);

}




esp_event_loop_handle_t loop_handler;


void app_main(void)
{
    printf("Inicio prueba con eventos\n");
    esp_event_loop_args_t loop_config = {
    .task_name = 'Loop',
    .queue_size = 10,
    .task_core_id = tskNO_AFFINITY,
    .task_priority = uxTaskPriorityGet(NULL),
    .task_stack_size = 2 * 1024
    };
    uint32_t counter = 0;
    esp_event_loop_create(&loop_config,&loop_handler);
    esp_event_handler_register_with(loop_handler,TEST_EVENT,ESP_EVENT_ANY_ID,run_on_event,NULL);


    while(1){
        printf("\nCounter:%lu\n",counter);
        if(counter == (COUNTER_MAX / 2) ){
            // Evento  EVENT_COUNTER_IN_THE_MIDDLE
            esp_event_post_to(loop_handler,TEST_EVENT,EVENT_COUNTER_IN_THE_MIDDLE,DATA_EVT_C_MID,strlen(DATA_EVT_C_MID)+1,pdMS_TO_TICKS(1000));


        }
        if(counter >= COUNTER_MAX){
            // Evento EVENT_COUNTER_FULL
            esp_event_post_to(loop_handler,TEST_EVENT,EVENT_COUNTER_FULL,DATA_EVT_CFULL,strlen(DATA_EVT_CFULL)+1,1000);
            counter = 0;
            esp_event_post_to(loop_handler,TEST_EVENT,EVENT_COUNTER_CLEAR,DATA_EVT_CLEAR,strlen(DATA_EVT_CLEAR)+1,1000);

        }
        vTaskDelay(500 / portTICK_PERIOD_MS);
        counter++;

    }

}