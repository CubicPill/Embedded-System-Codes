#include "delay.h"
#include "key.h"
#include "lcd.h"
#include "led.h"
#include "remote.h"
#include "sys.h"
#include "usart.h"

#define NAME_MAX_LEN 15
#define REM_NONE 255
#define REM_DEL 10
#define REM_UP 11
#define REM_DOWN 12
#define REM_LEFT 13
#define REM_RIGHT 14
#define REM_PLAY 15
#define REM_POWER 17
void board_init(const u8 board[][9], const u8 *name);
void copy_arr(const u8 src[][9], u8 dst[][9]);
u8 scan_input(u8 *key, u8 *press_key);
u8 validate(u8 numbers[][9]);
u8 KEY_OPT_LEN[10] = {1, 3, 4, 4, 4, 4, 4, 5, 4, 5};
u8 KEY_OPT_CHAR[10][5] = {{'0'},
                          {',', '.', '1'},
                          {'a', 'b', 'c', '2'},
                          {'d', 'e', 'f', '3'},
                          {'g', 'h', 'i', '4'},
                          {'j', 'k', 'l', '5'},
                          {'m', 'n', 'o', '6'},
                          {'p', 'q', 'r', 's', '7'},
                          {'t', 'u', 'v', '8'},
                          {'w', 'x', 'y', 'z', '9'}};

u8 scan_input(u8 *key_ptr, u8 *press_key_ptr) {
   // scan input of key and infrared
   u8 _key = Remote_Scan();
   *key_ptr = REM_NONE;
   *press_key_ptr = KEY_Scan(0);
   if (_key && RmtCnt == 0) {
      switch (_key) {

      case 2:
         *key_ptr = REM_PLAY;
         break;
      case 98:
         // go up
         *key_ptr = REM_UP;
         break;

      case 194:
         // go right
         *key_ptr = REM_RIGHT;
         break;

      case 34:
         // go left
         *key_ptr = REM_LEFT;
         break;

      case 168:
         // go down
         *key_ptr = REM_DOWN;
         break;
      case 66:
         *key_ptr = 0;
         break;
      case 104:
         *key_ptr = 1;
         break;

      case 152:
         *key_ptr = 2;
         break;

      case 176:
         *key_ptr = 3;
         break;

      case 48:
         *key_ptr = 4;
         break;

      case 24:
         *key_ptr = 5;
         break;

      case 122:
         *key_ptr = 6;
         break;

      case 16:
         *key_ptr = 7;
         break;

      case 56:
         *key_ptr = 8;
         break;

      case 90:
         *key_ptr = 9;
         break;

      case 82:
         *key_ptr = REM_DEL;
         break;
      case 162:
         *key_ptr = REM_POWER;
         break;
      }
   }
   printf("REMOTE %d\r\n", *key_ptr);
   printf("PRESS %d\r\n", *press_key_ptr);

   if (*key_ptr != REM_NONE || *press_key_ptr) {
      return 1;
   }
   return 0;
}

