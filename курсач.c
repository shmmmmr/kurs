#define _CRT_SECURE_NO_WARNINGS
#include<stdlib.h>
#include<windows.h>
#include<glut.h>
#include<stdio.h>
#include<time.h>
#include<math.h>
#include"line.h"

#ifdef _UNICODE
#define T_STRING(x) L##x
#else
#define T_STRING(x) x
#endif

clock_t start1;
clock_t start;
int visible[25][25];//проверка состояния клетки, 0-клетка серая, закрыта, 1-белая клетка, 2-флаг  
int arr[25][25];//наличие бомбы
int out = 0, bombCount;//аут - подорвались на бомбе 
int mouseVisit = 0, ent = 0;
int a[2];//хранит две координаты клетки(posi, posj)
int over = 0;//окончание игры
int size_field; //10 - easy, 20 - medium,  25 - hard
int is_bomb = -2;//для бота, есть ли бомба, когда нажата другая клавиша
int bots_answers[25][25];

void Init()
{
    glClearColor(0.72, 0.72, 0.73, 0.0); //цвет поля где выбор режима
    glEnable(GL_DEPTH);
    glShadeModel(GL_SMOOTH);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, 500, 0, 600, 0, 500);
    for (int i = 0; i < size_field; i++) {
        for (int j = 0; j < size_field; j++) {
            bots_answers[i][j] = 0;
        }
    }
}


void bitmap(float x, float y, void* font, char* string) //для отображения текстовых строк в заданных координатах
{

    char* c;
    glRasterPos2f(x, y); //место, где начнется отображение текста
    for (c = string; *c != '\0'; c++) //проходимся по каждому символу текста
    {
        glutBitmapCharacter(font, *c); //функция чтобы отобразить символ
    }
}



void wait() //задержка выполнения программы на пол секунды 
{
    start1 = clock();
    while (((double)clock() - start1) / CLOCKS_PER_SEC < 0.5)
    {
    }
}


void draw(int x, int y) //рисунок флага
{
    glColor3f(0, 0, 0);
    glPointSize(3);
    glBegin(GL_POINTS);
    bresen(x + (500 / size_field) / 5, y, x + (500 / size_field) / 5, (500 / size_field) / 2 + y);
    glEnd();

    glColor3f(1, 0, 0);
    glBegin(GL_POLYGON);
    if (size_field == 10) {
        glVertex2f(x + 10, y + 15);
        glVertex2f(x + 10, y + 28);
        glVertex2f(x - 10, y + 20);
    }
    else if (size_field == 20) {
        glVertex2f(x + (500 / size_field) / 5, y + 7);
        glVertex2f(x + (500 / size_field) / 5, y + 14);
        glVertex2f(x - (500 / size_field) / 5, y + 11);
    }
    else {
        glVertex2f(x + (500 / size_field) / 5, y + 4);
        glVertex2f(x + (500 / size_field) / 5, y + 10);
        glVertex2f(x - (500 / size_field) / 5, y + 7);
    }
    
    glEnd();
    glColor3f(1, 1, 1);

}


void fill(int arr[25][25], int bomb[25][2], int noBomb) //заполняем числовой информацией
{
    int i, j, k;
    for (i = 0; i < size_field; i++) //проходим по каждой клетке
        for (j = 0; j < size_field; j++)
        {
            if (arr[i][j] != -1) //если нет бомбы 
            {
                int a = 0; // тогда для данной клетки считаем сколько рядом бомб
                if (i - 1 >= 0 && j - 1 >= 0 && arr[i - 1][j - 1] == -1)
                    a++;
                if (i - 1 >= 0 && arr[i - 1][j] == -1)
                    a++;
                if (i - 1 >= 0 && j + 1 < size_field && arr[i - 1][j + 1] == -1)
                    a++;
                if (j - 1 >= 0 && arr[i][j - 1] == -1)
                    a++;
                if (j + 1 < size_field && arr[i][j + 1] == -1)
                    a++;
                if (i + 1 < size_field && j - 1 >= 0 && arr[i + 1][j - 1] == -1)
                    a++;
                if (i + 1 < size_field && arr[i + 1][j] == -1)
                    a++;
                if (i + 1 < size_field && j + 1 < size_field && arr[i + 1][j + 1] == -1)
                    a++;
                arr[i][j] = a;

            }
        }
}


