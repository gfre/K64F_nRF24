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
#include "Shell.h"
#include "PE_Error.h"

#include <STDDEF.h>

#define PAYLOAD_SIZE 		(24)
#define MSG_SIZE		(32)

static uint8 cdc_buffer[USB1_DATA_BUFF_SIZE];
static uint8 in_buffer[USB1_DATA_BUFF_SIZE];
static uint8 msgCtr = 0u;
static uint8 timeouts = 0u;
static uint8 configFlag = 0u;

typedef enum {
  RNETA_INITIAL, /* initialization state */
  RNETA_POWERUP, /* powered up the transceiver */
  RNETA_TX_RX /* ready to send and receive data */
} RNETA_State;


static RNETA_State appState = RNETA_INITIAL;

static uint8 bufIndex = 0u;
static uint8 msgCount = 0u;
static uint8 msg[RAPP_BUFFER_SIZE] = { 0u };

RAPP_MSG_Type GetMsgType(uint8 msgCount)
{
	switch(msgCount){
	case 0u :
		return RAPP_MSG_TYPE_MSG_A;
		break;
	case 1u :
		return RAPP_MSG_TYPE_MSG_B;
		break;
	case 2u :
		return RAPP_MSG_TYPE_MSG_C;
		break;
	default:
		return RAPP_MSG_TYPE_MSG_ERROR;
		break;
	}
}

static void PutMessage(uint8 currentByte, int i)
{

	if (0 == i && 0xAA == currentByte)
  	  {
  	    configFlag = 1u;

  	  }
  	else if (1 == i && 0xAA == currentByte && 1 == configFlag)
  	  {
  	    RAPP_BUF_PAYLOAD_START(msg)[0] = 0xAA;
  	    RAPP_BUF_PAYLOAD_START(msg)[1] = 0xAA;

  	    RAPP_PutPayload(msg, sizeof(msg), (uint8)PAYLOAD_SIZE, RAPP_MSG_TYPE_CONFIG_START, RNWK_ADDR_BROADCAST, RPHY_PACKET_FLAGS_NONE);
  	    configFlag = 0u;
  	  }
  	else if (1 == i && 0xBB == currentByte && 1 == configFlag)
  	  {

  	    RAPP_BUF_PAYLOAD_START(msg)[0] = 0xAA;
  	    RAPP_BUF_PAYLOAD_START(msg)[1] = 0xBB;

  	    RAPP_PutPayload(msg, sizeof(msg), (uint8)PAYLOAD_SIZE, RAPP_MSG_TYPE_CONFIG_STOP, RNWK_ADDR_BROADCAST, RPHY_PACKET_FLAGS_NONE);
  	    configFlag = 0u;
  	  }
  	else if (1 == i && 0xCC == currentByte && 1 == configFlag)
	  {

	    RAPP_BUF_PAYLOAD_START(msg)[0] = 0xAA;
	    RAPP_BUF_PAYLOAD_START(msg)[1] = 0xCC;

	    RAPP_PutPayload(msg, sizeof(msg), (uint8)PAYLOAD_SIZE, RAPP_MSG_TYPE_CONFIG_READY, RNWK_ADDR_BROADCAST, RPHY_PACKET_FLAGS_NONE);
	    configFlag = 0u;
	  }
  	else if (1 == i && 0xDD == currentByte && 1 == configFlag)
	  {

	    RAPP_BUF_PAYLOAD_START(msg)[0] = 0xAA;
	    RAPP_BUF_PAYLOAD_START(msg)[1] = 0xDD;

	    RAPP_PutPayload(msg, sizeof(msg), (uint8)PAYLOAD_SIZE, RAPP_MSG_TYPE_CONFIG_TRANS, RNWK_ADDR_BROADCAST, RPHY_PACKET_FLAGS_NONE);
	    configFlag = 0u;
	  }
  	else
  	  {
	    static uint8 msgCtr = 0u;

	    // Reset config Flag
	    configFlag = 0u;

	    if (0 == i)								// New message from Serial Port -> Reset
	    {
		    bufIndex = 0u;
		    msgCount = 0u;
	    }

	    if ((PAYLOAD_SIZE-1) == bufIndex)		// Payload is full -> Send
	    {
		    RAPP_BUF_PAYLOAD_START(msg)[bufIndex] = currentByte;

		    RAPP_PutPayload(msg, sizeof(msg), (uint8)PAYLOAD_SIZE, GetMsgType(msgCount), RNWK_ADDR_BROADCAST, RPHY_PACKET_FLAGS_NONE);



    /*
		    for(int j = 0; j < PAYLOAD_SIZE; j++)
		    {
			    CLS1_printf("%d - ",RAPP_BUF_PAYLOAD_START(msg)[j]);
		    }
		    CLS1_printf("\r\n\r\n");
    */
		    bufIndex = 0u;
		    msgCount++;
	    } else
	    {
		    RAPP_BUF_PAYLOAD_START(msg)[bufIndex] = currentByte;
		    bufIndex++;
	    }
  	  }



}
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



			while(i<sizeof(in_buffer) && CDC1_GetChar(&in_buffer[i])==ERR_OK)
			{
				PutMessage(in_buffer[i], i);
				i++;
			}
			//CLS1_printf("\r\n\r\n");
			in_buffer[i] = '\0';

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

