#include "ap_int.h"
#include "math.h"
#define weight_size 8
#define activation_size 8
#define psum_size 32
#define activation_depth 128
#define buffer_number 128
#define weight_column 128

typedef ap_int<activation_size> activation;
typedef ap_int<weight_size> weight;
typedef ap_int<psum_size> psum;

void systolic (activation input_activation[activation_depth][buffer_number], weight input_weight[buffer_number][weight_column], psum output_psum[activation_depth][weight_column]) {

int i,j,k;
  
static psum psum_array[buffer_number+1][weight_column];
#pragma HLS ARRAY_PARTITION variable=psum_array cyclic factor=16 dim=2

//these are loaded
static weight weight_buffer[buffer_number][weight_column];
#pragma HLS ARRAY_PARTITION variable=weight_buffer cyclic factor=16 dim=2

static activation activation_buffer[activation_depth][buffer_number];
#pragma HLS ARRAY_PARTITION variable=activation_buffer cyclic factor=16 dim=2


for (i=0;i<buffer_number;i++){
  for(j=0;j<weight_column;j++){
    //#pragma HLS unroll 
    weight_buffer[i][j]= input_weight[i][j];
    
  }
}


for (k=0;k<buffer_number;k++){
  for(i=0;i<activation_depth;i++){
    //#pragma HLS unroll 
    activation_buffer[i][k]= input_activation[i][k];
   
    
  }
}
  
for(j=0;j<weight_column;++j){
    //#pragma HLS unroll
    #pragma HLS pipeline II = 1
    psum_array[0][j]=0;
  }


for(k=0;k<activation_depth;k++){
  activation a_shift [weight_column+1];
  #pragma HLS ARRAY_PARTITION variable=a_shift complete dim=1
  a_shift[0]=activation_buffer[k][0];
 for (i=0;i<buffer_number;i++){
  #pragma HLS unroll 
   for(j=0;j<weight_column;j++){
     #pragma HLS unroll 
     //#pragma HLS pipeline II = 1
     psum_array[i+1][j]=(psum)(a_shift[j]*weight_buffer[i][j])+psum_array[i][j];
   }
   for(j=weight_column;j>0;--j){
     #pragma HLS pipeline II = 1
     a_shift[j]=a_shift[j-1];
   }
    if(i+1<buffer_number){
    a_shift[0]=activation_buffer[k][i+1];
    }
    
  }
   for(j=0;j<weight_column;j++){
     #pragma HLS unroll
     output_psum[k][j]= psum_array[buffer_number][j];
   }
 }
}
