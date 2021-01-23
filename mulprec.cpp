#include <stdio.h>
#include "mulprec.h"

/* 多倍長変数を大小比較する(符号を考慮しない) */
int compData(Integer* a, Integer* b, int bottom, int top) {
    for (int digit_p = top; digit_p >= bottom; digit_p--) {
        // 比較する桁が a > b の場合は, 大小関係が確定する
        if (a->getOneData(digit_p) > b->getOneData(digit_p)) {
            return GREATER_THAN;
        }

        // 比較する桁が a < b の場合は, 大小関係が確定する
        if (a->getOneData(digit_p) < b->getOneData(digit_p)) {
          return LESS_THAN;
        }
    }

    return EQUAL;
}

/* - 多倍長実数を大小比較する(符号を考慮する) - */
int compFloat(Float* a, Float* b, int bottom, int top) {
    // 比較元の符号の取得
    int a_sign = a->getSign();
    int b_sign = b->getSign();

    // aが正, bが負 なので, a > bであることが確定する
    if (a_sign == POSITIVE && a_sign == NEGATIVE)
        return GREATER_THAN;
    // aが負, bが正なので, a < bであることが確定する
    if (a_sign == NEGATIVE && b_sign == POSITIVE)
        return LESS_THAN;

    // 符号だけでは確定しないので, 上位桁から大小比較する
    for (int digit_p = top; digit_p >= bottom; digit_p--) {
        // 比較する桁が a > b の場合は, 大小関係が確定する
        if (a->getOneData(digit_p) > b->getOneData(digit_p)) {
            return (a_sign == POSITIVE) ? GREATER_THAN : LESS_THAN;
        }

        // 比較する桁が a < b の場合は, 大小関係が確定する
        if (a->getOneData(digit_p) < b->getOneData(digit_p)) {
            return (a_sign == POSITIVE) ? LESS_THAN : GREATER_THAN;
        }
    }

    // 符号が等しく, 全桁が等しいので, a == b であることが確定する
    return EQUAL;
}

/* - 多倍長実数の加算 - */
int addFloat(Float* a, Float* b, Float* c, int bottom, int top) {
    int carry;    // キャリー

    // aが正の数, bが負の数の場合
    if(a->getSign() == POSITIVE && b->getSign() == NEGATIVE) {
        Float* b_abs = new Float();
        b->getAbs(b_abs);
        carry = subFloat(a, b_abs, c, bottom, top);
        return carry;
    }
    // aが負の数, bが正の数の場合
    if (a->getSign() == NEGATIVE && b->getSign() == POSITIVE) {
        Float* a_abs = new Float();
        a->getAbs(a_abs);
        carry = subFloat(a_abs, b, c, 0, DIGIT - 1);
        return carry;
    }
    // aもbも負の数の場合
    if (a->getSign() == NEGATIVE && b->getSign() == NEGATIVE) {
        Float *a_abs = new Float();
        Float* b_abs = new Float();
        a->getAbs(a_abs);
        b->getAbs(b_abs);
        carry = addFloat(a_abs, b_abs, c, 0, DIGIT - 1);
        c->setSign(NEGATIVE);
        return carry;
    }

    // aもbも正の数の場合
    c->setSign(POSITIVE);

    // 桁内部の計算
    carry = 0;
    int digit_p;
    for (digit_p = 0; digit_p < DIGIT; digit_p++){
        c->setOneData((a->getOneData(digit_p) + b->getOneData(digit_p) + carry) % BASE, digit_p);
        carry = (a->getOneData(digit_p) + b->getOneData(digit_p) + carry) / BASE;
    }

    return carry;
}

/* - 多倍長実数の減算 - */
int subFloat(Float* a, Float* b, Float* c, int bottom, int top) {
    int borrow;   // ボロー

    // aが正の数, bが負の数の場合
    if (a->getSign() == POSITIVE && b->getSign() == NEGATIVE) {
        Float* b_abs = new Float();
        b->getAbs(b_abs);
        borrow = addFloat(a, b_abs, c, bottom, top);
        c->setSign(POSITIVE);
        return borrow;
    }
    // aが負の数, bが正の数の場合
    if (a->getSign() == NEGATIVE && b->getSign() == POSITIVE) {
        Float *a_abs = new Float();
        a->getAbs(a_abs);
        borrow = addFloat(a_abs, b, c, bottom, top);
        c->setSign(NEGATIVE);
        return borrow;
    }
    // aもbも負の数の場合
    if (a->getSign() == NEGATIVE && b->getSign() == NEGATIVE) {
        Float *a_abs = new Float();
        Float *b_abs = new Float();
        a->getAbs(a_abs);
        b->getAbs(b_abs);
        borrow = subFloat(a_abs, b_abs, c, bottom, top);
        c->setSign((c->getSign() == POSITIVE) ? NEGATIVE : POSITIVE);
        return borrow;
    }

    // aもbも正の数の場合
    // a - b が負の数になる場合
    if (compFloat(a, b, bottom, top) == LESS_THAN) {
        borrow = subFloat(b, a, c, bottom, top);
        c->setSign(NEGATIVE);
        return borrow;
    }

    // a - b が正の数になる場合
    c->setSign(POSITIVE);

    // 桁内部の計算
    borrow = 0;
    int digit_p;
    for (digit_p = 0, borrow = 0; digit_p < DIGIT; digit_p++){
        c->setOneData(a->getOneData(digit_p) - b->getOneData(digit_p) - borrow, digit_p);
        // 繰り下がりする場合
        if (c->getOneData(digit_p) < 0) {
            borrow = 1;
            c->addOneData(BASE, digit_p);
        }
        // 繰り下がりしない場合
        else {
            borrow = 0;
        }
    }

    return 0;
}

