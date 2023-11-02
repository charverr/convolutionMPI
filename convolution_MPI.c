#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define N 10
#define M 12
/*
int **convolution(int local_filter[3][3], int recieved_image[3][8]){
    
    int conv_N = 3-3+1; // 3x3 -> N-3+1
    int conv_M = 8-3+1; // 3x3 -> M-3+1


    int **output;
    output= malloc(sizeof(int*) * conv_N);
     
    for(int i = 0; i < conv_N; i++) {
        output[i] = malloc(sizeof(int*) * conv_M);
    }

    for ( int i = 0; i <conv_N; i++){
        for ( int j = 0; j < conv_M; j++){
            output[i+1][j+1] =  (local_filter[0][0]*recieved_image[i][j]+  local_filter[0][1]*recieved_image[i][j+1]+  local_filter[0][2]*recieved_image[i][j+2]
                        +local_filter[1][0]*recieved_image[i+1][j]+local_filter[1][1]*recieved_image[i+1][j+1]+local_filter[1][2]*recieved_image[i+1][j+2]
                        +local_filter[2][0]*recieved_image[i+2][j]+local_filter[2][1]*recieved_image[i+2][j+1]+local_filter[2][2]*recieved_image[i+2][j+2])/16; //3x3 ->16
        }   
    }

    return output;

}
*/
int main(int argc, char** argv){





    int rank, size, length;
    char name[80];




    int image[N][M]={{1,2,3,4,5,6,7,8,9,10,11,12},
                    {1,2,3,4,5,6,7,8,9,10,11,12},
                    {1,2,3,4,5,6,7,8,9,10,11,12},
                    {5,6,7,8,9,10,11,12,13,14,15,16},
                    {5,6,7,8,9,10,11,12,13,14,15,16},

                    {5,6,7,8,9,10,11,12,13,14,15,16},
                    {8,9,10,11,12,13,14,15,16,17,18,19},
                    {8,9,10,11,12,13,14,15,16,17,18,19},
                    {8,9,10,11,12,13,14,15,16,17,18,19},
                    {10,9,10,11,12,13,14,15,16,17,18,19}};




    
    int filter[3][3]={{1,2,1},{2,4,2},{1,2,1}}; 
    int back_ray[18];
    
    
    MPI_Init(&argc,&argv);
    MPI_Status status;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Get_processor_name(name,&length);

    int block = N/size;
    int recieved_image[block][M];
    int recieved_image2[4][M];
    int lines_left = N-block*size;
    int last_N = lines_left+2;
    int last_lines[last_N][M];

    int local_conv1_N = block-3+1; 
    int local_conv1_M = M-3+1; 

    int local_conv2_N = 4-3+1; 
    int local_conv2_M = M-3+1; 

    int local_conv3_N = last_N-3+1; 
    int local_conv3_M = M-3+1; 

    int local_output1[local_conv1_N][local_conv1_M];
    int local_output2[local_conv2_N][local_conv2_M];
    int local_output3[local_conv3_N][local_conv3_M];

    //1st Scatter
    MPI_Scatter(image,M*block, MPI_INT, recieved_image, M*block, MPI_INT,0, MPI_COMM_WORLD);

    // MUST CHECK IF THE BLOCK SIZE IS AT LEAST 3

    printf("Rank %d\n",rank);
    printf("Recieved block \n");
    for(int i = 0; i < block; i++){
        for(int j = 0; j < M; j++){
            printf(" %d ",recieved_image[i][j]);
        }
        printf("\n");
    }

    if(rank==0 && N%size!=0){
        printf("lines left %d \n",lines_left);
    }
    // Convolution 1
    for ( int i = 0; i <local_conv1_N; i++){
        for ( int j = 0; j < local_conv1_M; j++){
            local_output1[i][j] =  (filter[0][0]*recieved_image[i][j]+  filter[0][1]*recieved_image[i][j+1]+  filter[0][2]*recieved_image[i][j+2]
                        +filter[1][0]*recieved_image[i+1][j]+filter[1][1]*recieved_image[i+1][j+1]+filter[1][2]*recieved_image[i+1][j+2]
                        +filter[2][0]*recieved_image[i+2][j]+filter[2][1]*recieved_image[i+2][j+1]+filter[2][2]*recieved_image[i+2][j+2])/16; //3x3 ->16
        }   
    }    

    printf("local output1 \n");
    for(int i = 0; i < local_conv1_N; i++){
        for(int j = 0; j < local_conv1_M; j++){
            printf(" %d ",local_output1[i][j]);
        }
        printf("\n");
    }
    // 2nd Scatter
    if(rank==0){
        //Sending
        for(int i = 0; i < size-1; i++){
            MPI_Send(image+(block-2)+block*i,4*M,MPI_INT,i,1,MPI_COMM_WORLD);
            
        }
        if(lines_left!=0){
            MPI_Send(image+block*(size-1)+(block-2),last_N*M, MPI_INT,(size-1),2,MPI_COMM_WORLD);
        }
    }

    if(rank < size-1){
    printf("2nd Scatter \n");
       
        MPI_Recv( recieved_image2,4*M ,MPI_INT ,0,1 ,MPI_COMM_WORLD, &status);
        printf("Recieved block 2 \n");
        for(int i = 0; i < 4; i++){
            for(int j = 0; j < M; j++){
                printf(" %d ",recieved_image2[i][j]);
            }
            printf("\n");
        }

    /// CONVOLUTION 2
    for ( int i = 0; i <local_conv2_N; i++){
        for ( int j = 0; j < local_conv2_M; j++){
            local_output2[i][j] =  (filter[0][0]*recieved_image2[i][j]+  filter[0][1]*recieved_image2[i][j+1]+  filter[0][2]*recieved_image2[i][j+2]
                        +filter[1][0]*recieved_image2[i+1][j]+filter[1][1]*recieved_image2[i+1][j+1]+filter[1][2]*recieved_image2[i+1][j+2]
                        +filter[2][0]*recieved_image2[i+2][j]+filter[2][1]*recieved_image2[i+2][j+1]+filter[2][2]*recieved_image2[i+2][j+2])/16; //3x3 ->16
        }   
    }  

    printf("local output2 \n");
    for(int i = 0; i < local_conv2_N; i++){
        for(int j = 0; j < local_conv2_M; j++){
            printf(" %d ",local_output2[i][j]);
        }
        printf("\n");
    }
        
    } else if (rank == size-1 && lines_left!=0){
        MPI_Recv( last_lines,last_N*M,MPI_INT ,0,2 ,MPI_COMM_WORLD, &status);
        printf("Recieved block 2 for last lines \n");
        for(int i = 0; i < last_N; i++){
            for(int j = 0; j < M; j++){
                printf(" %d ",last_lines[i][j]);
            }
            printf("\n");
        }

            /// CONVOLUTION 3
        for ( int i = 0; i <local_conv3_N; i++){
            for ( int j = 0; j < local_conv3_M; j++){
                local_output3[i][j] =  (filter[0][0]*last_lines[i][j]+  filter[0][1]*last_lines[i][j+1]+  filter[0][2]*last_lines[i][j+2]
                            +filter[1][0]*last_lines[i+1][j]+filter[1][1]*last_lines[i+1][j+1]+filter[1][2]*last_lines[i+1][j+2]
                            +filter[2][0]*last_lines[i+2][j]+filter[2][1]*last_lines[i+2][j+1]+filter[2][2]*last_lines[i+2][j+2])/16; //3x3 ->16
            }   
        } 

        printf("local output3 \n");
        for(int i = 0; i < local_conv3_N; i++){
            for(int j = 0; j < local_conv3_M; j++){
                printf(" %d ",local_output3[i][j]);
            }
            printf("\n");
        }
    }


 



    MPI_Finalize();

    return 0;
}
