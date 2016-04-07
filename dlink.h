//------------------------------------------------------------
// AVRでD-Linkの通信をするためのライブラリ
// 無駄を省いて新規に作りなおし
//
// Author : botamochi
// Date   : 2016/04/07
//------------------------------------------------------------
#ifndef __DLINK_H__
#define __DLINK_H__

typedef unsigned int dframe;

#define DLINK_IOPORTNAME B
#define DLINK_PINNUMBER 0

//------------------------------------------------------------
// ライブラリ関数
//------------------------------------------------------------
void   dlink_init(void);
void   dlink_send_frame(dframe frm);
dframe dlink_recv_frame(void);

#endif
