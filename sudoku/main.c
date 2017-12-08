#include "led.h"
#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "lcd.h"
#include "remote.h"
#include "key.h"

#define NAME_MAX_LEN 15;
#define REM_ZERO 255
#define REM_DEL 10
#define REM_UP 11
#define REM_DOWN 12
#define REM_LEFT 13
#define REM_RIGHT 14
void board_init(const u8 board[][9]);
void copy_arr(const u8 src[][9],u8 dst[][9]);
u8 scan_input(u8 *key,u8*press_key);
u8 validate(u8 numbers[][9]);
u8 KEY_OPT_LEN[10]={3,4,4,4,4,4,5,4,5,2};
u8 KEY_OPT_CHAR[10][5]={
   {',','.','1'},
   {'a','b','c','2'},
   {'d','e','f','3'},
   {'g','h','i','4'},
   {'j','k','l','5'},
   {'m','n','o','6'},
   {'p','q','r','s','7'},
   {'t','u','v','8'},
   {'w','x','y','z','9'},
   {' ','0'}
};

u8 scan_input(u8 *key,u8*press_key){
   u8 _key=Remote_Scan();
   *key=0;
   *press_key=KEY_Scan(0);
   if(_key && RmtCnt==0) {
      switch(_key) {
      case 98:
         // go up
         *key=REM_UP;
         break;

      case 194:
         //go right
         *key=REM_RIGHT;
         break;

      case 34:
         //go left
         *key=REM_LEFT;
         break;

      case 168:
         //go down
         *key=REM_DOWN;
         break;
      case 66:
         *key=REM_ZERO;
         break;
      case 104:
         *key=1;
         break;

      case 152:
         *key=2;
         break;

      case 176:
         *key=3;
         break;

      case 48:
         *key=4;
         break;

      case 24:
         *key=5;
         break;

      case 122:
         *key=6;
         break;

      case 16:
         *key=7;
         break;

      case 56:
         *key=8;
         break;

      case 90:
         *key=9;
         break;


      case 82:
         *key=REM_DEL;
         break;

      }

   }


   if (*key || *press_key) {
      return 1;
   } return 0;

}



int main(void) {
   static const u8 KNOWN_NUM[9][9]={
      {6,0,5,0,0,0,3,0,9},
      {0,8,0,9,0,1,0,2,0},
      {0,3,0,7,0,6,0,4,0},
      {1,0,4,0,0,0,7,0,5},
      {0,0,0,0,8,0,0,0,0},
      {3,0,8,0,0,0,2,0,4},
      {0,5,0,4,0,3,0,9,0},
      {0,6,0,1,0,8,0,5,0},
      {7,0,9,0,0,0,8,0,1}
   };
   u8 fill_num[9][9];
   u8 YELLOW_AREA[5][4]={
      {12,12,84,84},
      {12,156,84,228},
      {84,84,156,156},
      {156,12,228,84},
      {156,156,228,228}
   };
   u8 i;
   u8 px=0,py=0; // current position
   s8 cx=0,cy=0; // position change
   u8* str=0; // String to be displayed
   u8 key=0;
   u8 press_key=0;
   u8 input=0; // input value
   u8 s=0; //KEY0 switch
   u8 sn=0;
   u8 name_cursor=0;
   u8 input_cursor=0;
   copy_arr(KNOWN_NUM,fill_num);
   delay_init();
   KEY_Init();
   NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
   uart_init(9600);
   LED_Init();
   LCD_Init();
   Remote_Init();


   LCD_ShowString(30,120,140,16,16,"Input your name:");
   LCD_ShowChar(24,140,'>',16,1);




   board_init(KNOWN_NUM);
   str="Ready";
   while(1) {
      key=Remote_Scan();
      press_key=KEY_Scan(0);
      if(key && RmtCnt==0) {
         switch(key) {
         case 98:
            // go up
            cy=-1;
            break;

         case 194:
            //go right
            cx=1;
            break;

         case 34:
            //go left
            cx=-1;
            break;

         case 168:
            //go down
            cy=1;
            break;
         case 66:
            //0
            break;
         case 104:
            input=1;
            break;

         case 152:
            input=2;
            break;

         case 176:
            input=3;
            break;

         case 48:
            input=4;
            break;

         case 24:
            input=5;
            break;

         case 122:
            input=6;
            break;

         case 16:
            input=7;
            break;

         case 56:
            input=8;
            break;

         case 90:
            input=9;
            break;


         case 82:
            // clear number in current block
            input=10;

            break;

         }
         if (input) {
            if (input==10) {
               //delete current
               if (KNOWN_NUM[py][px]==0) {
                  LCD_Fill(13+px*24,13+py*24,35+px*24,35+py*24,CYAN);
                  fill_num[py][px]=0;
                  str="Deleted";
               }
            } else{
               if (KNOWN_NUM[py][px]==0) {
                  LCD_Fill(13+px*24,13+py*24,35+px*24,35+py*24,CYAN);
                  LCD_ShowChar(20+24*px,16+24*py,input+48,16,1);
                  fill_num[py][px]=input;
                  str="( , ) => ";
                  sn=1;

               } else{

                  str="Can't set";
               }
            }
         } else if (key==162) {
            // power button,validate result
            if (validate(fill_num)) {
               str="You win!";
            }else{
               str="Incorrect!";
            }
         } else {
            if(!s&&((py+cy>8)||(py+cy)<0||(px+cx>8)||(px+cx<0))) {
               // on border and switch is off
               str="Can't move";
            }else{
               LCD_Fill(13+px*24,13+py*24,35+px*24,35+py*24,WHITE);
               for(i=0; i<5; ++i) {
                  if(((px+1)*24>YELLOW_AREA[i][0]) && ((px+1)*24<YELLOW_AREA[i][2]) && ((py+1)*24>YELLOW_AREA[i][1]) && ((py+1)*24<YELLOW_AREA[i][3])) {
                     LCD_Fill(13+px*24,13+py*24,35+px*24,35+py*24,YELLOW);
                     break;
                  }
               }
               if (fill_num[py][px]>0) {
                  if (KNOWN_NUM[py][px]>0) {
                     POINT_COLOR=BLACK;
                  }
                  LCD_ShowChar(20+24*px,16+24*py,fill_num[py][px]+48,16,1);
               }
               POINT_COLOR=RED;
               px=(px+cx+9)%9;
               py=(py+cy+9)%9;

               LCD_Fill(13+px*24,13+py*24,35+px*24,35+py*24,CYAN);
               if (fill_num[py][px]>0) {
                  if (KNOWN_NUM[py][px]>0) {
                     POINT_COLOR=BLACK;
                  }
                  LCD_ShowChar(20+24*px,16+24*py,fill_num[py][px]+48,16,1);
               }

               str="Move to ( , )";
               sn=2;

            }

         }

      }
      if(press_key) {
         if (press_key==WKUP_PRES) {
            board_init(KNOWN_NUM);
            copy_arr(KNOWN_NUM,fill_num);
            px=0;
            py=0;
            str="Game reseted";
         }else if (press_key==KEY1_PRES) {
            s=!s;
            if(s) {
               str="Switch is on";
            }else{
               str="Switch is off";
            }
         }
      }
      delay_ms(300);
      if (str) {
         POINT_COLOR=RED;
         LCD_Fill(60,260,240,280,WHITE);
         LCD_ShowString(30,260,210,16,16,str);
         if (sn==1) {
            LCD_ShowNum(38,260,px+1,1,16);
            LCD_ShowNum(54,260,py+1,1,16);
            LCD_ShowNum(102,260,input,1,16);
         }else if (sn==2) {

            LCD_ShowNum(102,260,px+1,1,16);
            LCD_ShowNum(118,260,py+1,1,16);
         }

      }
      str=0;
      sn=0;
      input=0;
      cy=0;
      cx=0;

   }
}

