//
// Created by migue on 07/06/2020.
//

#ifndef G070_AGENDA_H
#define G070_AGENDA_H

extern "C"{

#include <cctype>

class Agenda {
public:
  void add_task(void* fn, void* args); //qué quiero hacer?
};

};

#endif //G070_AGENDA_H
