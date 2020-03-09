//
// Created by tompi on 6/29/18.
//

#ifndef OPENCLDISPATCHER_BITONICVARIANTTEST_H
#define OPENCLDISPATCHER_BITONICVARIANTTEST_H

#pragma once

#include "../../Environment.h"
#include "../../distribution.h"
#include "../header/BitonicSorting.h"
#include "../header/Aggregating.h"

enum device_num {
    GPU = 0,
    CPU = 1
};

enum bs_element_variant {
    repeated_random = 0,
    unique_random = 1,
    ascending = 2,
    descending = 3
};

enum agg_element_variant{
    repeated_sorted = 0,
    unique_sorted = 1,
    all_same = 2,
    duplicate = 3
};

enum agg_size_variant{
    global_size = 0,
    local_size = 1,
};

double cpu_nanosecs, gpu_nanosecs;

/*void evaluate() {
    setup_environment();
    print_environment();
    cl_device_id DEVICE;

    DEVICE = device[0][0];

    long WG_SIZE;
    clGetDeviceInfo(DEVICE, CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(long), &WG_SIZE, NULL); // 256

    cout << WG_SIZE <<  "\t" << log(WG_SIZE)/log(2) << endl;

    DEVICE = device[0][1];
    clGetDeviceInfo(DEVICE, CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(long), &WG_SIZE, NULL); // 8192

    cout << WG_SIZE <<  "\t" << log(WG_SIZE)/log(2) << endl;
}*/

