#include "indexcontroller.h"


IndexController::IndexController()
    : ApplicationController()
{ }

void IndexController::index()
{
    render();
}

// Don't remove below this line
T_DEFINE_CONTROLLER(IndexController)
