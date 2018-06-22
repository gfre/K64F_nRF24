/**
 * \file
 * \brief This is a configuration file for the RNet stack
 * \author (c) 2014 Erich Styger, http://mcuoneclipse.com/
 * \note MIT License (http://opensource.org/licenses/mit-license.html)
 *
 * Here the stack can be configured using macros.
 */

#ifndef __RNET_APP_CONFIG__
#define __RNET_APP_CONFIG__

/*! type ID's for application messages */
typedef enum {
   RAPP_MSG_TYPE_DATA = 0x50
  ,RAPP_MSG_TYPE_DATA_A = RAPP_MSG_TYPE_DATA | 0x01
  ,RAPP_MSG_TYPE_DATA_B = RAPP_MSG_TYPE_DATA | 0x02
  ,RAPP_MSG_TYPE_DATA_C = RAPP_MSG_TYPE_DATA | 0x04
  ,RAPP_MSG_TYPE_CMD = 0x60
  ,RAPP_MSG_TYPE_CMD_SW_RESET = RAPP_MSG_TYPE_CMD | 0x01
  ,RAPP_MSG_TYPE_CMD_INIT     = RAPP_MSG_TYPE_CMD | 0x02
  ,RAPP_MSG_TYPE_CMD_STEADY   = RAPP_MSG_TYPE_CMD | 0x03
  ,RAPP_MSG_TYPE_CMD_GO       = RAPP_MSG_TYPE_CMD | 0x04
  ,RAPP_MSG_TYPE_CFG = 0x70
  ,RAPP_MSG_TYPE_CFG_PARAM = RAPP_MSG_TYPE_CFG | 0x01
  ,RAPP_MSG_TYPE_CFG_PID   = RAPP_MSG_TYPE_CFG | 0x02
  ,RAPP_MSG_TYPE_CFG_BCKST = RAPP_MSG_TYPE_CFG | 0x03
  ,RAPP_MSG_TYPE_ERROR = 0xFF
} RAPP_MSG_Type;


#endif /* __RNET_APP_CONFIG__ */
