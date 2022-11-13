/*
 * usbc.h
 *
 *  Created on: Nov 7, 2022
 *      Author: Nelson
 */

#ifndef USBC_H_
#define USBC_H_

#endif /* USBC_H_ */
#include  "usbd_def.h"
#include  "usbd_core.h"

#define CUSTOM_EPIN_ADDR                 0x81U
#define CUSTOM_EPOUT_ADDR                0x01U
#define CUSTOM_EPIN_SIZE                 0x40U
#define CUSTOM_EPOUT_SIZE                0x40U

#define USB_CUSTOM_CONFIG_DESC_SIZ       34U
#define USB_CUSTOM_DESC_SIZ              9U

typedef enum
{
  CUSTOM_IDLE = 0,
  CUSTOM_BUSY,
}
CUSTOM_StateTypeDef;


typedef struct
{
  uint32_t             Protocol;
  uint32_t             IdleState;
  uint32_t             AltSetting;
  CUSTOM_StateTypeDef     state;
}
USBD_CUSTOM_HandleTypeDef;

#define CUSTOM_FS_BINTERVAL            0x0AU

extern USBD_ClassTypeDef  USBD_CUSTOM;
