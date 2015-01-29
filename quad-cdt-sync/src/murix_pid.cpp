/*
The MIT License (MIT)

Copyright (c) 1981-2015 Murilo Pontes

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
 */

#include "murix_pid.h"
#include "timestamps.h"

#include <cstdlib>

#define PID_IDX_NOW  0
#define PID_IDX_PREV 1


double fRand(double fMin, double fMax)
{
    double f = (double) rand() / double(RAND_MAX);
    return fMin + f * (fMax - fMin);
}

///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

murix_perceptron::murix_perceptron(int len){
	this->len=len;

	this->input = new double[this->len+1];
	this->weight = new double[this->len+1];
	this->sum = 0;
	this->output = 0;
	this->learning_rate = 0.05;

    for(int i=0;i<this->len+1;i++){
    	this->weight[i]=fRand(0,1);
    }
}

double murix_perceptron::run(double* sample){
    for(int i=0;i<this->len;i++){
    	this->input[i]=sample[i];
    }
    this->input[this->len]=1;
    this->sum=0;
    for(int i=0;i<this->len+1;i++){
    	sum+=this->weight[i]*this->input[i];
    }
    this->output=this->sum;
    return this->output;
}

void murix_perceptron::train(double error){
    for(int i=0;i<this->len+1;i++){
    	this->weight[i] += this->learning_rate*error*this->input[i];
    }
}




///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

murix_controller::murix_controller(){
	this->setpoint=0;
	this->feedback=0;
	this->output=0;
	this->sample=new double[3];
	this->p = new murix_perceptron(3);
}

double murix_controller::act(double setpoint){
	this->setpoint=setpoint;
	this->sample[0]=this->setpoint;
	this->sample[1]=this->feedback;
	this->sample[2]=this->output;

	this->output = this->p->run(this->sample);

	return this->output;
}

void murix_controller::update_feedback(double feedback){
	this->feedback=feedback;
	this->p->train(this->setpoint-this->feedback);
}

///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

murix_pid::murix_pid(){

	feedback=0;
	target=0;
	actuator=0;

	kp=0;
	ki=0;
	kd=0;


	perror[PID_IDX_NOW]=0;
	perror[PID_IDX_PREV]=0;
	ierror=0;
	derror=0;


	error_min=0;
	error_max=0;

	time[PID_IDX_NOW]=0;
	time[PID_IDX_PREV]=0;
}

void murix_pid::update(){


    // time stuff
	time[PID_IDX_PREV] = time[PID_IDX_NOW];
	time[PID_IDX_NOW]= get_timestamp_in_seconds();
	dt = time[PID_IDX_NOW]-time[PID_IDX_PREV];

	// proportional term
	perror[PID_IDX_PREV] = perror[PID_IDX_NOW];
	perror[PID_IDX_NOW] = target - feedback;

	// integral term
	ierror += perror[PID_IDX_NOW] * dt;
	if(ierror>error_max) ierror=error_max;
	if(ierror<error_min) ierror=error_min;

	// derivation term
	derror =  (perror[PID_IDX_NOW]-perror[PID_IDX_PREV])/dt;

	// system action
    actuator = kp*perror[PID_IDX_NOW]+ki*ierror+kd*derror;

}