void bs_evaluation(int _loop) {

    setup_environment();
    print_environment();
    cl_device_id DEVICE;

    // For Max Element Size
    DEVICE = device[0][0];
    long WG_SIZE;
    clGetDeviceInfo(DEVICE, CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(long), &WG_SIZE, NULL); // 256

    int pow_size = log(WG_SIZE)/log(2) + 1;

    //For bs_element_array_variant
    for (int i = 0; i < 4; i++) {
        switch (i) {
            case repeated_random: {

                cout << "REPEATED RANDOM NUMBERS" << endl;
                cout << "ELENENTS\t" << "GPU_EXECUTION_TIME\t" << "CPU_EXECUTION_TIME" << endl;
                // For array of elements
                for (int j = 2; j <= pow_size; j++) {

                    // Generate random numbers
                    int m_size = pow(2, j);
                    // Set the Generate Size in distribution.h file
                    setGen(m_size);
                    // Create an array of _size
                    uint m_rand_arr[m_size];
                    for (int k = 0; k < m_size; k++) {
                        m_rand_arr[k] = UniformRandom();
                    }

                    // For Devices
                    for (int l = 0; l < 2; l++) {
                        switch (l) {
                            case GPU: {
                                // For Multiple Number of execution of Same data set
                                DEVICE = device[0][0];
                                double total_gpu_exe_time = 0;
                                for (int m = 0; m < _loop; m++) {
                                    BitonicSort(DEVICE, m_rand_arr, m_size);
                                    total_gpu_exe_time += execution_time_ns;
                                }
                                gpu_nanosecs = total_gpu_exe_time / _loop;
                            }
                                break;
                            case CPU: {
                                DEVICE = device[0][1];
                                // For Multiple Number of execution of Same data set
                                double total_cpu_exe_time = 0;
                                for (int m = 0; m < _loop; m++) {
                                    BitonicSort(DEVICE, m_rand_arr, m_size);
                                    total_cpu_exe_time += execution_time_ns;
                                }
                                cpu_nanosecs = total_cpu_exe_time / _loop;
                            }
                                break;
                        }
                    }
                    cout << m_size << "\t" << gpu_nanosecs << "\t" << cpu_nanosecs << "" << endl;
                }
            }
                break;
            case unique_random: {

                cout << "UNIQUE RANDOM NUMBERS" << endl;
                cout << "ELENENTS\t" << "GPU_EXECUTION_TIME\t" << "CPU_EXECUTION_TIME" << endl;
                // For array of elements
                for (int j = 2; j <= pow_size; j++) {

                    // Generate random numbers
                    int m_size = pow(2, j);
                    // Set the Generate Size in distribution.h file
                    setGen(m_size);
                    InitDenseUnique();
                    // Create an array of _size
                    uint m_rand_arr[m_size];
                    for (int k = 0; k < m_size; k++) {
                        m_rand_arr[k] = DenseUniqueRandom();
                    }

                    // For Devices
                    for (int l = 0; l < 2; l++) {
                        switch (l) {
                            case GPU: {
                                // For Multiple Number of execution of Same data set
                                DEVICE = device[0][0];
                                double total_gpu_exe_time = 0;
                                for (int m = 0; m < _loop; m++) {
                                    BitonicSort(DEVICE, m_rand_arr, m_size);
                                    total_gpu_exe_time += execution_time_ns;
                                }
                                gpu_nanosecs = total_gpu_exe_time / _loop;
                            }
                                break;
                            case CPU: {
                                DEVICE = device[0][1];
                                // For Multiple Number of execution of Same data set
                                double total_cpu_exe_time = 0;
                                for (int m = 0; m < _loop; m++) {
                                    BitonicSort(DEVICE, m_rand_arr, m_size);
                                    total_cpu_exe_time += execution_time_ns;
                                }
                                cpu_nanosecs = total_cpu_exe_time / _loop;
                            }
                                break;
                        }
                    }
                    cout << m_size << "\t" << gpu_nanosecs << "\t" << cpu_nanosecs << "" << endl;
                }
            }
                break;
            case ascending: {
                cout << "ASCENDING NUMBERS" << endl;
                cout << "ELENENTS\t" << "GPU_EXECUTION_TIME\t" << "CPU_EXECUTION_TIME" << endl;
                // For array of elements
                for (int j = 2; j <= pow_size; j++) {
                    // Generate random numbers
                    int m_size = pow(2, j);
                    // Set the Starting element Value in distribution.h file
                    setStartEle(j);
                    // Create an array of _size
                    uint m_rand_arr[m_size];
                    for (int k = 0; k < m_size; k++) {
                        m_rand_arr[k] = AscendingNumbers();
                    }

                    // For Devices
                    for (int l = 0; l < 2; l++) {
                        switch (l) {
                            case GPU: {
                                // For Multiple Number of execution of Same data set
                                DEVICE = device[0][0];
                                double total_gpu_exe_time = 0;
                                for (int m = 0; m < _loop; m++) {
                                    BitonicSort(DEVICE, m_rand_arr, m_size);
                                    total_gpu_exe_time += execution_time_ns;
                                }
                                gpu_nanosecs = total_gpu_exe_time / _loop;
                            }
                                break;
                            case CPU: {
                                DEVICE = device[0][1];
                                // For Multiple Number of execution of Same data set
                                double total_cpu_exe_time = 0;
                                for (int m = 0; m < _loop; m++) {
                                    BitonicSort(DEVICE, m_rand_arr, m_size);
                                    total_cpu_exe_time += execution_time_ns;
                                }
                                cpu_nanosecs = total_cpu_exe_time / _loop;
                            }
                                break;
                        }
                    }
                    cout << m_size << "\t" << gpu_nanosecs << "\t" << cpu_nanosecs << "" << endl;
                }
            }
                break;
            case descending: {
                cout << "DECENDING NUMBERS" << endl;
                cout << "ELENENTS\t" << "GPU_EXECUTION_TIME\t" << "CPU_EXECUTION_TIME" << endl;
                // For array of elements
                for (int j = 2; j <= pow_size; j++) {

                    // Generate random numbers
                    int m_size = pow(2, j);
                    // Set the Starting element Value in distribution.h file
                    setStartEle(j + m_size);
                    // Create an array of _size
                    uint m_rand_arr[m_size];
                    for (int k = 0; k < m_size; k++) {
                        m_rand_arr[k] = DecendingNumbers();
                    }

                    // For Devices
                    for (int l = 0; l < 2; l++) {
                        switch (l) {
                            case GPU: {
                                // For Multiple Number of execution of Same data set
                                DEVICE = device[0][0];
                                double total_gpu_exe_time = 0;
                                for (int m = 0; m < _loop; m++) {
                                    BitonicSort(DEVICE, m_rand_arr, m_size);
                                    total_gpu_exe_time += execution_time_ns;
                                }
                                gpu_nanosecs = total_gpu_exe_time / _loop;
                            }
                                break;
                            case CPU: {
                                DEVICE = device[0][1];
                                // For Multiple Number of execution of Same data set
                                double total_cpu_exe_time = 0;
                                for (int m = 0; m < _loop; m++) {
                                    BitonicSort(DEVICE, m_rand_arr, m_size);
                                    total_cpu_exe_time += execution_time_ns;
                                }
                                cpu_nanosecs = total_cpu_exe_time / _loop;

                            }
                                break;
                        }
                    }
                    cout << m_size << "\t" << gpu_nanosecs << "\t" << cpu_nanosecs << "" << endl;
                }
            }
                break;
        }

    }
}

