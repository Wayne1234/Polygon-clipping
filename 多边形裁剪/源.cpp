#include<gl/glut.h>
#include<fstream>
#include<iostream>
using namespace std;

const int Num = 100;
int NumOfPoint = 0;
int NewNumOfPoint = 0;
struct point
{
	GLfloat x, y;
}PS[Num];

point winMin = { 200, 200 };
point winMax = { 400, 400 };

void init()
{
	glClearColor(1.0, 1.0, 1.0, 0.0);
	glMatrixMode(GL_PROJECTION);
	gluOrtho2D(0, 600, 0, 600);
}
void getData(){
	ifstream fin("data5.txt");
	if (!fin){
		cerr << "ERROR：文件打开失败" << endl;
		exit(-1);
	}
	int s;
	fin >> s;
	NumOfPoint = s;
	for (int i = 0; i < NumOfPoint; i++){
		fin >> s;
		PS[i].x = s + 300;
		fin >> s;
		PS[i].y = s + 170;
	}
	fin.close();
}

//定义边：winEdge，左：0，上：1，右：2，下：3
point crosspoint(point p1, point p2, int winEdge){
	point p;
	float k;
	if (p1.x != p2.x)
		k = (p1.y - p2.y) / (p1.x - p2.x);
	switch (winEdge){
	case 0:
		p.x = winMin.x;
		p.y = k*(winMin.x - p2.x) + p2.y;
		break;
	case 1:
		p.y = winMax.y;
		if (p1.x != p2.x)
			p.x = p2.x + (winMax.y - p2.y) / k;
		else p.x = p2.x;
		break;
	case 2:
		p.x = winMax.x;
		p.y = p2.y + (winMax.x - p2.x)*k;
		break;
	case 3:
		p.y = winMin.y;
		if (p1.x != p2.x)
			p.x = p2.x + (winMin.y - p2.y) / k;
		else p.x = p2.x;
		break;
	}
	return p;
}

bool inside(point p, int winEdge) {
	switch (winEdge) {
	case 0: { //左边框
				if (p.x<winMin.x)return (false);
				break;
	}
	case 1: { //上边框
				if (p.y>winMax.y) return (false);
				break;
	}
	case 2: { //右边框
				if (p.x>winMax.x) return (false);
				break;
	}
	case 3: { //下边框
				if (p.y<winMin.y) return (false);
				break;
	}
	}
	return (true);
}

