﻿#pragma once

namespace vuda
{
    // Wait for compute device to finish.
    /*__host__ ​ __device__*/
    /*inline error_t deviceSynchronize(void)
    {
        std::thread::id tid = std::this_thread::get_id();
        const thread_info tinfo = interface_thread_info::GetThreadInfo(tid);
        tinfo.GetLogicalDevice()->WaitOn();
        return vudaSuccess;
    }*/

    // Returns which device is currently being used.
    /*__host__ ​ __device__*/
    inline error_t getDevice(int* device)
    {
        //
        // get device assigned to thread
        const detail::thread_info tinfo = detail::interface_thread_info::GetThreadInfo(std::this_thread::get_id());
        *device = tinfo.GetDeviceID();
        return vudaSuccess;
    }

    /* __host__ ​ __device__ */
    inline error_t getDeviceCount(int* count)
    {
        vk::UniqueInstance& inst = detail::Instance::get();
        *count = (int)inst->enumeratePhysicalDevices().size();
        return vudaSuccess;
    }

    /*__host__*/
    inline error_t getDeviceProperties(deviceProp* prop, int device)
    {
        vk::PhysicalDevice physDevice = detail::vudaGetPhysicalDevice(device);

        vk::PhysicalDeviceProperties deviceProperties;
        physDevice.getProperties(&deviceProperties);
                
        for(uint32_t i = 0; i<256; ++i)
            prop->name[i] = deviceProperties.deviceName[i];
         
        //prop->totalGlobalMem = deviceProperties.limits.;
        //prop->sharedMemPerBlock = 0;

        for(uint32_t i = 0; i<3; ++i)
        { 
            prop->maxGridSize[i] = deviceProperties.limits.maxComputeWorkGroupCount[i];
            prop->maxThreadsDim[i] = deviceProperties.limits.maxComputeWorkGroupSize[i];
        }
        prop->maxThreadsPerBlock = deviceProperties.limits.maxComputeWorkGroupInvocations;
        
        return vudaSuccess;
    }

    // __host__
    inline error_t setDevice(int device)
    {
        vk::PhysicalDevice physDevice = detail::vudaGetPhysicalDevice(device);

        //
        // get the QueueFamilyProperties of the PhysicalDevice
        std::vector<vk::QueueFamilyProperties> queueFamilyProperties = physDevice.getQueueFamilyProperties();

        //
        // get the first index into queueFamiliyProperties which supports compute
        size_t computeQueueFamilyIndex = std::distance(queueFamilyProperties.begin(), std::find_if(queueFamilyProperties.begin(), queueFamilyProperties.end(), [](vk::QueueFamilyProperties const& qfp) { return qfp.queueFlags & vk::QueueFlagBits::eCompute; }));
        assert(computeQueueFamilyIndex < queueFamilyProperties.size());

        //
        // create a UniqueDevice

        //
        // HARDCODED MAX NUMBER OF STREAMS
        const uint32_t queueCount = queueFamilyProperties[computeQueueFamilyIndex].queueCount;
        const uint32_t queueComputeCount = queueCount;
        const std::vector<float> queuePriority(queueComputeCount, 0.0f);
        //const float queuePriority[queueComputeCount] = {};
        vk::DeviceQueueCreateInfo deviceQueueCreateInfo(vk::DeviceQueueCreateFlags(), static_cast<uint32_t>(computeQueueFamilyIndex), queueComputeCount, queuePriority.data());

        //
        // create or get the logical device associated with the device id
        detail::logical_device* logicalDevice = detail::interface_logical_devices::create(device, deviceQueueCreateInfo, physDevice);

        //
        // assign this particular device to the thread
        detail::interface_thread_info::insert(std::this_thread::get_id(), device, logicalDevice);

        return vudaSuccess;
    }

} //namespace vuda