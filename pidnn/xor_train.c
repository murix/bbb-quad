/*
Fast Artificial Neural Network Library (fann)
Copyright (C) 2003-2012 Steffen Nissen (sn@leenissen.dk)

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include <stdio.h>

#include "fann.h"

int FANN_API test_callback(struct fann *ann, struct fann_train_data *train, unsigned int max_epochs, unsigned int epochs_between_reports, float desired_error, unsigned int epochs)
{
	printf("Epochs     %8d. MSE: %.5f. Desired-MSE: %.5f\n", epochs, fann_get_MSE(ann), desired_error);
	return 0;
}


int main()
{
	const unsigned int num_input = 2;
	const unsigned int num_output = 1;
	const unsigned int num_layers = 3;
	const unsigned int num_neurons_hidden = 3;
	struct fann *ann;
	ann = fann_create_standard(num_layers, num_input, num_neurons_hidden, num_output);
	//ann = fann_create_from_file("xor_fixed.net");
	
	fann_set_activation_function_hidden(ann, FANN_SIGMOID_SYMMETRIC);
	fann_set_activation_function_output(ann, FANN_SIGMOID);
	fann_set_train_error_function(ann, FANN_ERRORFUNC_LINEAR);
	fann_set_train_stop_function(ann, FANN_STOPFUNC_BIT);
	fann_set_bit_fail_limit(ann, 0.03f);
        fann_set_training_algorithm(ann, FANN_TRAIN_INCREMENTAL);
	fann_set_learning_momentum(ann, 0.4f);
	
	struct fann_train_data *data;
	data = fann_create_train(4,2,1);
	
	data->input[0][0]=-1;
	data->input[0][1]=-1;
	data->output[0][0]=0;

	data->input[1][0]=-1;
	data->input[1][1]=1;
	data->output[1][0]=1;

	data->input[2][0]=1;
	data->input[2][1]=-1;
	data->output[2][0]=1;

	data->input[3][0]=1;
	data->input[3][1]=1;
	data->output[3][0]=0;
	
	


	const float desired_error = (const float) 0;
	const unsigned int max_epochs = 10000;
	const unsigned int epochs_between_reports = 100;

        while(1){
	 fann_init_weights(ann, data);
	 fann_train_on_data(ann, data, max_epochs, epochs_between_reports, desired_error);
	 //fann_reset_MSE(ann);
	 fann_test_data(ann, data);
         printf("Testing network. bitfail=%d\n", fann_get_bit_fail(ann));
         if(fann_get_bit_fail(ann)==0) break;
        }

	for(int i = 0; i < fann_length_train_data(data); i++)
	{
	        fann_type *calc_out;
		calc_out = fann_run(ann, data->input[i]);
		printf("XOR test (%f,%f) -> %f, should be %f, difference=%f\n",
			   data->input[i][0], data->input[i][1], calc_out[0], data->output[i][0],  fann_abs(calc_out[0] - data->output[i][0]));
	}

	//printf("Saving network.\n");
	//fann_save(ann, "xor_float.net");
	//decimal_point = fann_save_to_fixed(ann, "xor_fixed.net");
	//fann_save_train_to_fixed(data, "xor_fixed.data", decimal_point);

	printf("Cleaning up.\n");
	fann_destroy_train(data);
	fann_destroy(ann);

	return 0;
}
