/* インクルードガード */
#ifndef MULPREC
#define MULPREC

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* 計算用の多倍長変数 */
#define BASE 10         // 基数
#define DIGIT 35000     // 桁数
#define DECPOS 5250     // 小数点の位置

/* 多倍長変数の符号を示す */
#define POSITIVE 1      // 正
#define NEGATIVE -1     // 負

/* 2つの多倍長変数の大小を示す */
#define GREATER_THAN 1  // 関数の引数の記述で考えた場合, 左側に表記された多倍長変数の方が大きい
#define EQUAL 0         // 多倍長変数の大小が等しい
#define LESS_THAN -1    // 関数の引数の記述で考えた場合, 左側に表記された多倍長変数の方が小さい

/* 多倍長変数 */
class Integer
{
  protected:
    /* 各桁を格納 */
    int data[DIGIT];
    /* 符号(POSITIVE(=1) or NEGATIVE(=-1)) */
    int sign;

  public:
    Integer (void) {
      // 桁数が自然数でない場合
      if (DIGIT < 0) {
            printf("[Error] DIGIT should be bigger than 0.\n");
            exit(-1);
        }
    }

    /*  - 多倍長変数の各桁の任意の桁を取得する - */
    int getOneData(int digit_p) {
      // 桁指定が合致しない場合
      if (digit_p < 0 || digit_p > DIGIT - 1){
        return -1;
      }

      // 正常終了
      return this->data[digit_p];
    }

    /*  - 多倍長変数の各桁の任意の桁に値を設定する - */
    int setOneData(int one_data, int digit_p) {
      // 桁指定が合致しない場合
      if (digit_p < 0 || digit_p > DIGIT - 1) {
        return -1;
      }
      this->data[digit_p] = one_data;

      // 正常終了
      return true;
    }

    /*  - 多倍長変数の各桁の任意の桁に値を足す - */
    int addOneData(int one_data, int digit_p) {
      // 桁指定が合致しない場合
      if (digit_p < 0 || digit_p > DIGIT - 1) {
        return -1;
      }
      this->data[digit_p] += one_data;

      // 正常終了
      return true;
    }

    /*  - 多倍長変数の各桁の任意の桁から値を引く - */
    int subOneData(int one_data, int digit_p) {
      // 桁指定が合致しない場合
      if (digit_p < 0 || digit_p > DIGIT - 1)
        return -1;
      this->data[digit_p] -= one_data;

      // 正常終了
      return true;
    }

    /* - 多倍長変数の符号を取得する - */
    int getSign(void) {
      return this->sign;
    }

    /* - 多倍長変数の符号を設定する - */
    int setSign(int sign) {
      // 符号でないものを指定した場合
      if (sign != POSITIVE && sign != NEGATIVE) {
        return -1;
      }

      this->sign = sign;
      // 正常終了
      return true;
    }

    /* - 多倍長整数の絶対値をとる - */
    void getAbs(Integer* result){
      for (int digit_p = 0; digit_p < DIGIT; digit_p++) {
        result->setOneData(this->getOneData(digit_p), digit_p);
      }
      result->setSign(POSITIVE);
    }

    /* - 多倍長変数をゼロクリアする(符号はコピーしない) - */
    int clearByZero(int bottom, int top){
      // 桁指定が合致しない場合
      if (bottom < 0 || top >= DIGIT) {
        return -1;
      }

      // 各桁を0にする
      for (int digit_p = bottom; digit_p <= top; digit_p++) {
        this->setOneData(0, digit_p);
      }

      // 正常終了
      return true;
    }

    /* - 多倍長変数をゼロクリアする(符号はコピーする) - */
    int clearByZeroAll(int bottom, int top){
        this->setSign(POSITIVE);
        return this->clearByZero(bottom, top);
    }

    /* - 多倍長整数のゼロ判定をする - */
    int isZero(int bottom, int top){
      // 桁指定が合致しない場合
      if (bottom < 0 || top >= DIGIT) {
        return -1;
      }

      // 下の桁から0でない桁を探索する
      for (int digit_p = bottom; digit_p <= top; digit_p++) {
        // ゼロでない場合(正常終了)
        if (this->getOneData(digit_p) != 0) {
          return false;
        }
      }

      // ゼロである場合(正常終了)
      return true;
    }
};

/* 多倍長実数 */
class Float : public Integer
{
  public:
    Float (void) {
      // 桁数が自然数でない場合
      if (DIGIT < 0) {
        printf("[Error] DIGIT should be bigger than 0.\n");
        exit(-1);
      }
      // 小数点の位置が合致しない場合
      if (DECPOS < 0 || DECPOS >= DIGIT) {
        printf("[Error] DECPOS should be bigger than 0 and smaller than DIGIT.\n");
        exit(-2);
      }
    }

    /* - 多倍長変数の小数部の桁数を調べる - */
    int getDecDigit(void) {
      int digit_count = DECPOS;

      for (int digit_p = 0; digit_p < DECPOS; digit_p++, digit_count--) {
        if (this->getOneData(digit_p) != 0) {
          break;
        }
      }

      return digit_count;
    }

    /* - 多倍長実数の整数部の桁数を調べる - */
    int getIntegerDigit() {
      int digit_int = DIGIT - DECPOS;

      for (; digit_int > 0; digit_int--) {
        if (this->getOneData(digit_int + DECPOS - 1) != 0) {
          break;
        }
      }

      return digit_int;
    }

    /* - 多倍長実数の桁数を調べる - */
    int getDigit () {
      return this->getIntegerDigit() + this->getDecDigit();
    }

