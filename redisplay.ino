#include <ArduinoJson.h>
#include <U8glib.h>

#include "redis_logo.h"

// Configuration section --------------------
U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_DEV_0 | U8G_I2C_OPT_NO_ACK | U8G_I2C_OPT_FAST);

#define SERIAL_SPEED 9600

#define INTERVAL_PANEL 8000
#define LOGO_ANIM_DELAY 800

#define SYMBOL_SPACING 6
#define CHARCODE_BULLET 210
// -----------------------------------------

#define FONT_NORMAL 0
#define FONT_SYMBOL 1
#define NEWLINE '\n'

#define MAX_GRAPH_VALUES 40

#define PANEL_LOGO 0
#define PANEL_BASIC 1
#define PANEL_ADVANCED 2
#define PANEL_OPS_SEC 3

int k_width_anim = 0;
byte k_state = PANEL_LOGO;
long prev_millis = 0;
bool dirty = true;

char command[512] = {0};
char *command_it = command;
  
struct basic_stats_t {
  char clients[8];
  char memory[8];
  char ops_per_sec[8];
} basic_stats;

struct advanced_stats_t {
  char rejected_conn[8];
  char key_hits[8];
  char key_miss[8];
} advanced_stats;

struct ops_sec_stats_t {
  int graph_values[MAX_GRAPH_VALUES];
  char max_value[8];
} ops_sec_stats;
  
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

int draw_panel_header(const char *title) {
  int y = -1;
  int str_width = 0;
  
  set_font(FONT_SYMBOL);
  
  char bullet[2] = {CHARCODE_BULLET, '\0'};
  str_width = u8g.drawStr(0, y, bullet);
  str_width += SYMBOL_SPACING;

  set_font(FONT_NORMAL);

  u8g.drawStr(str_width, y, title);
  y += u8g.getFontLineSpacing() + 2;
  u8g.drawHLine(0, y, 128);
  y += 5;
  
  return y;
}

void draw_basic_stats(void) {
  int y = draw_panel_header("Basic Stats");
  draw_new_line(&y, "Clients", basic_stats.clients, false);
  draw_new_line(&y, "Memory", basic_stats.memory);
  draw_new_line(&y, "Ops/sec", basic_stats.ops_per_sec);
}

void draw_adv_stats(void) {
  int y = draw_panel_header("Advanced Stats");

  draw_new_line(&y, "Rej Conn", advanced_stats.rejected_conn, false);
  draw_new_line(&y, "Key Hits", advanced_stats.key_hits);
  draw_new_line(&y, "Key Miss", advanced_stats.key_miss);
}

void draw_anim_logo() {
  u8g.drawXBMP(0, 2, REDISLOGO_WIDTH, REDISLOGO_HEIGHT, redislogo_bits);
  u8g.drawHLine(0, 50, k_width_anim);
  u8g.drawStr(0, 52, "Server Monitor v.1.0");
}

void draw_ops_sec_stats(void) {
  const int xoffset = u8g.getWidth() - MAX_GRAPH_VALUES - 4;
  int y = draw_panel_header("Operations/sec.");

  const int max_value_w = u8g.getStrWidth(ops_sec_stats.max_value) + 6;
  u8g.drawStr(xoffset-max_value_w, y, ops_sec_stats.max_value);
  
  u8g.drawStr(0, 40, "Last 40");
  u8g.drawStr(0, 40+u8g.getFontLineSpacing(), "seconds");

  y = y + (u8g.getHeight() - y - 1);
  
  for(int value_idx=0; value_idx<MAX_GRAPH_VALUES; value_idx++) {
    const int value = ops_sec_stats.graph_values[value_idx];
    u8g.drawLine(value_idx+xoffset, y, value_idx+xoffset, y-value);
  }
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
    case PANEL_OPS_SEC:
      draw_ops_sec_stats();
      break;
    default:
      break;    
  }
}