void board_init(const u8 KNOWN_NUM[][9]){
   u8 i,j;
   POINT_COLOR=BLACK;
   LCD_Clear(WHITE);
   LCD_Fill(12,12,84,84,YELLOW);
   LCD_Fill(12,156,84,228,YELLOW);
   LCD_Fill(84,84,156,156,YELLOW);
   LCD_Fill(156,12,228,84,YELLOW);
   LCD_Fill(156,156,228,228,YELLOW);
   LCD_Fill(13,13,35,35,CYAN);
   LCD_DrawLine(12,12,228,12);
   LCD_DrawLine(12,36,228,36);
   LCD_DrawLine(12,60,228,60);
   LCD_DrawLine(12,84,228,84);
   LCD_DrawLine(12,108,228,108);
   LCD_DrawLine(12,132,228,132);
   LCD_DrawLine(12,156,228,156);
   LCD_DrawLine(12,180,228,180);
   LCD_DrawLine(12,204,228,204);
   LCD_DrawLine(12,228,228,228);
   LCD_DrawLine(12,12,12,228);
   LCD_DrawLine(36,12,36,228);
   LCD_DrawLine(60,12,60,228);
   LCD_DrawLine(84,12,84,228);
   LCD_DrawLine(108,12,108,228);
   LCD_DrawLine(132,12,132,228);
   LCD_DrawLine(156,12,156,228);
   LCD_DrawLine(180,12,180,228);
   LCD_DrawLine(204,12,204,228);
   LCD_DrawLine(228,12,228,228);
   LCD_DrawLine(0,255,240,255);

   for (i=0; i<9; ++i) {
      for (j=0; j<9; ++j) {
         int curr=KNOWN_NUM[j][i];
         if (curr>0) {
            LCD_ShowChar(20+24*i,16+24*j,curr+48,16,1);
         }
      }
   }
   POINT_COLOR=RED;
   LCD_ShowString(75,234,100,16,16,"Sudoku Game");
   LCD_ShowString(20,260,10,16,16,">");

}

void copy_arr(const u8 src[][9],u8 dst[][9]){
   u8 i,j;
   for(i=0; i<9; ++i) {
      for(j=0; j<9; ++j) {
         dst[i][j]=src[i][j];
      }
   }
}
u8 validate(u8 numbers[][9]){
   u8 i,j;
   u8 v_temp[27][9]={0};
   // matrix for verification
   // 0-8 rows
   // 9-17 columns
   // 18-26 blocks
   u8 curr;


   for(i=0; i<9; ++i) {
      for(j=0; j<9; ++j) {

         if (numbers[i][j]==0) {
            return 0;
         }
         curr=numbers[i][j];
         v_temp[j][curr-1]+=1;
         v_temp[i+9][curr-1]+=1;
         v_temp[i/3+3*(j/3)+18][curr-1]+=1;


      }
   }

   for(i=0; i<9; ++i) {
      for(j=0; j<27; ++j) {
         if (v_temp[j][i]==0) {
            return 0;
         }
      }
   }

   return 1;
}