#include <stdlib.h>
#include "mruby.h"
#include "mruby/value.h"

static mrb_value
f_gpio_pinmode(mrb_state *mrb, mrb_value self)
{
  char buf[100], *dir_str;
  mrb_int pin, dir;
  mrb_get_args(mrb, "ii", &pin, &dir);

  if( dir==1 ){  // output
    dir_str = "output";
  } else {  // input
    dir_str = "input";
  }
  sprintf(buf, "set %s to port %d", dir_str, pin);
  puts(buf);

  return mrb_nil_value();
}

static mrb_value
f_gpio_dwrite(mrb_state *mrb, mrb_value self)
{
  char buf[100];
  mrb_int pin, value;
  mrb_get_args(mrb, "ii", &pin, &value);

  if( value != 0 ){
    value = 1;
  }

  sprintf(buf, "echo %d >> /sys/class/gpio/gpio%d/value", value, pin);
  system(buf);

  return mrb_nil_value();
}

static mrb_value
f_gpio_dread(mrb_state *mrb, mrb_value self)
{
  int value = 0;    /* dummy value */
  mrb_int pin;
  char buf[100], chval[100];
  FILE *fval;

  mrb_get_args(mrb, "i", &pin);

  sprintf(buf, "cat /sys/class/gpio/gpio%d/value", pin);
  fval=popen(buf, "r");
  fgets(chval,sizeof(chval),fval);
  pclose(fval);

  value = atoi(chval);

  return mrb_fixnum_value(value);
}


static mrb_value
f_sleep(mrb_state *mrb, mrb_value self)
{
  char buf[100];
  mrb_int sec;
  mrb_get_args(mrb, "i", &sec);

  sprintf(buf, "sleep %d", sec);
  system(buf);

  return mrb_nil_value();
}

static mrb_value
f_sensor(mrb_state *mrb, mrb_value self)
{
  char buf[100], chval[100];
  FILE *fval;
  mrb_int sensor;
  double s0, s1, s2, value;
  mrb_get_args(mrb, "i", &sensor);

  sprintf(buf, "sudo python temp.py");
  fval=popen(buf, "r");
  fgets(chval,sizeof(chval),fval);
  pclose(fval);

  sscanf(chval, "%lf%lf%lf", &s0, &s1, &s2);

  if( sensor == 1 ){
    value = s1;
  } else {
    value = s2;
  }

  return mrb_fixnum_value(value);
}



void
mrb_mruby_raspi_shimane_gem_init(mrb_state* mrb)
{
  struct RClass *c;
  c = mrb_define_module(mrb, "Raspi");

  mrb_define_const(mrb, c, "OUTPUT", mrb_fixnum_value(1));
  mrb_define_const(mrb, c, "INPUT", mrb_fixnum_value(0));
  mrb_define_const(mrb, c, "HIGH", mrb_fixnum_value(1));
  mrb_define_const(mrb, c, "LOW", mrb_fixnum_value(0));

  mrb_define_module_function(mrb, c, "pinMode", f_gpio_pinmode, MRB_ARGS_REQ(2));
  mrb_define_module_function(mrb, c, "digitalWrite", f_gpio_dwrite, MRB_ARGS_REQ(2));
  mrb_define_module_function(mrb, c, "digitalRead", f_gpio_dread, MRB_ARGS_REQ(1));

  mrb_define_module_function(mrb, c, "sleep", f_sleep, MRB_ARGS_REQ(1));

  mrb_define_module_function(mrb, c, "sensor", f_sensor, MRB_ARGS_REQ(1));
}

void
mrb_mruby_raspi_shimane_gem_final(mrb_state* mrb)
{
}
