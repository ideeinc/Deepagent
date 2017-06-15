#include "cudatool.h"
#include <cuda_runtime_api.h>
#include <TGlobal>


QList<int> CudaTool::getGpus()
{
    QList<int> gpus;
    int count = 0;

#ifdef CPU_ONLY
    NO_GPU;
#else
    auto res = cudaGetDeviceCount(&count);
    if (res != cudaSuccess) {
        tError() << cudaGetErrorString(res);
        count = 0;
    }
#endif

    for (int i = 0; i < count; ++i) {
        gpus << i;
    }
    return gpus;
}
