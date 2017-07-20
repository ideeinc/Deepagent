#include "applicationcontroller.h"
#include "logics/cudatool.h"
#include "logics/managedfilecontext.h"
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

    // caffe
#ifdef CPU_ONLY
    caffe::Caffe::set_mode(caffe::Caffe::CPU);
#else
    // Set device id and mode
    auto gpus = CudaTool::getGpus();
    if (gpus.isEmpty()) {
        caffe::Caffe::set_mode(caffe::Caffe::CPU);
    } else {
        tInfo() << "Use GPU with device ID " << gpus[0];
        cudaDeviceProp deviceProp;
        cudaGetDeviceProperties(&deviceProp, gpus[0]);
        tInfo() << "GPU device name: " << deviceProp.name;
        caffe::Caffe::SetDevice(gpus[0]);
        caffe::Caffe::set_mode(caffe::Caffe::GPU);
    }
#endif

    // load all file's hashes.
    ManagedFileContext::load();
}

void ApplicationController::staticRelease()
{ }

bool ApplicationController::preFilter()
{
    return true;
}

// Don't remove below this line
T_DEFINE_CONTROLLER(ApplicationController)
