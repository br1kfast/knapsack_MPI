#include<stdlib.h>
#include<stdio.h>
#define MPICH_SKIP_MPICXX
#include "mpi.h"
#include "math.h"
#include<sys/time.h>
#include<unistd.h>

/*Используйте методы динамического планирования для решения проблем с рюкзаком*/
/*使用动态规划方法解决背包问题*/
	/*struct timeval{
                long tv_sec;//seconds
                long tv_usec;//微秒

        };
        struct timezone {
                int tz_minuteswest;//和Greenwich时间差了多少分钟
                int tz_dsttime;//日光节约时间的状态
        };*/


int main (int argc,char *argv[]){   //命令行参数，argc 是指传入参数的个数，argv[] 是一个指针数组，指向传递给程序的每个参数
    MPI_Status status;


    int len;
    char name[MPI_MAX_PROCESSOR_NAME];
    int m, my_rank, pnumber; 
    int group_size1; //进程数 4(包括主进程)
    int tb1[51][121], tb2[51][121];
    int a[51] = {0,2,3,4,7,8,9,11,13,14,15,17,18,20,23,24,27,28,29,30,32,35,36,39,41,42,44,45,46,47,48,50,52,53,57,58,59,60,61,62,64,65,68,69,73,74,75,78,79,80,81};
    int c[51] = {0,1,3,5,9,11,13,15,17,19,21,23,25,29,31,33,35,39,41,43,45,47,49,51,53,55,59,61,63,65,67,69,71,73,75,77,79,81,83,85,89,91,93,97,99,103,107,111,115,117,121};
    int b = 120; //背包容量
    m = 50; //物品个数     

    for (int i=0; i<=b; i++) {
            tb1[0][i]=0;
            tb2[0][i]=0;
    }
    for (int i=0; i<=m; i++) {
            tb1[i][0]=0;
            tb2[i][0]=0;
    }


    struct timeval tv1,tv2;   
    long t1,t2;

    gettimeofday(&tv1,NULL);
    t1 = tv1.tv_sec*1000 + tv1.tv_usec/1000;


    MPI_Init ( &argc , &argv );
    MPI_Comm_size ( MPI_COMM_WORLD , &group_size1 );
    MPI_Comm_rank ( MPI_COMM_WORLD , &my_rank );  
    MPI_Get_processor_name( name, &len ); 


    if ( group_size1 < 3 ) {//如果进程少于3个,退出并且返回给调用环境一个错误码99
        printf ( "not enough processor!\n" );
        MPI_Abort ( MPI_COMM_WORLD,99 );
    }
    
    pnumber = group_size1 - 1;//副进程数，参与计算的是除了主进程之外的，所以要-1
    
    if (my_rank == 0) { //主进程
        //printf ( "My processor number is %d, as the main processer\n" , my_rank );
        printf("Hello from host %s[%d] %d of %d\n", name, len, my_rank, group_size1);

        printf ( "Knapscack of capacity : %d\n", b);
   
        for (int i=1;i<=pnumber;i++) {//主进程向所有的副进程发送物品的个数
            MPI_Send(&m,1,MPI_INT,i,i,MPI_COMM_WORLD);//(数据的起始地址，变量的个数，变量的类型，接受的进程号，信息标志，通信域)
        }
		
        for (int i=1;i<=pnumber;i++) {//主进程向其他的副进程发送m个整形数据，物品的价值，TAG号是i，即进程号
           MPI_Send(&c[1],m,MPI_INT,i,i,MPI_COMM_WORLD);
        }

        for (int i = 1; i <= pnumber; i++) {//主进程向其他副进程发送物品的价值
            MPI_Send(&a[1],m,MPI_INT,i,i,MPI_COMM_WORLD);
        }
    }

	else if (my_rank == 1) { //进程A
        // printf("My processor number is %d\n", my_rank);
        printf("Hello from host %s[%d] %d of %d\n", name, len, my_rank, group_size1);

        //从主进程接收数据
        MPI_Recv(&m,1,MPI_INT,0,my_rank,MPI_COMM_WORLD,&status);//(数据的起始地址，变量的个数，数据类型，接收数据来源，标志，通信域，&status)
        MPI_Recv(&c[1],m,MPI_INT,0,my_rank,MPI_COMM_WORLD,&status);
        MPI_Recv(&a[1],m,MPI_INT,0,my_rank,MPI_COMM_WORLD,&status);

        for (int j = 1; j <= 120; j++) {
            for (int i = 1; i <= 17; i++) {
                if (((j - a[i]) >= 0) && (tb1[i-1][j] <= tb1[i][j - a[i]] + c[i])) {
                    tb1[i][j] = tb1[i][j - a[i]] + c[i];
                    tb2[i][j] = i;
                } else {
                    tb1[i][j] = tb1[i-1][j];
                    tb2[i][j] = tb2[i-1][j];
                }
            }
            //每处理完一列就向进程B发送数据
            for (int i = 1; i <= 17; i++) {
                MPI_Send(&tb1[i][j],1,MPI_INT,2,i*10+j,MPI_COMM_WORLD);//(数据的起始地址，变量的个数，变量的类型，接受的进程号，信息标志，通信域)
                MPI_Send(&tb2[i][j],1,MPI_INT,2,i*10+j,MPI_COMM_WORLD);//(数据的起始地址，变量的个数，变量的类型，接受的进程号，信息标志，通信域)
            }
        }
    } 
    else if (my_rank == 2) {

        //printf("My processor number is %d\n", my_rank);
        printf("Hello from host %s[%d] %d of %d\n", name, len, my_rank, group_size1);

        //从主进程接收数据
        MPI_Recv(&m,1,MPI_INT,0,my_rank,MPI_COMM_WORLD,&status);//(数据的起始地址，变量的个数，数据类型，接收数据来源，标志，通信域，&status)
        MPI_Recv(&c[1],m,MPI_INT,0,my_rank,MPI_COMM_WORLD,&status);
        MPI_Recv(&a[1],m,MPI_INT,0,my_rank,MPI_COMM_WORLD,&status);

        //从进A接收数据
        for (int j = 1; j <= 120; j++) {
            for (int i = 1; i <= 17; i++) {
                MPI_Recv(&tb1[i][j],1,MPI_INT,1,i*10+j,MPI_COMM_WORLD,&status);
                MPI_Recv(&tb2[i][j],1,MPI_INT,1,i*10+j,MPI_COMM_WORLD,&status);
            }
        }

        for (int j = 1; j <= 120; j++) {
            for (int i = 18; i <= 34; i++) {
                if (((j - a[i]) >= 0) && (tb1[i-1][j] <= tb1[i][j - a[i]] + c[i])) {
                    tb1[i][j] = tb1[i][j - a[i]] + c[i];
                    tb2[i][j] = i;
                } else {
                    tb1[i][j] = tb1[i-1][j];
                    tb2[i][j] = tb2[i-1][j];
                }
            }
            //每处理完一列就向进程C发送数据
            for (int i = 18; i <= 34; i++) {
                MPI_Send(&tb1[i][j],1,MPI_INT,3,i*100+j,MPI_COMM_WORLD);//(数据的起始地址，变量的个数，变量的类型，接受的进程号，信息标志，通信域)
                MPI_Send(&tb2[i][j],1,MPI_INT,3,i*100+j,MPI_COMM_WORLD);//(数据的起始地址，变量的个数，变量的类型，接受的进程号，信息标志，通信域)
            }
        }
    } 
    else if (my_rank == 3) {
        //printf("My processor number is %d\n", my_rank);
        printf("Hello from host %s[%d] %d of %d\n", name, len, my_rank, group_size1);

        //从主进程接收数据
        MPI_Recv(&m,1,MPI_INT,0,my_rank,MPI_COMM_WORLD,&status);//(数据的起始地址，变量的个数，数据类型，接收数据来源，标志，通信域，&status)
        MPI_Recv(&c[1],m,MPI_INT,0,my_rank,MPI_COMM_WORLD,&status);
        MPI_Recv(&a[1],m,MPI_INT,0,my_rank,MPI_COMM_WORLD,&status);

        //从进B接收数据
        for (int j = 1; j <= 120; j++) {
            for (int i = 18; i <= 34; i++) {
                MPI_Recv(&tb1[i][j],1,MPI_INT,2,i*100+j,MPI_COMM_WORLD,&status);
                MPI_Recv(&tb2[i][j],1,MPI_INT,2,i*100+j,MPI_COMM_WORLD,&status);
            }
        }

        for (int j = 1; j <= 120; j++) {
            for (int i = 35; i <= 50; i++) {
                if (((j - a[i]) >= 0) && (tb1[i-1][j] <= tb1[i][j - a[i]] + c[i])) {
                    tb1[i][j] = tb1[i][j - a[i]] + c[i];
                    tb2[i][j] = i;
                } else {
                    tb1[i][j] = tb1[i-1][j];
                    tb2[i][j] = tb2[i-1][j];
                }
            }
        }

    }

    int weight = 120;
    int value[50];
    int count[50];
    int w = 0;
    int total_value = 0;
    value[0] = tb2[50][120];
    while (weight > 0) {
        int value_now = tb2[50][weight];
        if (value_now > 0 ) {
	   
            if (value[w] == value_now) {
                count[w]++;
		weight = weight - a[value[w]];
            } else {
		w++;
		value[w] = value_now;
		count[w]++;
		weight = weight - a[value[w]];
                
            }
        }
    }
   
    printf("===================================result======================================\n");
    printf("Total capacity = %d\n",b);
    for (int i = 0; i <= w; i++) {
        total_value += c[value[i]];
        printf("Product_number:%d  value:%d  weight:%d  count:%d\n", value[i], c[value[i]],  a[value[i]], count[i]);
    }
    printf("Total value = %d\n", total_value);


    gettimeofday(&tv2,NULL);
    t2 = tv2.tv_sec*1000 + tv2.tv_usec/1000;
    printf("\n");
    printf("Total caculate time is = %ldms\n",t2-t1);
	

    MPI_Finalize();

    return 0;
}
