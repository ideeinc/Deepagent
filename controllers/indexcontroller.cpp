#include "indexcontroller.h"


IndexController::IndexController(const IndexController &)
    : ApplicationController()
{ }

void IndexController::index()
{
    render();
}


// Don't remove below this line
T_REGISTER_CONTROLLER(indexcontroller)
