#include "applicationcontroller.h"
#include <glog/logging.h>
#include <caffe/caffe.hpp>


ApplicationController::ApplicationController()
    : TActionController()
{ }

ApplicationController::~ApplicationController()
{ }

void ApplicationController::staticInitialize()
{
    // glog
    google::InitGoogleLogging("caffe");
    google::InstallFailureWriter(nullptr);
    google::InstallFailureSignalHandler();
    google::SetLogDestination(google::INFO, qPrintable(Tf::app()->logPath() + "caffe-"));
    FLAGS_logtostderr = 0;

#ifdef CPU_ONLY
    caffe::Caffe::set_mode(caffe::Caffe::CPU);
#else
    caffe::Caffe::set_mode(caffe::Caffe::GPU);
#endif
}

void ApplicationController::staticRelease()
{ }

bool ApplicationController::preFilter()
{
    return true;
}

// Don't remove below this line
T_DEFINE_CONTROLLER(ApplicationController)
