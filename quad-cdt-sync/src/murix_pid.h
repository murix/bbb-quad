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

#ifndef MURIX_PID_H_
#define MURIX_PID_H_

class murix_perceptron {
public:
	double* input;
	double* weight;
	double* wchange;
	double sum;
	double output;
	double learning_rate;
	int len;
	murix_perceptron(int len);
    double run(double* sample);
    void train(double error);
};

class murix_controller {
public:
	double feedback;
	double target;
	double output;
	double* sample;
	murix_perceptron* p;

	murix_controller();
	double update(double min_output,double max_output);


};


class murix_pid {
public:



	murix_pid();
	void update();
	void reset_integral();

	//
	//
	double error_min;
	double error_max;
	//
	double feedback;
	double target;
	//
	double output;
	//
	double kp;
	double ki;
	double kd;

private:
	//
	double perror[2];
	double ierror;
	double derror;
	//
	double time[2];
    double dt;
};



#endif /* MURIX_PID_H_ */
