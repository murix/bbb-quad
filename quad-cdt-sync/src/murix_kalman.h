/*
 * murix_kalman.h
 *
 *  Created on: 12/01/2015
 *      Author: murix
 */

#ifndef MURIX_KALMAN_H_
#define MURIX_KALMAN_H_

#define IDX_NOW  0
#define IDX_PREV 1

class murix_kalman {

	double A;
	double X[2];
	double B;
	double I;
	double U;
	double Q;
	double P[2];
	double K;
	double R;
	double H;

	murix_kalman(){
		A=1;
		X[IDX_NOW]=0;
		X[IDX_PREV]=0;
		B=1;
		I=1;
		U=0;
		Q=0;
		P[IDX_PREV]=0;
		P[IDX_NOW]=0;
		K=0.5;
		R=0;
		H=1;
	}
	double transpose(double M){
		return M;
	}
	double inverse(double M){
		return 1/M;
	}

	//prediction
	void time_update(){
		//project the state ahead
		X[IDX_NOW]=A*X[IDX_PREV]+B*U;
		//project the error covariance ahead
		P[IDX_NOW]=A*P[IDX_PREV]*transpose(A)+Q;
	}
	void measuarement_update(double z){
		//compute kalman gain
		K = P[IDX_NOW]*transpose(H)*inverse(H*P[IDX_NOW]*transpose(H)+R);
		// update the estimate via z
		X[IDX_PREV]=X[IDX_NOW];
		X[IDX_NOW] = X[IDX_NOW]+K*(z - H*X[IDX_NOW]);
		// update the error covariance
		P[IDX_PREV]=P[IDX_NOW];
		P[IDX_NOW]=(I-K*H)*P[IDX_NOW];
	}


};



#endif /* MURIX_KALMAN_H_ */