int main(void) {
   static const u8 KNOWN_NUM[9][9] = {
      {6, 0, 5, 0, 0, 0, 3, 0, 9}, {0, 8, 0, 9, 0, 1, 0, 2, 0},
      {0, 3, 0, 7, 0, 6, 0, 4, 0}, {1, 0, 4, 0, 0, 0, 7, 0, 5},
      {0, 0, 0, 0, 8, 0, 0, 0, 0}, {3, 0, 8, 0, 0, 0, 2, 0, 4},
      {0, 5, 0, 4, 0, 3, 0, 9, 0}, {0, 6, 0, 1, 0, 8, 0, 5, 0},
      {7, 0, 9, 0, 0, 0, 8, 0, 1}
   };
   // initial numbers in board
   u8 fill_num[9][9];
   u8 YELLOW_AREA[5][4] = {{12, 12, 84, 84},
                           {12, 156, 84, 228},
                           {84, 84, 156, 156},
                           {156, 12, 228, 84},
                           {156, 156, 228, 228}};
   u8 i;
   u8 px = 0, py = 0; // current position
   s8 cx = 0, cy = 0; // position change
   u8 *str = 0;      // String to be displayed
   u8 key = 0;
   u8 last_key = 0;
   u8 press_key = 0;
   u8 s = 0; // KEY0 switch
   u8 sn = 0;
   u8 name_cursor = 0;
   u8 input_cursor = 0;
   u8 name[NAME_MAX_LEN + 1] = {0};
   copy_arr(KNOWN_NUM, fill_num);
   delay_init();
   KEY_Init();
   NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
   uart_init(9600);
   LED_Init();
   LCD_Init();
   Remote_Init();

   LCD_ShowString(30, 120, 140, 16, 16, "Input your name:");
   LCD_ShowChar(20, 140, '>', 16, 1);
   LCD_Fill(30, 160, 240, 180, WHITE);
   LCD_ShowChar(30 + 8 * name_cursor, 160, '^', 16, 1);
   while (1) { // input name
      scan_input(&key, &press_key);
      if (key <= 9) {
         // input letters
         if (key == last_key) {

            input_cursor = (input_cursor + 1) % KEY_OPT_LEN[key];
            // letters in the same key
         } else {
            input_cursor = 0;
         }
         name[name_cursor] = KEY_OPT_CHAR[key][input_cursor];
         LCD_Fill(30 + 8 * name_cursor, 140, 38 + 8 * name_cursor, 156, WHITE);
         LCD_ShowChar(30 + 8 * name_cursor, 140, KEY_OPT_CHAR[key][input_cursor],
                      16, 1);

      } else if (key == REM_RIGHT) {
         // input next letter
         if (name_cursor < NAME_MAX_LEN) {
            ++name_cursor;
         }
         input_cursor = 0;
         LCD_Fill(30, 160, 240, 180, WHITE);
         LCD_ShowChar(30 + 8 * name_cursor, 160, '^', 16, 1);

      } else if (key == REM_LEFT) {
         // go back
         if (name_cursor > 0) {
            --name_cursor;
         }
         input_cursor = 0;
         LCD_Fill(30, 160, 240, 180, WHITE);
         LCD_ShowChar(30 + 8 * name_cursor, 160, '^', 16, 1);
      } else if (key == REM_DEL) {
         // delete current
         input_cursor = 0;
         LCD_Fill(30 + 8 * name_cursor, 140, 38 + 8 * name_cursor, 156, WHITE);
         name[name_cursor] = ' ';
      } else if (key == REM_PLAY) {
         // confirm input
         name[name_cursor + 1] = '\0';
         break;
      }
      if (key != REM_NONE) {
         last_key = key;
      }
      delay_ms(300);
   }

   board_init(KNOWN_NUM, name);
   str = "Ready";
   while (1) {
      scan_input(&key, &press_key);

      if (key != REM_NONE) {
         if (key == REM_DEL) {
            // delete current
            if (KNOWN_NUM[py][px] == 0) {
               LCD_Fill(13 + px * 24, 13 + py * 24, 35 + px * 24, 35 + py * 24,
                        CYAN);
               // clear current cursor location
               fill_num[py][px] = 0;
               str = "Deleted";
            } else {
               str = "Can't delete";
            }
         } else if (key > 0 && key <= 9) {
            // input number
            if (KNOWN_NUM[py][px] == 0) {
               LCD_Fill(13 + px * 24, 13 + py * 24, 35 + px * 24, 35 + py * 24,
                        CYAN);
               LCD_ShowChar(20 + 24 * px, 16 + 24 * py, key + 48, 16, 1);
               fill_num[py][px] = key;
               str = "( , ) => ";
               sn = 1;

            } else {
               // given numbers can't be set
               str = "Can't set";
            }
         } else if (key == REM_POWER) {
            // power button,validate result
            if (validate(fill_num)) {
               str = "You win!";
            } else {
               str = "Incorrect!";
            }
         } else {
            // move cursor location
            switch (key) {
            case REM_UP:
               cy = -1;
               break;
            case REM_DOWN:
               cy = 1;
               break;
            case REM_RIGHT:
               cx = 1;
               break;
            case REM_LEFT:
               cx = -1;
               break;
            default:
               cy = 0;
               cx = 0;
               break;
            }
            if (!s && ((py + cy > 8) || (py + cy) < 0 || (px + cx > 8) ||
                       (px + cx < 0))) {
               // on border and switch is off
               str = "Can't move";
            } else if (cy != 0 || cx != 0) {
               LCD_Fill(13 + px * 24, 13 + py * 24, 35 + px * 24, 35 + py * 24,
                        WHITE);
               for (i = 0; i < 5; ++i) {
                  if (((px + 1) * 24 > YELLOW_AREA[i][0]) &&
                      ((px + 1) * 24 < YELLOW_AREA[i][2]) &&
                      ((py + 1) * 24 > YELLOW_AREA[i][1]) &&
                      ((py + 1) * 24 < YELLOW_AREA[i][3])) {
                     // if inside yellow area, restore color to yellow
                     LCD_Fill(13 + px * 24, 13 + py * 24, 35 + px * 24, 35 + py * 24,
                              YELLOW);
                     break;
                  }
               }
               if (fill_num[py][px] > 0) {
                  if (KNOWN_NUM[py][px] > 0) {
                     POINT_COLOR = BLACK;
                  }
                  LCD_ShowChar(20 + 24 * px, 16 + 24 * py, fill_num[py][px] + 48, 16,
                               1);
               }
               POINT_COLOR = RED;
               px = (px + cx + 9) % 9;
               py = (py + cy + 9) % 9;

               LCD_Fill(13 + px * 24, 13 + py * 24, 35 + px * 24, 35 + py * 24,
                        CYAN);
               if (fill_num[py][px] > 0) {
                  if (KNOWN_NUM[py][px] > 0) {
                     POINT_COLOR = BLACK;
                  }
                  LCD_ShowChar(20 + 24 * px, 16 + 24 * py, fill_num[py][px] + 48, 16,
                               1);
               }

               str = "Move to ( , )";
               sn = 2;
            }
         }
      }

      if (press_key) {
         if (press_key == WKUP_PRES) {
            // clear board
            board_init(KNOWN_NUM, name);
            copy_arr(KNOWN_NUM, fill_num);
            px = 0;
            py = 0;
            str = "Game reseted";
         } else if (press_key == KEY1_PRES) {
            s = !s;
            if (s) {
               str = "Switch is on";
            } else {
               str = "Switch is off";
            }
         }
      }
      delay_ms(300);
      if (str) {
         // print message to screen
         POINT_COLOR = RED;
         LCD_Fill(60, 260, 240, 280, WHITE);
         LCD_ShowString(30, 260, 210, 16, 16, str);
         if (sn == 1) {
            LCD_ShowNum(38, 260, px + 1, 1, 16);
            LCD_ShowNum(54, 260, py + 1, 1, 16);
            LCD_ShowNum(102, 260, key, 1, 16);
         } else if (sn == 2) {

            LCD_ShowNum(102, 260, px + 1, 1, 16);
            LCD_ShowNum(118, 260, py + 1, 1, 16);
         }
      }
      str = 0;
      sn = 0;
      cy = 0;
      cx = 0;
   }
}

