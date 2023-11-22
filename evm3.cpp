#include <stdio.h>
#include <stdlib.h>
#include <ctime>
#include <windows.h>

const int ARRAYSIZE = 10485760;
const int TIMES = 100;

int a1[ARRAYSIZE];
int a2[ARRAYSIZE];

int resultmmx[ARRAYSIZE];
int resultsmpl[ARRAYSIZE];

void print_array(int* array, const char* array_name, int until)
{
    // для проверки правильности вычислений
    printf("%s = [", array_name);
    for (int i = 0; i < until; i++)
        printf("%d, ",array[i]);
    printf("]\n");
}

void init_arrays()
{
    // заполняем массивы
    srand(time(0));
    for (int i = 0; i < ARRAYSIZE; i++)
    {
        a1[i] = rand();
        a2[i] = rand();
    }
}

void MMXFunc()
{
    // указатели на массивы
    int p1 = (int) & a1[0];
    int p2 = (int) & a2[0];
    int p3 = (int) & resultmmx[0];

    __asm
    {
        // загружаем указатели в регистры
        mov esi, p1
        mov edi, p2

        // в регистре ebx будет смещение по массиву
        mov ebx, 0

        // загружаем в регистр ecx размер массива
        // этот регистр будет служить счётчиком в цикле
        mov ecx, ARRAYSIZE
        shr ecx, 1
        // логический сдвиг вправо нужен, т.к. регистры mmx работают с учетверёнными словами (8 байт)

        __the_loop:; Метка цикла
            // загружаем данные в регистры mmx
            movq mm0, [esi + ebx]; первый массив данных
            movq mm1, [edi + ebx]; второй массив данных
            // складываем значения в регистрах
            paddw mm0, mm1
            // результат операции пойдёт в регистр mm0
            movq[resultmmx + ebx], mm0
            // записываем результат операции в переменную, по смещению ebx (i-й элемент массива)
            add ebx, 8
            // смещаемся
            dec ecx
            // уменьшаем счётчик цикла
            jnz short __the_loop; Если ZeroFlag = 0->выходим из цикла

        emms; очищаем контекст mmx
    }
}

void simple_func()
{
    // реализация сложения массивов на языке высокого уровня
    for (int i = 0; i < ARRAYSIZE; i++)
    {
        resultsmpl[i] = a1[i] + a2[i];
    }
}

void copy_func(void (*function)(void))
{
    for (int i = 0; i < TIMES; i++)
    {
        function();
    }
}

void calc_time(const char* method_name, void(*function)(void))
{
    printf("Метод: %s\n",method_name);

    clock_t Time = clock();
    copy_func(function);

    Time = clock() - Time;
    printf("Время работы = %d тактов\n\n", Time);
}

int main()
{
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);

    init_arrays();

    calc_time("сложение на 'чистом' Си", simple_func);
    calc_time("сложение на MMX", MMXFunc);




    return 0;
}

