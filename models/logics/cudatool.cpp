#include "cudatool.h"
#ifndef CPU_ONLY
#include <cuda_runtime_api.h>
#endif
#include <TGlobal>


QList<int> CudaTool::getGpus()
{
    QList<int> gpus;
    int count = 0;

#ifndef CPU_ONLY
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
