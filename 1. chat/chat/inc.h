#pragma once
#include "Client.h"
#include "Server.h"

void print_error(string text, bool critical);
void set_font_color(size_t c);
void create_thread(LPTHREAD_START_ROUTINE);
size_t get_font_number(string text);