    /* - 表示用の多倍長実数を文字列<str>に変換する - */
    void getToStr(char* str, int bottom, int top) {
      char str_tmp[3];

      // 符号の文字化
      strcat(str, (this->getSign() == POSITIVE) ? "+" : "-");

      // 各桁の文字化
      for (int digit_p = top; digit_p >= bottom; digit_p--) {
        sprintf(str_tmp, "%d", this->getOneData(digit_p));
        strcat(str, str_tmp);
        if (digit_p == DECPOS && DECPOS != 0) {
          strcat(str, ".");
        }
      }
      strcat(str, "\0");
    }

    /* - 多倍長実数を表示する - */
    void disp(int bottom, int top) {
      char str[DIGIT * 2 + 4] = "";
      // 多倍長実数の文字化
      this->getToStr(str, bottom, top);
      // 多倍長変数の表示
      printf("%s", str);
    }

    /* - 文字列から多倍長実数を設定 - */
    int setByStr(char* str) {
      // 符号の設定
      switch(str[0]) {
        case '+':
          setSign(POSITIVE);
          break;
        case '-':
          setSign(NEGATIVE);
          break;
        // 符号の指定でない場合
        default:
          this->clearByZeroAll(0, DIGIT - 1);
          return -2;
      }

      int dec_through = 0;        // 小数点をとおりすぎたかどうか
      int dec_digit = 0;          // 小数点の位置
      int str_len = strlen(str);  // 文字列の長さ

      // 小数点の位置を調べる
      for (int str_p = str_len - 1; str_p >= 1; str_p--) {
        if (str[str_p] == '.') {
          dec_digit = str_len - str_p - 1;
          break;
        }
      }

      // 小数点の位置の指定ができない場合
      if (dec_digit >= DECPOS) {
        printf("[Warning] data was overflowed under decimal point.\n");
        this->clearByZeroAll(0, DIGIT - 1);
        return -3;
      }

      char ch;      // 文字列から取り出した文字を格納
      int digit_p;  // 多倍長変数内の考えている桁の位置
      int str_p;    // 文字列内の取り出している文字の位置

      // 文字列の分の多倍長変数を格納
      for (digit_p = 0, str_p = str_len - 1; str_p >= 1; digit_p++, str_p--)
      {
        // オーバーフローした場合
        if (digit_p > DIGIT - 1) {
          printf("[Warning] data was overflowed.\n");
          this->clearByZeroAll(0, DIGIT - 1);
          return -1;
        }

        // 文字列から取り出した文字を格納
        ch = str[str_len - digit_p - 1];

        // 小数点を示す文字だった場合
        if (ch == '.') {
          // 小数点が2個以上存在すると分かった場合
          if (dec_through == 1) {
            printf("[Warning] Format is not correct\n");
            this->clearByZeroAll(0, DIGIT - 1);
            return -2;
          }

          dec_through = 1;
          continue;
        }

        // 桁に格納できない文字(0~9以外)だった場合
        if (ch < '0' || ch > '9') {
          printf("[Warning] Format is not correct\n");
          this->clearByZeroAll(0, DIGIT - 1);
          return -2;
        }

        this->setOneData(ch - '0', digit_p - dec_through + DECPOS - dec_digit);
      }

      // まだ格納していない桁に0を格納する
      // 下位桁側
      if (dec_digit < DECPOS) {
        this->clearByZero(0, DECPOS - dec_digit - 1);
      }
      // 上位桁側
      this->clearByZero(digit_p - dec_through + DECPOS - dec_digit, DIGIT - 1);

      // 正常終了
      return true;
    }

    /* 多倍長実数のコピーをする(符号はコピーしなく, コピー前後の示す桁は異なる) */
    int copyData(Float *result, int bottom_or, int top_or, int bottom_rs, int top_rs) {
      // 桁指定が合致しない場合
      if ((bottom_or < 0 || top_or >= DIGIT)||(bottom_rs < 0 || top_rs >= DIGIT)) {
        return -1;
      }
      // 下限の桁の位置よりも上限の桁の位置が小さかった場合
      if (bottom_or > top_or || bottom_rs > top_rs) {
        return -1;
      }
      // 桁の範囲が異なる場合
      if (top_or - bottom_or != top_rs - bottom_rs) {
        return -2;
      }

      // 桁の位置の差
      int dif = bottom_rs - bottom_or;

      // 各桁をコピーする
      for (int digit_p = bottom_or; digit_p <= top_or; digit_p++) {
          result->setOneData(this->getOneData(digit_p), digit_p + dif);
      }

      // 正常終了
      return true;
    }

    /* 多倍長実数のコピーをする(符号はコピーしなく, コピー前後の示す桁は同じ) */
    int copyData(Float *result, int bottom, int top) {
      // 桁指定が合致しない場合
      if (bottom < 0 || top >= DIGIT) {
          return -1;
      }

      // 各桁をコピーする
      for (int digit_p = bottom; digit_p <= top; digit_p++) {
          result->setOneData(this->getOneData(digit_p), digit_p);
      }

      return true;
    }

    /* 多倍長実数のコピーをする(符号もコピー) */
    int copyAll(Float *result, int bottom, int top) {
      // 桁指定が合致しない場合
      if (bottom < 0 || top >= DIGIT) {
        return -1;
      }

      // 各桁をコピーする
      for (int digit_p = bottom; digit_p <= top; digit_p++) {
        result->setOneData(this->getOneData(digit_p), digit_p);
      }

      // 符号をコピーする
      result->setSign(this->getSign());

      return 0;
    }
};

/* プロトタイプ宣言 */
int compData(Integer *, Integer *, int, int);
int compFloat(Float *, Float *, int, int);
int addFloat(Float *, Float *, Float *, int, int);
int subFloat(Float *, Float *, Float *, int, int);
int multipleFloat(Float *, Float *, Float *);
int goldschmidt(Float *, Float *, Float *, int, int);

#endif