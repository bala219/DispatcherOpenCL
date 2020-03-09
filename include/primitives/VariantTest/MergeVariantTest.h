//
// Created by tompi on 6/29/18.
//

#ifndef OPENCLDISPATCHER_MERGEVARIANTTEST_H
#define OPENCLDISPATCHER_MERGEVARIANTTEST_H

#endif //OPENCLDISPATCHER_MERGEVARIANTTEST_H


#pragma once
#include <CL/cl.h>
#include "../header/Merging.h"
#include "../../globals.h"
#include "../../Environment.h"


void evaluate_Merge(const uint POWER_MAX_SIZE,const int KERNEL_MAX_WORK_GROUP_SIZE,const int LOOP_SIZE){

    /*const int POWER_MAX_SIZE=9; // log(512) , 512 is the maximum number of elements that you can process
    const int KERNEL_MAX_WORK_GROUP_SIZE=256; //Maximum number of threads that can be assigned
    const int LOOP_SIZE=10; //Number times you want to run the same evaluation to get average execution time*/
    setup_environment();
    print_environment();
    cl_device_id CPU,GPU;

    GPU=device[0][0];
    CPU=device[0][1];

    double GPU_exec_time=0;
    double CPU_exec_time=0;

/*
//------------------------------------------------------------------------------

    //First Get data
    uint _m_size_left=pow(2,9);
    uint _m_size_right=_m_size_left/2;
    uint _m_size_chunk=32;

    uint *_m_arr_left=(uint *) calloc((uint)_m_size_left, sizeof(uint)) ;
    uint *_m_arr_right=(uint *) calloc((uint)_m_size_right, sizeof(uint));
    int *_m_arr_result=(int *) calloc((int)_m_size_left, sizeof(int));


    for (int i=0,val=1;i<_m_size_left;i=i+2,val++)
    {
        _m_arr_left[i]=val;
        _m_arr_left[i+1]=val;
    }

    for(int i=0,val=1;i<_m_size_right;i++,val++)
    {
        if(val%5==0|val%7==0)
        {
            val++;
        }
        _m_arr_right[i]=val;
    }

//----------------------------------------------------------------------------------------------------
//Evaluate by changing chunk size
    cout<<"Chunk size \t  GPU Execution time \t CPU Execution time "<<endl;

    for (int i=5; i < 10; i++)  //Note global size= _size_left/_size_chunk. There is a max limit that can be assigned to global size from system to system
    {
        GPU_exec_time=0;
        _m_size_chunk=pow(2,i);
        //run it on GPU
        for(int loop=0;loop<LOOP_SIZE;loop++) {
            //call merge with CPU
            Merge(CPU, _m_arr_left, _m_arr_right, _m_arr_result, _m_size_left, _m_size_right, _m_size_chunk );
            CPU_exec_time=CPU_exec_time+execution_time_ns;

            //call merge with GPU
            Merge(GPU, _m_arr_left, _m_arr_right, _m_arr_result, _m_size_left, _m_size_right, _m_size_chunk);
            GPU_exec_time=GPU_exec_time+execution_time_ns;
        }

        CPU_exec_time=CPU_exec_time/LOOP_SIZE;
        GPU_exec_time=GPU_exec_time/LOOP_SIZE;
        cout<<pow(2,i)<<"\t"<<GPU_exec_time<<"\t"<<CPU_exec_time<<endl;
    }
//---------------------------------------------------------------------------------------------------

    //Evaluate by changing size of data

    _m_size_chunk=32;
    cout<<"Left array size \t  Right array size \t  GPU Execution time \t CPU Execution time"<<endl;

    for (int power=5;power<10;power++)
    {
        CPU_exec_time=0;
        GPU_exec_time=0;
        uint _m_size_left=pow(2,power);
        uint _m_size_right=_m_size_left/2;
        //_m_size_chunk=pow(2,power-2);
        uint *_m_arr_left=(uint *) calloc((uint)_m_size_left, sizeof(uint)) ;
        uint *_m_arr_right=(uint *) calloc((uint)_m_size_right, sizeof(uint));
        int *_m_arr_result=(int *) calloc((int)_m_size_left, sizeof(int));


        for (int i=0,val=1;i<_m_size_left;i=i+2,val++)
        {
            _m_arr_left[i]=val;
            _m_arr_left[i+1]=val;
        }


        for(int i=0,val=1;i<_m_size_right;i++,val++)
        {
            if(val%5==0|val%7==0)
            {
                val++;
            }
            _m_arr_right[i]=val;
        }

        //call merge with GPU
        for(int loop=0;loop<LOOP_SIZE;loop++) {
            //call merge with CPU
            Merge(CPU, _m_arr_left, _m_arr_right, _m_arr_result, _m_size_left, _m_size_right, _m_size_chunk );
            CPU_exec_time=CPU_exec_time+execution_time_ns;

            //call merge with GPU
            Merge(GPU, _m_arr_left, _m_arr_right, _m_arr_result, _m_size_left, _m_size_right, _m_size_chunk);
            GPU_exec_time=GPU_exec_time+execution_time_ns;

        }
        CPU_exec_time=CPU_exec_time/LOOP_SIZE;
        GPU_exec_time=GPU_exec_time/LOOP_SIZE;
        cout<<pow(2,power)<<"\t"<<pow(2,power-1)<<"\t"<<GPU_exec_time<<"\t"<<CPU_exec_time<<"\n";
    }
    */

    //--------------------------------------------------------------------------------------------------------------------------
    //Testing for max device work group size

    //Evaluate by changing size of data


    cout<<"Left_size \t  Right_size \t Chunk_size \t GPU_time \t CPU_time"<<endl;

    for (int power_size=3;power_size<=POWER_MAX_SIZE;power_size++)
    {
        for (int power_chunk=3; power_chunk <=power_size; power_chunk++)  //Note global size= _size_left/_size_chunk. There is a max limit that can be assigned to global size from system to system
        {
            //if ((power_chunk == 3&(power_size == 8|9))|(power_chunk == 4&power_size == 9)) {
            //if((power_size-power_chunk)<5)
            //{
                CPU_exec_time = 0;
                GPU_exec_time = 0;
                uint _m_size_left = pow(2, power_size);
                uint _m_size_right = _m_size_left / 2;
                uint _m_size_chunk = pow(2, power_chunk);
                if (_m_size_left / _m_size_chunk <= KERNEL_MAX_WORK_GROUP_SIZE) {
                    uint *_m_arr_left = (uint *) calloc((uint) _m_size_left, sizeof(uint));
                    uint *_m_arr_right = (uint *) calloc((uint) _m_size_right, sizeof(uint));
                    int *_m_arr_result = (int *) calloc((int) _m_size_left, sizeof(int));


                    for (int i = 0, val = 1; i < _m_size_left; i = i + 2, val++) {
                        _m_arr_left[i] = val;
                        _m_arr_left[i + 1] = val;
                    }


                    for (int i = 0, val = 1; i < _m_size_right; i++, val++) {
                        if (val % 5 == 0 | val % 7 == 0) {
                            val++;
                        }
                        _m_arr_right[i] = val;
                    }

                    //call merge with GPU
                    for (int loop = 0; loop < LOOP_SIZE; loop++) {
                        //call merge with CPU
                        Merge(CPU, _m_arr_left, _m_arr_right, _m_arr_result, _m_size_left, _m_size_right,
                              _m_size_chunk);
                        CPU_exec_time = CPU_exec_time + execution_time_ns;

                        //call merge with GPU
                        Merge(GPU, _m_arr_left, _m_arr_right, _m_arr_result, _m_size_left, _m_size_right,
                              _m_size_chunk);
                        GPU_exec_time = GPU_exec_time + execution_time_ns;

                    }
                    CPU_exec_time = CPU_exec_time / LOOP_SIZE;
                    GPU_exec_time = GPU_exec_time / LOOP_SIZE;
                    cout << _m_size_left << "\t" << _m_size_right << "\t" << _m_size_chunk << "\t" << GPU_exec_time
                         << "\t" << CPU_exec_time
                         << "\n";
                }
            //}
        }
    }

}