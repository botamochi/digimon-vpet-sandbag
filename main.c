//------------------------------------------------------------
// 初代デジタルモンスター用サンドバッグ
//
// Author : botamochi
// Date   : 2016/04/07
//
// MCU    : ATtiny13A
// CLOCK  : 1.2 MHz
// FUSE   : HIGH 0xFF, LOW 0x6A
//------------------------------------------------------------
#include <avr/io.h>
#include "dlink.h"

#define VER 0b0000 // バージョン
#define STR 0b0000 // トレーニングによる強さ(?)
#define RES 0b0010 // 勝敗データ(負け)

#define SWITCH_PIN PB1

//------------------------------------------------------------
// main関数
//------------------------------------------------------------
int main(void)
{
  dframe dat1, dat2;
  unsigned int pos, count = 0;

  // ポートの初期化
  PORTB = (1<<SWITCH_PIN);
  DDRB = 0x00;
  dlink_init();

  while (1) {
    // スイッチが押されるまで待機
    while ((PINB & (1<<SWITCH_PIN)) != 0) {
      count++;
    }
    // デジモンの種類をランダムに設定(値は3〜14)
    pos = (count % 12) + 3;
    // 送信するデータを生成
    dat1 = (STR << 4) | pos;
    dat2 = (VER << 4) | RES;
    // 通信
    dlink_send_frame((~dat1 << 8) | (dat1 & 0xFF));
    dlink_recv_frame();
    dlink_send_frame((~dat2 << 8) | (dat2 & 0xFF));
    dlink_recv_frame();
  }
  return 0;
}
