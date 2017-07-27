#include "applicationcontroller.h"
#include "logics/cudatool.h"
#include "logics/managedfilecontext.h"
#include "logics/tagrepository.h"
#include <glog/logging.h>
#include <caffe/caffe.hpp>
#include <TApplicationScheduler>


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

    // Tag Resolution
    class TagResolutionGeneration : public TApplicationScheduler {
    public:
        TagResolutionGeneration() : TApplicationScheduler(nullptr) {}
        void job()
        {
            tInfo() << "TagResolutionGeneration ... started";
            TagRepository().regenerateTagResolution();
            tInfo() << "TagResolutionGeneration ... done";
        }
    };

    bool gen = Tf::conf("settings").value("EnableTagResolutionGeneration").toBool();
    if (gen) {
        static TagResolutionGeneration tr;
        tr.setSingleShot(true);
        tr.start(1);
    }
}

void ApplicationController::staticRelease()
{ }

bool ApplicationController::preFilter()
{
    // キャッシュオフヘッダ
    httpResponse().header().addRawHeader("Cache-Control", "no-store, no-cache, must-revalidate, max-age=0, post-check=0, pre-check=0");
    httpResponse().header().addRawHeader("Pragma", "no-cache");
    return true;
}

// Don't remove below this line
T_DEFINE_CONTROLLER(ApplicationController)