bool interpret_command()
{
  StaticJsonBuffer<512> json_buffer;
  JsonObject& json = json_buffer.parseObject(command);
  
  if(!json.success()) {
    return false;
  }

  const char *cmd_op = json["cmd"];
  
  if(!strcmp(cmd_op, "panel_advanced")) {
    k_state = PANEL_ADVANCED;
    prev_millis = millis();
    return true;
  }

  if(!strcmp(cmd_op, "update_basic")) {
    strcpy(basic_stats.clients, json["clients"]);
    strcpy(basic_stats.memory, json["memory"]);
    strcpy(basic_stats.ops_per_sec, json["ops/s"]);
    return true;
  }

  if(!strcmp(cmd_op, "update_advanced")) {
    strcpy(advanced_stats.rejected_conn, json["rej_conn"]);
    strcpy(advanced_stats.key_hits, json["key_hits"]);
    strcpy(advanced_stats.key_miss, json["key_miss"]);
    return true;
  }

  if(!strcmp(cmd_op, "update_ops_sec")) {
    JsonArray &graph_values = json["graph_values"];

    int values_size = graph_values.size();
    if(values_size > MAX_GRAPH_VALUES)
      return false;
    
    strcpy(ops_sec_stats.max_value, json["max_value"]);
    
    for(int i=0; i<values_size; i++) {
      const int value = graph_values[i].as<int>();
      ops_sec_stats.graph_values[i] = value;
    }
    return true;
  }
  
  return false;
}

void initialize_stats(void)
{
  strcpy(basic_stats.clients, "0");
  strcpy(basic_stats.memory, "0");
  strcpy(basic_stats.ops_per_sec, "0");
  
  strcpy(advanced_stats.rejected_conn, "0");
  strcpy(advanced_stats.key_hits, "0");
  strcpy(advanced_stats.key_miss, "0");
  
  strcpy(ops_sec_stats.max_value, "0");
  for(int i=0; i<=MAX_GRAPH_VALUES; i++) {
    ops_sec_stats.graph_values[i] = 0;
  }
}

void change_panels(void) {
  // Change panels based on interval
  unsigned long curr_millis = millis();
  if(curr_millis - prev_millis >= INTERVAL_PANEL) {
    prev_millis = curr_millis;
    
    switch(k_state) {
      case PANEL_BASIC:
        k_state = PANEL_ADVANCED;
        break;
      case PANEL_ADVANCED:
        k_state = PANEL_OPS_SEC;
        break;
      case PANEL_OPS_SEC:
        k_state = PANEL_BASIC;
        break;
      default:
        break;    
    }
    
    dirty = true;
  }
}

void anim_logo(void) {
  // Handle initial logo and animation
  if(k_state==PANEL_LOGO) {
      dirty = true;
      k_width_anim += 8;
      if(k_width_anim >= 128) {
        k_state = PANEL_BASIC;
        delay(LOGO_ANIM_DELAY);
      }
  }
}

bool read_command(void)
{
  while(Serial.available()>0) {
    const char ch = Serial.read();
    if(ch != NEWLINE) {
      *command_it = ch;
      command_it++;
    } else {
      *command_it='\0';
      command_it = command;
      return true;
    }
  }
  return false;
}
 
void check_commands(void)
{
  if(read_command()) {
    bool ret = interpret_command();
    // If the command changed any state, we need redraw
    if(ret)
      dirty = true;
  }
}
  
void setup(void) {
  Serial.begin(SERIAL_SPEED);
  u8g.setColorIndex(1);
  set_font(FONT_NORMAL);
  initialize_stats();
  dirty = true;
}

void loop(void) {
  
  // U8g drawing loop (when dirty only to save cycles)
  if(dirty) {
    u8g.firstPage();  
    do {
      draw();
    } while(u8g.nextPage());
    dirty = false;
  }

  anim_logo();
  change_panels();

  // Check if a command was received
  check_commands();

}