void spread(int i, int j) //функция открытия пустых клеток
{
    if (i >= 0 && j >= 0 && i < size_field && j < size_field) //мы находимся в пределах игрового поля
    {
        if (arr[i][j] != -1 && visible[i][j] != 1) //если нет бомбы и эта клетка еще не открыта
        {
            visible[i][j] = 1; //делаем ее открытой
            if (arr[i][j] == 0) //если в клетке нет числа, то функция рекурсивно вызывает саму себя для всех восьми смежных клеток
            {
                spread(i - 1, j - 1);
                spread(i - 1, j);
                spread(i - 1, j + 1);
                spread(i, j - 1);
                spread(i, j + 1);
                spread(i + 1, j - 1);
                spread(i + 1, j);
                spread(i + 1, j + 1);
            }
        }
        else;
    }
    else;
}


int finish()//проверка открытости всех клеток без бомб
{
    int i, j;
    for (i = 0; i < size_field; i++) //проходимся по всем клеткам поля 
        for (j = 0; j < size_field; j++)
        {
            if ((arr[i][j] != -1 && visible[i][j] == 0) || (arr[i][j] != -1 && visible[i][j] == 2)) 
                return 0; //игра не завершена 
        }
    return 1; //все клетки без бомб открыты
}


void outline() // Делаем сетку, разбиваем на ячейки
{
    for (int i = 0; i <= size_field; i++) {
        bresen(i * 500 / size_field, 0, i * 500 / size_field, 500);
    }
    for (int i = 0; i <= size_field; i++) {
        bresen(0, i * 500 / size_field, 500, i * 500 / size_field);
    }
}


void bomb(int x, int y) //рисунок бомбы
{
    glColor3f(0, 0, 0);
    glBegin(GL_POLYGON);
    circle((500 / size_field) / 10 * 3, x, y);
    glEnd();

    glColor3f(0, 0, 0);
    glBegin(GL_POLYGON);
    glVertex2f(x - (500 / size_field) / 10, y + (500 / size_field) / 5 + 5);
    glVertex2f(x + (500 / size_field) / 10, y + (500 / size_field) / 5 + 5);
    glVertex2f(x, y + (500 / size_field) / 5 * 2);
    glEnd();
    glBegin(GL_POLYGON);
    glVertex2f(x - (500 / size_field) / 5 + 5, y - (500 / size_field) / 10);
    glVertex2f(x - (500 / size_field) / 5 + 5, y + (500 / size_field) / 10);
    glVertex2f(x - (500 / size_field) / 5 * 2, y);
    glEnd();
    glBegin(GL_POLYGON);
    glVertex2f(x - (500 / size_field) / 10, y - (500 / size_field) / 5 + 5);
    glVertex2f(x + (500 / size_field) / 10, y - (500 / size_field) / 5 + 5);
    glVertex2f(x, y - (500 / size_field) / 5 * 2);
    glEnd();
    glBegin(GL_POLYGON);
    glVertex2f(x + (500 / size_field) / 5 + 5, y - (500 / size_field) / 10);
    glVertex2f(x + (500 / size_field) / 5 + 5, y + (500 / size_field) / 10);
    glVertex2f(x + (500 / size_field) / 5 * 2, y);
    glEnd();
}


void setbomb(void) //случайная установка бомб
{
    int bomb[100][2]; //содержит координаты установленных бомб
    int r, i, j, count, k;
    srand(time(NULL));//чтобы бомбы не перерисовывались, были зафиксированы в течение одной игры
    for (count = 0; count < bombCount; count++)
    {
    manb:
        r = rand() % (size_field * size_field);//получение рандомной координаты, чтобы не было выхода за пределы поля
        i = r / size_field; // i для хранения значения строки (вертикальной координаты) выбранной клетки
        j = r % size_field;//горизонтальная клетка
        for (k = 0; k <= count - 1; k++)
        {
            if (i == bomb[k][0] && j == bomb[k][1]) //проверяется, не совпадают ли полученные координаты с какой-либо уже существующей бомбой
                goto manb; //выходим на повторную генерацию
            else continue;
        }

        bomb[count][0] = i; //сохраняем координаты установленной бомбы
        bomb[count][1] = j;

    }

    for (k = 0; k < bombCount; k++)
        arr[bomb[k][0]][bomb[k][1]] = -1; //отмечаем эту клетку, как ту, на которой стоит мина 
    fill(arr, bomb, bombCount);

}

