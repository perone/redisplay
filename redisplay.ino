
#include "U8glib.h"
#include "redis_logo.h"

U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_DEV_0 | U8G_I2C_OPT_NO_ACK | U8G_I2C_OPT_FAST);

#define PANEL_LOGO 0
#define PANEL_BASIC 1
#define PANEL_ADVANCED 2

#define SYMBOL_SPACING 6
#define CHARCODE_BULLET 210

#define FONT_NORMAL 0
#define FONT_SYMBOL 1

int k_width_anim = 0;
byte k_state = PANEL_LOGO;

void set_font(byte font_type)
{
  switch(font_type)
  {
    case FONT_NORMAL:
      u8g.setFont(u8g_font_7x13);
      break;
    case FONT_SYMBOL:
      u8g.setFont(u8g_font_7x13_67_75);
      break;
    default:
      u8g.setFont(u8g_font_7x13);
      break;
  }
  u8g.setFontPosTop();
}

void draw_new_line(int *y, const char *key, const char *value, bool new_line=true)
{
  int str_width;
  if(new_line)
    *y += u8g.getFontLineSpacing() + 2;
  u8g.drawStr(0, *y, key);
  str_width = u8g.getStrWidth(value);
  u8g.drawStr(128-str_width, *y, value);
}

void draw_basic_stats(void) {
  int y = -1;
  int str_width = 0;
  
  set_font(FONT_SYMBOL);
  
  char bullet[2] = {CHARCODE_BULLET, '\0'};
  str_width = u8g.drawStr(0, y, bullet);
  str_width += SYMBOL_SPACING;

  set_font(FONT_NORMAL);

  u8g.drawStr(str_width, y, "Basic Stats");
  y += u8g.getFontLineSpacing() + 2;
  u8g.drawHLine(0, y, 128);
  y += 5;
  
  draw_new_line(&y, "Clients", "14", false);
  draw_new_line(&y, "Used Memory", "80.6M");
  draw_new_line(&y, "Ops per sec", "2104");
}

void draw_adv_stats(void) {
  u8g.drawStr(0, -1, "Advanced Stats");
  u8g.drawFrame(10, 14, 20, 80);
}

void draw_anim_logo() {
  u8g.drawXBMP(0, 2, REDISLOGO_WIDTH, REDISLOGO_HEIGHT, redislogo_bits);
  u8g.drawHLine(0, 50, k_width_anim);
  u8g.drawStr(0, 52, "Server Monitor v.1.0");
}

void draw(void) {
  switch(k_state) {
    case PANEL_LOGO:
      draw_anim_logo();
      break;
    case PANEL_BASIC:
      draw_basic_stats();
      break;
    case PANEL_ADVANCED:
      draw_adv_stats();
      break;
    default:
      break;    
  }
}

void setup(void) {
  u8g.setColorIndex(1);
  set_font(FONT_NORMAL);
}

void loop(void) {
  u8g.firstPage();  
  do {
    draw();
  } while( u8g.nextPage() );

  switch(k_state) {
    case PANEL_LOGO:
      k_width_anim += 8;
      if(k_width_anim >= 128) {
        k_state = PANEL_BASIC;
        delay(1000);
      }
      break;
    case PANEL_BASIC:
      k_state = PANEL_ADVANCED;
      delay(8000);
      break;
    case PANEL_ADVANCED:
      k_state = PANEL_BASIC;
      delay(100);
      break;
    default:
      break;    
  }
  
}