/* - 多倍長実数の乗算 - */
int multipleFloat(Float* a, Float* b, Float* c) {
    // 整数部で桁が入っている一番上の桁
    int a_digit = a->getIntegerDigit() + DECPOS;
    int b_digit = b->getIntegerDigit() + DECPOS;

    // オーバーフロー
    if (a_digit + b_digit - DECPOS > DIGIT) {
        printf("[Error] data was overflowed.(integer-part)\n");
        return -1;
    }

    // 小数部で桁が入っている一番下の桁
    int a_digit_min = DECPOS - a->getDecDigit();
    int b_digit_min = DECPOS - b->getDecDigit();
    int digit_min_smaller = (a_digit_min > b_digit_min) ? b_digit_min : a_digit_min;

    // 符号の計算
    c->setSign(
        ((a->getSign() == POSITIVE && b->getSign() == POSITIVE) || (a->getSign() == NEGATIVE && b->getSign() == NEGATIVE)) ? POSITIVE : NEGATIVE);

    // 桁内部の計算
    int a_p, b_p, c_p;
    c->clearByZero(0, DIGIT - 1);
    for (b_p = digit_min_smaller; b_p < b_digit; b_p++) {
        for (a_p = digit_min_smaller; a_p < a_digit; a_p++) {
            c_p = a_p + b_p - DECPOS;

            // オーバーフロー(内部にどの数字が入っていてもオーバーフローする)
            if (c_p > DIGIT - 1) {
                printf("[Error] data was overflowed.\n");
                return -1;
            }

            // アンダーフロー(切り捨て)
            if (c_p < 0) {
              continue;
            }

            c->addOneData(a->getOneData(a_p) * b->getOneData(b_p), c_p);
            if (c->getOneData(c_p) >= BASE) {
                if(c_p + 1 < DIGIT) {
                    c->addOneData(c->getOneData(c_p) / BASE, c_p + 1);
                    c->setOneData(c->getOneData(c_p) % BASE, c_p);
                }
                // オーバーフロー(内部に入っている数字によってオーバーフローする)
                else {
                    printf("c_p = %d\n", c_p);
                    c->setOneData(c->getOneData(c_p) % BASE, c_p);
                    return -1;
                }
            }
        }
    }

    // 正常終了
    return true;
}

/* - 多倍長実数の除算(ゴールドシュミット法) - */
int goldschmidt(Float *a, Float *b, Float *c, int bottom, int top) {
  // 計算に利用する多倍長変数を変更しないように別の変数に格納
  Float* a_workval = new Float();
  Float* b_workval = new Float();
  a->copyData(a_workval, 0, DIGIT - 1);
  b->copyData(b_workval, 0, DIGIT - 1);
  a_workval->setSign(POSITIVE);
  b_workval->setSign(POSITIVE);

  // ゼロで割ることの防止
  if (b_workval->isZero(bottom, top)) {
      printf("divide by zero\n");
      return -1;
  }

  // 符号の取得
  int a_sign = a_workval->getSign();
  int b_sign = b_workval->getSign();

  // 作業用変数
  Float *workval1 = new Float();
  Float *workval2 = new Float();

  // 基準値の整数部分の桁数の取得
  int digit_base = b->getIntegerDigit();

  // 基準値
  Float *base = new Float();
  base->clearByZeroAll(0, DIGIT- 1);
  base->setOneData(1, DECPOS + digit_base);

  // 1を示す多倍長変数
  Float *one = new Float();
  one->clearByZeroAll(0, DIGIT- 1);
  one->setOneData(1, DECPOS);

  // 基準値と割る多倍長変数から求められる, 割られる多倍長変数と割る多倍長変数に掛ける多倍長変数
  Float *mul = new Float();
  // 取得した1個前の多倍長変数mulを格納
  Float *mul_old = new Float();

  for (int loop_count = 0; true; loop_count++)
  {
    if (loop_count == 0) {
      // 桁内部の除算終了
      if(compData(b_workval, base, bottom, top) != LESS_THAN) {
        break;
      }
    }
    else {
      // 桁内部の除算終了
      if(compData(mul, one, 0, DIGIT - 1) != GREATER_THAN) {
        break;
      }
      if (loop_count >= 2 && compData(mul, mul_old, 0, DECPOS - 1) == EQUAL) {
        break;
      }

      workval1->copyData(a_workval, 0, DIGIT - 1);
      workval2->copyData(b_workval, 0, DIGIT - 1);
      // 1個前の多倍長変数mulを取得
      mul->copyData(mul_old, 0, DECPOS - 1);
    }

    // mulを求める
    subFloat(base, b_workval, workval2, 0, DIGIT - 1);
    workval2->copyData(mul, digit_base, DIGIT - 1, 0, DIGIT - digit_base - 1);
    mul->setOneData(1, DECPOS);

    // 割られる多倍長変数と割る多倍長変数にmulを掛ける
    multipleFloat(a_workval, mul, workval1);
    multipleFloat(b_workval, mul, workval2);
  }

  // 計算結果を格納する多倍長変数への結果の反映
  c->clearByZero(DIGIT - digit_base, DIGIT - 1);
  workval1->copyData(c, digit_base, DIGIT - 1, 0, DIGIT - digit_base - 1);

  // 符号の計算
  if (a_sign == POSITIVE && b_sign == POSITIVE) {
    c->setSign(POSITIVE);
  }
  else if (a_sign == POSITIVE && b_sign == NEGATIVE) {
    c->setSign(NEGATIVE);
  }
  else if (a_sign == NEGATIVE && b_sign == POSITIVE) {
    c->setSign(NEGATIVE);
  }
  else {
    c->setSign(POSITIVE);
  }

  return true;
}
