#include <stdio.h>
#include <time.h>
#include "mulprec.h"

#define N 3500    // シグマ内部の項の総数

int main(void) {
  // 時間計測開始
  clock_t start = clock(), end;

  // シグマ内部1個前の項の分母の値
  Float* M_before = new Float();
  M_before->setByStr((char*)"+3");

  // 1を示す多倍長変数
  Float* one = new Float();
  one->setByStr((char*)"+1");
  // 2を示す多倍長変数
  Float* two = new Float();
  two->setByStr((char*)"+2");

  // シグマ内部の項の値
  Float* div_num = new Float();
  // シグマの合計を格納する多倍長変数
  Float* sum = new Float();

  // シグマでk=0の処理
  sum->setByStr((char*)"+1");
  Float* workval = new Float();

  // シグマでk=1の処理
  goldschmidt(one, M_before, div_num, DECPOS, DIGIT - 1);
  addFloat(sum, div_num, workval, 0, DIGIT - 1);
  workval->copyAll(sum, 0, DIGIT - 1);

  // シグマでk=2以降の処理
  for (int k = 2; k < N; k++) {
    // シグマ内部の項の分母を求める
    multipleFloat(M_before, two, workval);
    addFloat(workval, one, M_before, DECPOS, DIGIT - 1);
    // シグマ内部の項を求める
    goldschmidt(one, M_before, div_num, DECPOS, DIGIT - 1);
    // シグマの合計に反映する
    addFloat(sum, div_num, workval, 0, DIGIT - 1);
    workval->copyAll(sum, 0, DIGIT - 1);
  }

  // 時間計測終了
  end = clock();

  // 計算結果を表示
  printf("result = ");
  sum->disp(DECPOS - 1000, DECPOS);
  printf("\n");

  // かかった時間の表示
  printf("time = %.2lf[s]\n", (double)(end - start) / CLOCKS_PER_SEC);

  return 0;
}