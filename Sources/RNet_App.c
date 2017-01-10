/**
 * \file
 * \brief This is main application file
 * \author (c) 2014 Erich Styger, http://mcuoneclipse.com/
 * \note MIT License (http://opensource.org/licenses/mit-license.html)
 *
 * This module implements the application part of the program.
 */

#include "RNet_App.h"
#include "RStack.h"
#include "RApp.h"
#include "FRTOS1.h"
#include "RPHY.h"
#include "Radio.h"
#include "LED1.h"
#include "LED2.h"
#include "LED3.h"
#include "KEY1.h"
#include <stdio.h>
#include "USB1.h"

static uint8_t cdc_buffer[USB1_DATA_BUFF_SIZE];
static uint8_t in_buffer[USB1_DATA_BUFF_SIZE];

typedef enum {
  RNETA_INITIAL, /* initialization state */
  RNETA_POWERUP, /* powered up the transceiver */
  RNETA_TX_RX /* ready to send and receive data */
} RNETA_State;

static RNETA_State appState = RNETA_INITIAL;


static void RadioPowerUp(void) {
  /* need to ensure that we wait at least 100 ms (I use 150 ms here) after power-on of the transceiver */
  portTickType xTime;
  
  xTime = FRTOS1_xTaskGetTickCount();
  if (xTime<(150/portTICK_RATE_MS)) {
    /* not powered for 100 ms: wait until we can access the radio transceiver */
    xTime = (150/portTICK_RATE_MS)-xTime; /* remaining ticks to wait */
    FRTOS1_vTaskDelay(xTime);
  }
  (void)RADIO_PowerUp(); /* enable the transceiver */
}

static void Process(void) {
  for(;;) {
    switch(appState) {
    case RNETA_INITIAL:
      appState = RNETA_POWERUP;
      continue;
      
    case RNETA_POWERUP:
      RadioPowerUp();
      appState = RNETA_TX_RX;
      break;
      
    case RNETA_TX_RX:
      (void)RADIO_Process();
      break;
  
    default:
      break;
    } /* switch */
    break; /* break for loop */
  } /* for */
}

static void RNetTask(void *pvParameters) {
	 (void)pvParameters; /* not used */
	  if (RAPP_SetThisNodeAddr(RNWK_ADDR_BROADCAST)!=ERR_OK) { /* set a default address */
	    for(;;); /* "ERR: Failed setting node address" */
	  }

	  appState = RNETA_INITIAL; /* initialize state machine state */
	  for(;;) {
		  Process(); /* process state machine */
	      while(CDC1_App_Task(cdc_buffer, sizeof(cdc_buffer))==ERR_BUSOFF) {
	        /* device not enumerated */
	        LED1_Neg(); LED2_Off();
	        WAIT1_Waitms(10);
	      }
	      LED1_Off(); LED2_Neg();


		  if (CDC1_GetCharsInRxBuf()!=0) {
			int i = 0;
			while(   i<sizeof(in_buffer)-1
				  && CDC1_GetChar(&in_buffer[i])==ERR_OK
				 )
			{
			  i++;
			}
			in_buffer[i] = '\0';
			(void)CDC1_SendString((unsigned char*)"Got Message: ");
			(void)CDC1_SendString(in_buffer);
			RAPP_SendPayloadDataBlock(&in_buffer, sizeof(in_buffer), RAPP_MSG_TYPE_PING, RNWK_ADDR_BROADCAST, RPHY_PACKET_FLAGS_NONE);
			(void)CDC1_SendString((unsigned char*)"Transmission: OK!\r\n");
		  }
		  FRTOS1_vTaskDelay(pdMS_TO_TICKS(10));
	  }
}


void RNETA_Init(void) {
  RSTACK_Init(); /* initialize stack */


   if (xTaskCreate(
        RNetTask,  /* pointer to the task */
        "RNet", /* task name for kernel awareness debugging */
        configMINIMAL_STACK_SIZE, /* task stack size */
        (void*)NULL, /* optional task startup argument */
        tskIDLE_PRIORITY,  /* initial priority */
        (xTaskHandle*)NULL /* optional task handle to create */
      ) != pdPASS) {
    /*lint -e527 */
    for(;;){}; /* error! probably out of memory */
    /*lint +e527 */
  }
}