void help_bot(int i, int j) {
    is_bomb = 0;
    if (visible[i][j] != 1) {//ячейка не была открыта   й
        int n, new_j, new_i, free = 0;
        int add[3] = { -1, 0, 1 };
        for (int y = 0; y <= 2; y++) {
            for (int u = 0; u <= 2; u++) {
                new_j = j + add[u];
                new_i = i + add[y];
                if (arr[i + add[y]][j + add[u]] >= 1 && visible[i + add[y]][j + add[u]] == 1 && arr[new_i][new_j] != 0 && !(add[y] == 0 && add[u] == 0)) {
                    n = arr[new_i][new_j];
                    for (int h = 0; h <= 2; h++) {
                        for (int g = 0; g <= 2; g++) {
                            if (add[h] == 0 && add[g] == 0) continue;
                            if (visible[new_i + add[h]][new_j + add[g]] == 1) free++;
                            if (bots_answers[new_i + add[h]][new_j + add[g]] == 1) n--;
                        }
                    }
                    if (8 - free == n) {
                        bots_answers[i][j] = 1;//точно бомба
                    }
                    if (n == 0) {
                        for (int h = 0; h <= 2; h++) {
                            for (int g = 0; g <= 2; g++) {
                                if (visible[new_i + add[h]][new_j + add[g]] == 0 && bots_answers[new_i + add[h]][new_j + add[g]] != 1) bots_answers[new_i + add[h]][new_j + add[g]] = -1;//точно не бомба
                            }
                        }
                    }
                    free = 0;
                }
            }
        }
    }
}