void agg_evaluation(int _loop) {
    setup_environment();
    print_environment();
    cl_device_id DEVICE;

    // For Max Element Size
    DEVICE = device[0][0];
    long WG_SIZE;
    clGetDeviceInfo(DEVICE, CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(long), &WG_SIZE, NULL); // 256

    int pow_size = log(WG_SIZE)/log(2) + 1;

    //For bs_element_array_variant
    for(int i = 0; i < 4; i++) {
        switch (i) {
            case repeated_sorted: {
                cout << "REPEATED SORTED NUMBERS" << endl;
                cout << "ELENENTS\t" << "GS\t" << "LS\t" << "GPU_ET\t" << "CPU_ET" << endl;
                // For array of elements
                for (int j = 2; j <= pow_size; j++) {
                    // Generate random numbers
                    int m_size = pow(2, j);
                    // Set the Generate Size in distribution.h file
                    setGen(m_size);
                    // Create an array of _size
                    uint m_rand_arr[m_size];
                    for (int k = 0; k < m_size; k++) {
                        m_rand_arr[k] = UniformRandom();
                    }

                    // For Devices GPU
                    DEVICE = device[0][0];

                    // Fetch sorted Array
                    BitonicSort(DEVICE, m_rand_arr, m_size);

                    // For local size & global size
                    for (int l = 0; l < j; l++) {
                        uint _ls = pow(2, l);
                        uint _gs = m_size / _ls;

                        if (_gs <= WG_SIZE && _gs >= _ls) {
                            double total_gpu_exe_time = 0, total_cpu_exe_time = 0;
                            for (int k = 0; k < _loop; k++) {
                                // FOR GPU
                                Aggregate(DEVICE, m_rand_arr, m_size, _gs, _ls);
                                total_gpu_exe_time += execution_time_ns;
                                // FOR CPU
                                DEVICE = device[0][1];
                                Aggregate(DEVICE, m_rand_arr, m_size, _gs, _ls);
                                total_cpu_exe_time += execution_time_ns;
                            }
                            gpu_nanosecs = total_gpu_exe_time / _loop;
                            cpu_nanosecs = total_cpu_exe_time / _loop;

                            cout << m_size << "\t" << _gs << "\t" << _ls << "\t" << gpu_nanosecs << "\t" << cpu_nanosecs << endl;

                        }
                    }
                }
            }
                break;
            /*case unique_sorted: {
                cout << "UNIQUE SORTED NUMBERS" << endl;
                cout << "ELENENTS\t" << "GS\t" << "LS\t" << "GPU_ET\t" << "CPU_ET" << endl;
                // For array of elements
                for (int j = 2; j <= pow_size; j++) {
                    // Generate random numbers
                    int m_size = pow(2, j);
                    // Set the Generate Size in distribution.h file
                    setGen(m_size);
                    // Create an array of _size
                    uint m_rand_arr[m_size];
                    for (int k = 0; k < m_size; k++) {
                        m_rand_arr[k] = SequentialNumbers(2);
                    }

                    // For local size & global size
                    for (int l = 0; l < j; l++) {
                        uint _ls = pow(2, l);
                        uint _gs = m_size / _ls;

                        if (_gs <= WG_SIZE && _gs >= _ls) {
                            double total_gpu_exe_time = 0, total_cpu_exe_time = 0;
                            for (int k = 0; k < _loop; k++) {
                                // FOR GPU
                                DEVICE = device[0][0];
                                Aggregate(DEVICE, m_rand_arr, m_size, _gs, _ls);
                                total_gpu_exe_time += execution_time_ns;
                                // FOR CPU
                                DEVICE = device[0][1];
                                Aggregate(DEVICE, m_rand_arr, m_size, _gs, _ls);
                                total_cpu_exe_time += execution_time_ns;
                            }
                            gpu_nanosecs = total_gpu_exe_time / _loop;
                            cpu_nanosecs = total_cpu_exe_time / _loop;

                            cout << m_size << "\t" << _gs << "\t" << _ls << "\t" << gpu_nanosecs << "\t" << cpu_nanosecs << endl;
                        }
                        else
                            cout << "HI\t" << _gs << "\t" << _ls << endl;
                    }
                }
            }
                break;*/
            /*case all_same: {
                cout << "ALL SAME NUMBERS" << endl;
                cout << "ELENENTS\t" << "GS\t" << "LS\t" << "GPU_ET\t" << "CPU_ET" << endl;
                // For array of elements
                for (int j = 2; j <= pow_size; j++) {
                    // Generate random numbers
                    int m_size = pow(2, j);
                    // Set the Generate Size in distribution.h file
                    setGen(m_size);
                    // Create an array of _size
                    uint m_rand_arr[m_size];
                    for (int k = 0; k < m_size; k++) {
                        m_rand_arr[k] = 10;
                    }

                    // For local size & global size
                    for (int l = 0; l < j; l++) {
                        uint _ls = pow(2, l);
                        uint _gs = m_size / _ls;

                        if (_gs <= WG_SIZE && _gs >= _ls) {
                            double total_gpu_exe_time = 0, total_cpu_exe_time = 0;
                            for (int k = 0; k < _loop; k++) {
                                // FOR GPU
                                DEVICE = device[0][0];
                                Aggregate(DEVICE, m_rand_arr, m_size, _gs, _ls);
                                total_gpu_exe_time += execution_time_ns;
                                // FOR CPU
                                DEVICE = device[0][1];
                                Aggregate(DEVICE, m_rand_arr, m_size, _gs, _ls);
                                total_cpu_exe_time += execution_time_ns;
                            }
                            gpu_nanosecs = total_gpu_exe_time / _loop;
                            cpu_nanosecs = total_cpu_exe_time / _loop;

                            cout << m_size << "\t" << _gs << "\t" << _ls << "\t" << gpu_nanosecs << "\t" << cpu_nanosecs << endl;
                        }
                        else
                            cout << "HI\t" << _gs << "\t" << _ls << endl;
                    }
                }
            }
                break;*/
            case duplicate: {
                cout << "DUPLICATE NUMBERS" << endl;
                cout << "ELENENTS\t" << "DUP_%" << "GS\t" << "LS\t" << "GPU_ET\t" << "CPU_ET" << endl;

                // For Duplicate Percent of elements
                for (int m = 0; m <= 100; m+=10) {
                    // For array of elements
                    for (int j = 2; j <= pow_size; j++) {
                        // Generate random numbers
                        int m_size = pow(2, j);
                        // Set the Generate Size in distribution.h file
                        setGen(m_size);
                        // Create an array of _size
                        uint m_rand_arr[m_size];
                        uint p_ele = (m_size * m) / 100;
                        for (int k = 0; k < m_size; k++) {
                            m_rand_arr[k] = (p_ele + k >= m_size - 1) ? k+1 : m_size;
                        }

                        if((m == 0) || (m > 0 && p_ele > 1)) {
                            // For local size & global size
                            for (int l = 0; l < j; l++) {
                                uint _ls = pow(2, l);
                                uint _gs = m_size / _ls;

                                if (_gs <= WG_SIZE && _gs >= _ls) {
                                    double total_gpu_exe_time = 0, total_cpu_exe_time = 0;
                                    for (int k = 0; k < _loop; k++) {
                                        // FOR GPU
                                        DEVICE = device[0][0];
                                        Aggregate(DEVICE, m_rand_arr, m_size, _gs, _ls);
                                        total_gpu_exe_time += execution_time_ns;
                                        // FOR CPU
                                        DEVICE = device[0][1];
                                        Aggregate(DEVICE, m_rand_arr, m_size, _gs, _ls);
                                        total_cpu_exe_time += execution_time_ns;
                                    }
                                    gpu_nanosecs = total_gpu_exe_time / _loop;
                                    cpu_nanosecs = total_cpu_exe_time / _loop;

                                    cout << m_size << "\t" << m << "\t" << _gs << "\t" << _ls << "\t" << gpu_nanosecs << "\t" << cpu_nanosecs << endl;
                                }
                                //else
                                //cout << "HI\t" << _gs << "\t" << _ls << endl;
                            }
                        }
                    }
                }
            }
                break;
        }
    }
}

#endif //OPENCLDISPATCHER_BITONICVARIANTTEST_H
