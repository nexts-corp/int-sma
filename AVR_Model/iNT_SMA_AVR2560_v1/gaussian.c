#include <stdlib.h>
#include "debug.h"

// -- for polynomial interpolation by gaussian elimination -- //
int n = 3;
float a[3][4];                                                  

/*============================================================*/
void forwardSubstitution() 
{
    int i, j, k, max;
    float t;
    for (i = 0; i < n; ++i) {   
    
        max = i;
        for (j = i + 1; j < n; ++j)
            if (a[j][i] > a[max][i])
                max = j;

        for (j = 0; j < n + 1; ++j) {
            t = a[max][j];
            a[max][j] = a[i][j];
            a[i][j] = t;
        }
                           
        /*          
        printDebug("\r\n-- fw_sub_swap(%d) --\r\n",i);
        for (k = 0; k < n; ++k) {                   
            printDebug("\t\t\t");
            for (j = 0; j < n + 1; ++j)
                printDebug("%.2f\t", a[k][j]);
            printDebug("\r\n");
        }
        printDebug("\r\n");
        */  
                                                                                              
        //printDebug("\r\n-- fw_sub_calculate(%d) --\r\n",i);
        for (j = n; j >= i; --j){
            for (k = i + 1; k < n; ++k){
                a[k][j] -= a[k][i]/a[i][i] * a[i][j];
                //printDebug("\t\t\t\t\t\t%f (%f/%f * %f)\r\n",a[k][j],a[k][i],a[i][i],a[i][j]); 
            }                                         
        }
        //printDebug("\r\n");  
                            
        
        /*               
        printDebug("\r\n-- fw_sub_res(%d) --\r\n",i);
        for (k = 0; k < n; ++k) {                   
            printDebug("\t\t");
            for (j = 0; j < n + 1; ++j)
                printDebug("%.2f\t", a[k][j]);
            printDebug("\r\n");
        }
        printDebug("\r\n");     
        */
    }
}

void reverseElimination() 
{
    int i, j, k;
    for (i = n - 1; i >= 0; --i) {
        a[i][n] = a[i][n] / a[i][i];
        a[i][i] = 1;
        for (j = i - 1; j >= 0; --j) {
            a[j][n] -= a[j][i] * a[i][n];
            a[j][i] = 0;
        }                  
                                    
        /*
        printDebug("\r\n-- rev_Eli(%d) --\r\n",i);
        for (k = 0; k < n; ++k) {                   
            printDebug("\t\t");
            for (j = 0; j < n + 1; ++j)
                printDebug("%.2f\t", a[k][j]);
            printDebug("\r\n");
        }
        printDebug("\r\n");          
        */
    }
}

void gaussian(float factor1, float factor2, float factor3, float ref1, float ref2, float ref3, float *resA, float *resB, float *resC)
{                                                               
    int i, j;

    // -- form matrix -- //
    a[0][0] = 1.0;      a[0][1] = factor1;     a[0][2] = factor1 * factor1;  a[0][3] = ref1;
    a[1][0] = 1.0;      a[1][1] = factor2;     a[1][2] = factor2 * factor2;  a[1][3] = ref2;
    a[2][0] = 1.0;      a[2][1] = factor3;     a[2][2] = factor3 * factor3;  a[2][3] = ref3; 
                         
                                          
//    printDebug("\r\n<gaussian> --- start ---\r\n");
//    for (i = 0; i < n; ++i) {                   
//        printDebug("\t\t");
//        for (j = 0; j < n + 1; ++j)
//            printDebug("%.2f\t", a[i][j]);
//        printDebug("\r\n");
//    } 
        
    forwardSubstitution();
    reverseElimination();
                                          
//    printDebug("\r\n<gaussian> --- finish ---\r\n");
//    for (i = 0; i < n; ++i) {                   
//        printDebug("\t\t");
//        for (j = 0; j < n + 1; ++j)
//            printDebug("%.2f\t", a[i][j]);
//        printDebug("\r\n");
//    } 
                                        
    *resA = a[2][3];
    *resB = a[1][3];
    *resC = a[0][3];    
                                                                                                           
   // printDebug("\r\n<gaussian> y = %f(x^2) + %f(x) + %f\r\n",*resA,*resB,*resC); 
    
    return;
}