void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT); //для очистки буфера цвета перед рисованием нового изображения, чтобы избежать размытости или "остаточных" изображений прошлых кадров
    glClearColor(0.72, 0.72, 0.73, 0.0);
    if (is_bomb == 1) {//бомба есть
        glColor3f(1, 1, 1); //линии на поле игры 
        bitmap(10, 530, GLUT_BITMAP_9_BY_15, "The bomb is here");
        glPointSize(3);
        glBegin(GL_POINTS);
        outline();
        glEnd();
    }
    else if (is_bomb == 0) {
        glColor3f(1, 1, 1); //линии на поле игры 
        bitmap(10, 530, GLUT_BITMAP_9_BY_15, "Probably the bomb is here");
        glPointSize(3);
        glBegin(GL_POINTS);
        outline();
        glEnd();
    }
    else if (is_bomb == -1) {
        glColor3f(1, 1, 1); //линии на поле игры 
        bitmap(10, 530, GLUT_BITMAP_9_BY_15, "The bomb isn't here");
        glPointSize(3);
        glBegin(GL_POINTS);
        outline();
        glEnd();
    }
    if (out) //подорвались на бомбе
    {
        if (!over) //over==1
        {
            LPCTSTR pszSound1 = T_STRING("C:\\Users\\Регина\\Downloads\\SOUNDm.wav");
            BOOL result1 = PlaySound(pszSound1, NULL, SND_FILENAME | SND_ASYNC);
            glColor3f(1, 1, 1); //белые линии разделений
            int i, j;
            for (i = 0; i < size_field; i++)
                for (j = 0; j < size_field; j++)
                {
                    if (visible[i][j] == 1) //клетка открыта
                    {
                        glColor3f(1, 1, 1);
                        glBegin(GL_POLYGON);
                        glVertex2f(j * (500 / size_field) + (500 / size_field) / 10, 500 - (500 / size_field) - i * (500 / size_field) + (500 / size_field) / 10);
                        glVertex2f(j * (500 / size_field) + (500 / size_field) * 0.9, 500 - (500 / size_field) - i * (500 / size_field) + (500 / size_field) / 10);
                        glVertex2f(j * (500 / size_field) + (500 / size_field) * 0.9, 500 - (500 / size_field) - i * (500 / size_field) + (500 / size_field) * 0.9);
                        glVertex2f(j * (500 / size_field) + (500 / size_field) / 10, 500 - (500 / size_field) - i * (500 / size_field) + (500 / size_field) * 0.9);
                        glEnd();
                        glColor3f(1, 1, 1);
                        if (arr[i][j] != 0)
                        {
                            char str[5];
                            _itoa(arr[i][j], str, 10);
                            bitmap(j * (500 / size_field) + (500 / size_field) / 5 * 2, 500 - (500 / size_field) - i * (500 / size_field) + (500 / size_field) / 5 * 2, GLUT_BITMAP_9_BY_15, str);
                        }
                    }
                }
            glColor3f(1, 1, 1);
            glPointSize(3);
            glBegin(GL_POINTS);
            outline();
            glEnd();
            bomb(a[1] * (500 / size_field) + (500 / size_field) / 2, 500 - (500 / size_field) - a[0] * (500 / size_field) + (500 / size_field) / 2);
            glFlush();
            wait();
            for (i = 0; i < size_field; i++)
                for (j = 0; j < size_field; j++)
                    if (arr[i][j] == -1)
                        bomb(j * (500 / size_field) + (500 / size_field) / 2, 500 - (500 / size_field) - i * (500 / size_field) + (500 / size_field) / 2);
            //отрисовка всех остальных бомб
            glFlush();
            wait();
        }
        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(0.3, 0.3, 1, 0);
        glColor3f(1, 1, 1);
        bitmap(200, 300, GLUT_BITMAP_9_BY_15, "YOU LOSE");
        over = 1;
    }

    else //открыли ячейку без бомбы
    {
        if (!over)
        {
            glColor3f(1, 1, 1); //линии на поле игры 

            bitmap(10, 550, GLUT_BITMAP_9_BY_15, "Time:");
            float time = ((double)clock() - start) / CLOCKS_PER_SEC;
            char str[100000];
            _itoa(time, str, 10);
            bitmap(200, 550, GLUT_BITMAP_9_BY_15, str);


            glPointSize(3);
            glBegin(GL_POINTS);
            outline();
            glEnd();
            int i, j;
            for (i = 0; i < size_field; i++)
                for (j = 0; j < size_field; j++)
                {
                    if (visible[i][j] == 1) //нет флага, рисуем белую клетку, клетка открыта
                    {
                        glColor3f(1, 1, 1);
                        glBegin(GL_POLYGON);
                        glVertex2f(j * (500 / size_field) + (500 / size_field) / 10, 500 - (500 / size_field) - i * (500 / size_field) + (500 / size_field) / 10);
                        glVertex2f(j * (500 / size_field) + (500 / size_field) * 0.9, 500 - (500 / size_field) - i * (500 / size_field) + (500 / size_field) / 10);
                        glVertex2f(j * (500 / size_field) + (500 / size_field) * 0.9, 500 - (500 / size_field) - i * (500 / size_field) + (500 / size_field) * 0.9);
                        glVertex2f(j * (500 / size_field) + (500 / size_field) / 10, 500 - (500 / size_field) - i * (500 / size_field) + (500 / size_field) * 0.9);
                        glEnd();
                        glColor3f(0, 0, 0);
                        if (arr[i][j] != 0)
                        {
                            if (arr[i][j] == 1)
                                glColor3f(0, 0, 1); // синий
                            else if (arr[i][j] == 2)
                                glColor3f(0, 1, 0); // зелёный
                            else if (arr[i][j] == 3)
                                glColor3f(1, 0, 0); // красный
                            else
                                glColor3f(0, 0, 0); // чёрный

                            char str[5];//рисует цифру
                            _itoa(arr[i][j], str, 10);
                            bitmap(j * (500 / size_field) + (500 / size_field) / 5 * 2, 500 - (500 / size_field) - i * (500 / size_field) + (500 / size_field) / 5 * 2, GLUT_BITMAP_9_BY_15, str);
                        }
                    }
                    else if (visible[i][j] == 2)//есть флаг, клетка закрыта
                    {
                        draw(j * (500 / size_field) + (500 / size_field) / 5 * 2, 500 - (500 / size_field) - i * (500 / size_field) + (500 / size_field) / 5);
                    }
                }
            glFlush();
        }
        if (finish())
        {
            wait();
            glClear(GL_COLOR_BUFFER_BIT);
            glClearColor(0.3, 0.3, 1, 0);
            glColor3f(1, 1, 1);
            bitmap(200, 300, GLUT_BITMAP_9_BY_15, "YOU WON");
            over = 1;
        }
    }
    glFlush();

}