void board_init(const u8 KNOWN_NUM[][9], const u8 *name) {
   // draw empty board
   u8 i, j;
   POINT_COLOR = BLACK;
   LCD_Clear(WHITE);
   LCD_Fill(12, 12, 84, 84, YELLOW);
   LCD_Fill(12, 156, 84, 228, YELLOW);
   LCD_Fill(84, 84, 156, 156, YELLOW);
   LCD_Fill(156, 12, 228, 84, YELLOW);
   LCD_Fill(156, 156, 228, 228, YELLOW);
   LCD_Fill(13, 13, 35, 35, CYAN);
   LCD_DrawLine(12, 12, 228, 12);
   LCD_DrawLine(12, 36, 228, 36);
   LCD_DrawLine(12, 60, 228, 60);
   LCD_DrawLine(12, 84, 228, 84);
   LCD_DrawLine(12, 108, 228, 108);
   LCD_DrawLine(12, 132, 228, 132);
   LCD_DrawLine(12, 156, 228, 156);
   LCD_DrawLine(12, 180, 228, 180);
   LCD_DrawLine(12, 204, 228, 204);
   LCD_DrawLine(12, 228, 228, 228);
   LCD_DrawLine(12, 12, 12, 228);
   LCD_DrawLine(36, 12, 36, 228);
   LCD_DrawLine(60, 12, 60, 228);
   LCD_DrawLine(84, 12, 84, 228);
   LCD_DrawLine(108, 12, 108, 228);
   LCD_DrawLine(132, 12, 132, 228);
   LCD_DrawLine(156, 12, 156, 228);
   LCD_DrawLine(180, 12, 180, 228);
   LCD_DrawLine(204, 12, 204, 228);
   LCD_DrawLine(228, 12, 228, 228);
   LCD_DrawLine(0, 255, 240, 255);
// fill given numbers
   for (i = 0; i < 9; ++i) {
      for (j = 0; j < 9; ++j) {
         int curr = KNOWN_NUM[j][i];
         if (curr > 0) {
            LCD_ShowChar(20 + 24 * i, 16 + 24 * j, curr + 48, 16, 1);
         }
      }
   }
   POINT_COLOR = RED;
   LCD_ShowString(20, 234, 100, 16, 16, "Sudoku Game");
   LCD_ShowString(120, 234, 120, 16, 16, name);
   LCD_ShowString(20, 260, 10, 16, 16, ">");
}

void copy_arr(const u8 src[][9], u8 dst[][9]) {
   u8 i, j;
   for (i = 0; i < 9; ++i) {
      for (j = 0; j < 9; ++j) {
         dst[i][j] = src[i][j];
      }
   }
}

u8 validate(u8 numbers[][9]) {
   // validate the game input
   u8 i, j;
   u8 v_temp[27][9] = {0};
   // matrix for verification
   // 0-8 rows
   // 9-17 columns
   // 18-26 blocks
   u8 curr;

   for (i = 0; i < 9; ++i) {
      for (j = 0; j < 9; ++j) {

         if (numbers[i][j] == 0) {
            return 0;
         }
         curr = numbers[i][j];
         v_temp[j][curr - 1] += 1;
         v_temp[i + 9][curr - 1] += 1;
         v_temp[i / 3 + 3 * (j / 3) + 18][curr - 1] += 1;
      }
   }

   for (i = 0; i < 9; ++i) {
      for (j = 0; j < 27; ++j) {
         if (v_temp[j][i] == 0) {
            return 0;
         }
      }
   }

   return 1;
}
