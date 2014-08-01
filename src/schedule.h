#ifndef SCHEDULE_H
#define SCHEDULE_H

struct program { time_t start; time_t end; char *title; };

extern struct program programs[];

#endif
