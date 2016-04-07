//------------------------------------------------------------
// AVRでD-Linkの通信をするためのライブラリ
// 無駄を省いて新規に作りなおし
//
// Author : botamochi
// Date   : 2016/04/07
//------------------------------------------------------------
#include <avr/io.h>
#include <util/delay.h>
#include "dlink.h"

// 入出力関係のマクロ
#define CONCAT(a, b) a ## b
#define CONCAT_EXPAND(a, b) CONCAT(a, b)
#define DLINK_PORT CONCAT_EXPAND(PORT, DLINK_IOPORTNAME)
#define DLINK_DDR  CONCAT_EXPAND(DDR,  DLINK_IOPORTNAME)
#define DLINK_PIN  CONCAT_EXPAND(PIN,  DLINK_IOPORTNAME)

#define DLINK_IO_SET()  DLINK_PORT |= (1<<DLINK_PINNUMBER)
#define DLINK_IO_CLR()  DLINK_PORT &= ~(1<<DLINK_PINNUMBER)
#define DLINK_IO_READ() (DLINK_PIN & (1<<DLINK_PINNUMBER))

#define DLINK_MODE_IN()  DLINK_DDR &= ~(1<<DLINK_PINNUMBER)
#define DLINK_MODE_OUT() DLINK_DDR |= (1<<DLINK_PINNUMBER)

// 送信時のパルス幅の定義
#define DLINK_RESET_PULSE_LOW_TIME  60000L
#define DLINK_START_PULSE_HIGH_TIME 2000L
#define DLINK_START_PULSE_LOW_TIME  1000L
#define DLINK_LONG_PULSE_HIGH_TIME  2500L
#define DLINK_LONG_PULSE_LOW_TIME   1500L
#define DLINK_SHORT_PULSE_HIGH_TIME 1000L
#define DLINK_SHORT_PULSE_LOW_TIME  3000L

// タイムアウト
#define DLINK_PULSE_READ_TIMEOUT    5000L

// 1us遅延関数
#define DELAY_US(time) _delay_us(time)

static unsigned int dlink_read_pulse(void);

//------------------------------------------------------------
// 初期化処理
//------------------------------------------------------------
void dlink_init()
{
  DLINK_IO_SET();
  DLINK_MODE_IN();
}


//------------------------------------------------------------
// 1フレーム送信
//------------------------------------------------------------
void dlink_send_frame(dframe frm)
{
  int i;

  DLINK_MODE_OUT();

  //リセットパルス
  DLINK_IO_CLR();
  DELAY_US(DLINK_RESET_PULSE_LOW_TIME);

  // スタートパルス
  DLINK_IO_SET(); DELAY_US(DLINK_START_PULSE_HIGH_TIME);
  DLINK_IO_CLR(); DELAY_US(DLINK_START_PULSE_LOW_TIME);

  // データパルス
  for (i = 0; i < 16; i++) {
    if (frm & (1<<i)) {
      DLINK_IO_SET(); DELAY_US(DLINK_LONG_PULSE_HIGH_TIME);
      DLINK_IO_CLR(); DELAY_US(DLINK_LONG_PULSE_LOW_TIME);
    } else {
      DLINK_IO_SET(); DELAY_US(DLINK_SHORT_PULSE_HIGH_TIME);
      DLINK_IO_CLR(); DELAY_US(DLINK_SHORT_PULSE_LOW_TIME);
    }
  }
  DLINK_IO_SET();
  DLINK_MODE_IN();
  DELAY_US(1000);
}


//------------------------------------------------------------
// 1フレーム受信
//------------------------------------------------------------
dframe dlink_recv_frame()
{
  int i;
  dframe frm = 0;
  unsigned int thr, tm;

  // リセットパルス
  while (DLINK_IO_READ() != 0);
  while (DLINK_IO_READ() == 0);

  // スタートパルス
  thr = dlink_read_pulse();
  if (thr == 0) return 0;

  // データパルス
  for (i = 0; i < 16; i++) {
    tm = dlink_read_pulse();
    if (tm == 0) return 0;
    if (tm > thr) frm |= (1<<i);
  }
  while (DLINK_IO_READ() == 0);
  return frm;
}

//------------------------------------------------------------
// パルスの幅を計測
//------------------------------------------------------------
static unsigned int dlink_read_pulse()
{
  unsigned int time = 0;
  unsigned int timeout = DLINK_PULSE_READ_TIMEOUT;

  while (DLINK_IO_READ() == 0) {
    if (timeout == 0) return 0;
    timeout--;
    DELAY_US(1);
  }
  while (DLINK_IO_READ() != 0) {
    if (timeout == 0) return 0;
    timeout--;
    time++;
    DELAY_US(1);
  }
  return time;
}
