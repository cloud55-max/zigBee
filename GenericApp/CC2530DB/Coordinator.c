#include "OSAL.h"
#include "AF.h"
#include "ZDApp.h"
#include "ZDOBject.h"
#include "ZDProfile.h"
#include <string.h>
#include "Coordinator.h"
#include "DebugTrace.h"
#if !defined( WIN32)
#include "OnBoard.h"
#endif
#include "hal_lcd.h"
#include "hal_led.h"
#include "hal_key.h"
#include "hal_uart.h"

const cId_t GenericApp_ClusterList[GENERICAPP_MAX_CLUSTERS ] =
{
    GENERICAPP_CLUSTERID
};

const SimpleDescriptionFormat_t GenericApp_SimpleDesc =
{
    GENERICAPP_ENDPOINT,
    GENERICAPP_PROFID,
    GENERICAPP_DEVICEID,
    GENERICAPP_DEVICE_VERSION,
    GENERICAPP_FLAGS,
    0,
    (cId_t *)NULL,
    GENERICAPP_MAX_CLUSTERS,
    (cId_t *)GenericApp_ClusterList
};
endPointDesc_t GenericApp_epDesc;
byte GenericApp_TaskID;
byte GenericApp_TransID;
devStates_t GenericApp_NwkState;

void GenericApp_Init( byte task_id )
{
    GenericApp_TaskID   =   task_id;
    GenericApp_NwkState   =   DEV_INIT;
    GenericApp_TransID    =   0;
    GenericApp_epDesc.endPoint    =   GENERICAPP_ENDPOINT;
    GenericApp_epDesc.task_id   =   &GenericApp_TaskID;
    GenericApp_epDesc.simpleDesc    =   
      (SimpleDescriptionFormat_t *)&GenericApp_SimpleDesc;
    GenericApp_epDesc.latencyReq    =   noLatencyReqs;
    afRegister( &GenericApp_epDesc );
}

UINT16 GenericApp_ProcessEvent( byte task_id, UINT16 events )
{
  afIncomingMSGPacket_t *MSGpkt;
  if ( events &SYS_EVENT_MSG )
    {
     MSGpkt = (afIncomingMSGPacket_t *)osal_msg_receive
       ( GenericApp_TaskID);
     while ( MSGpkt )
      {
        switch ( MSGpkt->hdr.event )
        {
        case ZDO_STATE_CHANGE:
          GenericApp_NwkState = (devStates_t)(MSGpkt->hdr.
          status);
          if (GenericApp_NwkState ==DEV_END_DEVICE)
          {
            GenericApp_SendTheMessage();
          }
          break;
        default:
          break;
        }
        osal_msg_deallocate( (uint8 *)MSGpkt);
        MSGpkt = (afIncomingMSGPacket_t*)osal_msg_receive(GenericApp_TaskID );
}
        return (events ^ SYS_EVENT_MSG);
    }
         return 0;
     }
void GenericApp_SendTheMessage( void )
{
    unsigned char theMessageData[4] = "LED";
    afAddrType_t my_DstAddr;
    my_DstAddr.addrMode = (afAddrMode_t)Addr16Bit;
    my_DstAddr.endPoint = GENERICAPP_ENDPOINT;
    my_DstAddr.addr.shortAddr = 0x0000;
    AF_DataRequest(&my_DstAddr,&GenericApp_epDesc,GENERICAPP_CLUSTERID,
                          3,
                          theMessageData,
                          &GenericApp_TransID,
                          AF_DISCV_ROUTE,
                          AF_DEFAULT_RADIUS);
                          HalLedBlink(HAL_LED_2,0, 50,500);
        }
         
         