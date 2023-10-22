#define CHARSET_SIZE 51
#define SYMBOL_SIZE 6

//Characters in morseset are in the same order as in charset 

//Contains the used character set
char *charset = "ABCDEFGHIJLKMNOPQRSTUVWXYZ1234567890&'@():,=!.-+\"?/";

//Contains the used morse table
//SIGUSR1 = 10 = '.', SIGUSR2 = 12 = '-'
int morseset [CHARSET_SIZE][SYMBOL_SIZE]= {
    {10,12,0,0,0,0},      //A
    {12,10,10,10,0,0},    //B
    {12,10,12,10,0,0},    //C
    {12,10,10,0,0,0},     //D
    {10,0,0,0,0,0},       //E
    {10,10,12,10,0,0},    //F
    {12,12,10,0,0,0},     //G
    {10,10,10,10,0,0},    //H
    {10,10,0,0,0,0},      //I
    {10,12,12,12,0,0},    //J
    {12,10,12,0,0,0},     //k
    {10,12,10,10,0,0},    //L
    {12,12,0,0,0,0},      //M
    {12,10,0,0,0,0},      //N
    {12,12,12,0,0,0},     //O
    {10,12,12,10,0,0},    //P
    {12,12,10,12,0,0},    //Q
    {10,12,10,0,0,0},     //R
    {10,10,10,0,0,0},     //S
    {12,0,0,0,0,0},       //T
    {10,10,12,0,0,0},     //U
    {10,10,10,12,0,0},    //V
    {10,12,12,0,0,0},     //W
    {12,10,10,12,0,0},    //X
    {12,10,12,12,0,0},    //Y
    {12,12,10,10,0,0},    //Z
    {10,12,12,12,12,0},   //1
    {10,10,12,12,12,0},   //2
    {10,10,10,12,12,0},   //3
    {10,10,10,10,12,0},   //4
    {10,10,10,10,10,0},   //5
    {12,10,10,10,10,0},   //6
    {12,12,10,10,10,0},   //7
    {12,12,12,10,10,0},   //8
    {12,12,12,12,10,0},   //9
    {12,12,12,12,12,0},   //0
    {10,12,10,10,10,0},   //&
    {10,12,12,12,12,10},  //'
    {10,12,12,10,12,10},  //@
    {12,10,12,12,10,0},   //(
    {12,10,12,12,10,12},  //)
    {12,12,12,10,10,10},  //:
    {12,12,10,10,12,12},  //,
    {12,10,10,10,12,0},   //=
    {12,10,12,10,12,12},  //!
    {10,12,10,12,10,12},  //.
    {12,10,10,10,10,12},  //-
    {10,12,10,12,10,0},   //+
    {10,12,10,10,12,10},  //"
    {10,10,12,12,10,10},  //?
    {12,10,10,12,10,0},   // /
    };