void mouse2(int mouse, int state, int x, int y)
{
    //что то было нажато, то гото мен, т.е ничего не делаем
    if (!ent) goto man;//ент проверяет не обработано ли нажатие мыши, чтобы не началась обработка другого действия, когда еще не завершилось старое
    mouseVisit++;//второе нажатие кнопки убирает флаг
    int posi, posj;
    posj = x / (500 / size_field);
    if (y >= 100)
        posi = (y - 100) / (500 / size_field);
    else goto wrngpos;//отрисовка дисплея
    if (visible[posi][posj] == 1) //если ячейка открыта
        goto man;
    if (mouse == 2) //нажата правая кнопка мыши
    {
        is_bomb = -2;//убирается надпись о наличии бомбы, нажатие любой другой клавишей кроме колеса
        if (visible[posi][posj] == 2 && mouseVisit % 2 == 1)//если уже стоит флаг
            visible[posi][posj] = 0;
        else if (mouseVisit % 2 == 1) visible[posi][posj] = 2;
    }
    else if (mouse == 1) {//проверка нажатия колесика мышки
        help_bot(posi, posj);
        is_bomb = bots_answers[posi][posj];
    }
    else if (mouse == 0)
    {
        is_bomb = -2;
        if (arr[posi][posj] == -1) //если нажали на бомбу
        {
            out = 1; a[0] = posi; a[1] = posj;
        }
        else if (arr[posi][posj] == 0) // если ячейка пустая 
            spread(posi, posj);
        else
            visible[posi][posj] = 1;// эта ячейка с числом, открываем его
    }
man:
wrngpos:
    ent = 1;
    glutDisplayFunc(display);
    glutPostRedisplay();
}


void update(int value) //циклическое обновление экрана игры с определенной задержкой, для обновления графики игры
{
    glutDisplayFunc(display); //отрисовка display
    glutPostRedisplay();
    glutTimerFunc(1000, update, value);
}


void starting(void) //экран, где выбор сложности
{
    glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(0.72, 0.72, 0.73);

    glColor3f(0, 0, 0);
    bitmap(180, 506, GLUT_BITMAP_9_BY_15, "Select a mode:");
    bitmap(210, 416, GLUT_BITMAP_9_BY_15, "1.Easy");
    bitmap(200, 316, GLUT_BITMAP_9_BY_15, "2.Medium");
    bitmap(210, 216, GLUT_BITMAP_9_BY_15, "3.Hard");
    glFlush();
}


void mouse1(int mouse, int state, int x, int y) // выбор уровня сложности игры
{

    if (x >= 150 && x <= 350 && y >= 150 && y <= 200)
    {
        size_field = 10;
        bombCount = 8;
        setbomb();
        start = clock();
        glutDisplayFunc(display);
        glutMouseFunc(mouse2);
        glutTimerFunc(500, update, 0);
        glutPostRedisplay();
    }
    else if (x >= 150 && x <= 350 && y >= 250 && y <= 300)
    {
        size_field = 20;
        bombCount = 40;
        setbomb();
        start = clock();
        glutDisplayFunc(display);
        glutMouseFunc(mouse2);
        glutTimerFunc(500, update, 0);
        glutPostRedisplay();
    }
    else if (x >= 150 && x <= 350 && y >= 350 && y <= 400)
    {
        size_field = 25;
        bombCount = 90;
        setbomb();
        start = clock();
        glutDisplayFunc(display);
        glutMouseFunc(mouse2);
        glutTimerFunc(500, update, 0);
        glutPostRedisplay();
    }
    else
    {
        glutDisplayFunc(starting);
        glutPostRedisplay();
    }
}




int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_SINGLE | GLUT_RGBA);
    glutInitWindowPosition(250, 100);
    glutInitWindowSize(500, 600);
    glutCreateWindow("Minesweep");
    Init();
    glutDisplayFunc(starting);
    glutMouseFunc(mouse1);
    glutMainLoop();
    return 0;
}