void clip(){
	int num0 = 0;
	point LeftSet[Num];
	for (int i = 0; i<NumOfPoint; i++)
	{
		if (!inside(PS[i], 0) && inside(PS[(i + 1) % NumOfPoint], 0))//i在外，i+1在内，保留交点，交点作为下一次迭代的i
		{
			LeftSet[num0] = crosspoint(PS[i], PS[(i + 1) % NumOfPoint], 0);
			num0++;
			LeftSet[num0] = PS[(i + 1) % NumOfPoint];
			num0++;
		}
		if (inside(PS[i], 0) && !inside(PS[(i + 1) % NumOfPoint], 0))//i在内，i+1在外，保留交点即可，i+1仍然是下一次迭代的i
		{
			LeftSet[num0] = crosspoint(PS[i], PS[(i + 1) % NumOfPoint], 0);
			num0++;
		}
		if ((PS[(i + 1) % NumOfPoint].x == winMin.x) ||
			(inside(PS[i], 0) && inside(PS[(i + 1) % NumOfPoint], 0)))//两个都在内部，那么直接保留i+1即可或者只要i+1在边上，那么直接保留i+1即可
		{
			LeftSet[num0] = PS[(i + 1) % NumOfPoint];
			num0++;
		}
	}

	int num1 = 0;
	point UpSet[Num];
	for (int i = 0; i < num0; i++){
		if (!inside(LeftSet[i], 1) && inside(LeftSet[(i + 1) % num0], 1))
		{
			UpSet[num1] = crosspoint(LeftSet[i], LeftSet[(i + 1) % num0], 1);
			num1++;
			UpSet[num1] = LeftSet[(i + 1) % num0];
			num1++;
		}
		if (LeftSet[i].y<winMax.y&&LeftSet[(i + 1) % num0].y>winMax.y)
		{
			UpSet[num1] = crosspoint(LeftSet[i], LeftSet[(i + 1) % num0], 1);
			num1++;
		}
		if ((!inside(LeftSet[i], 1) && LeftSet[(i + 1) % num0].y == winMax.y) || (inside(LeftSet[i], 1) && inside(LeftSet[(i + 1) % num0], 1)))
		{
			UpSet[num1] = LeftSet[(i + 1) % num0];
			num1++;
		}
	}

	int num2 = 0;
	point RightSet[Num];
	for (int i = 0; i<num1; i++)
	{
		if (!inside(UpSet[i], 2) && inside(UpSet[(i + 1) % num1], 2))
		{
			RightSet[num2] = crosspoint(UpSet[i], UpSet[(i + 1) % num1], 2);
			num2++;
			RightSet[num2] = UpSet[(i + 1) % num1];
			num2++;
		}
		if (UpSet[i].x<winMax.x&&UpSet[(i + 1) % num1].x>winMax.x)
		{
			RightSet[num2] = crosspoint(UpSet[i], UpSet[(i + 1) % num1], 2);
			num2++;
		}
		if ((!inside(UpSet[i], 2) && UpSet[(i + 1) % num1].x == winMax.x) || (inside(UpSet[i], 2) && inside(UpSet[(i + 1) % num1], 2)))
		{
			RightSet[num2] = UpSet[(i + 1) % num1];
			num2++;
		}
	}

	//对得到的顶点进行下边界裁剪
	int num3 = 0;
	point DownSet[Num];
	for (int i = 0; i<num2; i++)
	{
		if (!inside(RightSet[i], 3) && inside(RightSet[(i + 1) % num2], 3))
		{
			DownSet[num3] = crosspoint(RightSet[i], RightSet[(i + 1) % num2], 3);
			num3++;
			DownSet[num3] = RightSet[(i + 1) % num2];
			num3++;
		}
		if (RightSet[i].y>winMin.y&&RightSet[(i + 1) % num2].y<winMin.y)
		{
			DownSet[num3] = crosspoint(RightSet[i], RightSet[(i + 1) % num2], 3);
			num3++;
		}
		if ((!inside(RightSet[i], 3) && RightSet[(i + 1) % num2].y == winMin.y) || (inside(RightSet[i], 3) && inside(RightSet[(i + 1) % num2], 3)))
		{
			DownSet[num3] = RightSet[(i + 1) % num2];
			num3++;
		}
	}

	NewNumOfPoint = num3;
	for (int i = 0; i<NewNumOfPoint; i++)
	{
		PS[i].x = (int)DownSet[i].x;
		PS[i].y = (int)DownSet[i].y;
	}
}

void display()
{
	glClear(GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glColor3f(0.0, 0.5, 0.3);
	glPointSize(3.0);
	glBegin(GL_LINE_LOOP);
	glVertex2f(winMin.x, winMin.y);
	glVertex2f(winMax.x, winMin.y);
	glVertex2f(winMax.x, winMax.y);
	glVertex2f(winMin.x, winMax.y);
	glEnd();
	getData();
	glColor3f(0.0, 0.0, 1.0);
	glBegin(GL_LINE_LOOP);
	for (int i = 0; i < NumOfPoint; i++){
		glVertex2f(PS[i].x, PS[i].y);
	}
	glEnd();

	clip();
	glColor3f(1.0, 0.5, 0.3);
	glBegin(GL_LINE_LOOP);
	for (int i = 0; i < NewNumOfPoint; i++){
		glVertex2f(PS[i].x, PS[i].y);
	}
	glEnd();
	glFlush();

}

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowPosition(800, 20);
	glutInitWindowSize(600, 600);
	glutCreateWindow("多边形裁剪");
	init();
	glutDisplayFunc(display);
	glutMainLoop();
	return 0;
}