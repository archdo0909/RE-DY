// Project1.cpp : Defines the entry point for the console application.
//

#include <stdio.h>
#include <Windows.h>
#include <stdlib.h>
#include <gl/glut.h>
#include <math.h>
#include <string.h>
#include <vector>

#define judge 0.0001
#define e_judge 0.000000001
#define side_num 6
//#define node_Num_m 11
//#define node_Num_n 21
int node_Num_m[2];
int node_Num_n[2];
//int node_Num_m;
//int node_Num_n;
bool close_flag = false;
bool open_flag = false;
bool close_flag_n = false;
bool open_flag_n = false;
double c[3];
int window_size_x = 1000;
int window_size_y = window_size_x;
GLfloat orange[] = { 255.0 / 256.0, 153.0 / 256.0, 0.0 / 256.0, 0.9 };
GLfloat blue[] = { 0.0 / 256.0, 65.0 / 256.0, 255.0 / 256.0, 0.4 };
GLfloat white[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat blue2[] = { 102.0 / 256.0, 204.0 / 256.0, 255.0 / 256.0, 0.9 };
GLfloat blue_node[] = { 0.5, 0.5, 1.0, 1.0 };
bool up_flag = false;
bool down_flag = false;
int w_view;
int h_view;
int first_count = 1;
double wall_z = 0.5;
double wall_n = 5.0;
int num_count = 0;
int con_count = 0;
int tri_count = 0;
double damp_k = 1000.0;
double damp_k_normal = 20;
double dv = 3.0;
double node_Radius = 0.08;
double View_from[3] = { 0.0, 25.0, 13.0 };
double View_to[3] = { 0.0, -15.0, 0.0 };
double View_from2[3] = { 0.0, 13.0, 0.01 };
double View_to2[3] = { 0.0, -10.0, 0.0 };
double View_from3[3] = { 0.0, 13.0, 0.01 };
double View_to3[3] = { 0.0, -10.0, 0.0 };
bool MouseFlagRight = false;
bool MouseFlagLeft = false;
bool MouseFlagMiddle = false;
bool View_point_flag = false;
GLUnurbsObj *theNurb;
typedef struct{
	double x[3];
}position;
typedef struct{
	int number;
	int node_Num_w;
	int node_Num_h;
}face;
typedef struct{
	int t[3];
	int color;
	double normal[3];
	double A;
	double total;
}triangle_d;
typedef struct{
	bool torf;
	double len;
	//int color;
}edge_d;
typedef struct{
	int number;
	int edge_flag;			//ノード番号
	int none_flag;
	int dup_flag;
	int sin_flag;
	face face;
	position pos;		//位置
}node;
typedef struct{
	int number;
	int edge_flag;			//ノード番号
	int none_flag;
	int N[6];
	int T[6];
	double m_normal[3];
	double cosa[6];
	double cosb[6];
	double cota[6];
	double cotb[6];
	double K;
	position pos;		//位置
	position del_pos;	//速度
	position acc;		//加速度
	double color_grad;
}node2;
typedef struct{
	int number;
	position pos;
	face face;
}point;
static node2 node_surface2[10000];
static node node_surface[500][500][2];
static edge_d edge[5000][5000];
static face face_info[3];
static point node_point[3][4]; //node_point[face Num][coordinate Num]
static triangle_d triangle_data[10000];
void get_info(){
	int i;
	int j;
	printf("    1...........2       \n ");
	printf("    :         :       \n ");
	printf("    :         :       \n ");
	printf("   4:.........:3      \n ");

	for (i = 0; i < 2; i++){
		for (j = 0; j < 4; j++){
			printf("Put Coordinate %d from face %d\n", j + 1, i + 1);
			scanf_s("%lf, %lf", &c[0], &c[1]);
			node_point[i][j].pos.x[0] = c[0];
			node_point[i][j].pos.x[1] = c[1];
			node_point[i][j].face.number = i;

		}
	}
	printf("Put division for face \n");
	//scanf_s("%d, %d", &node_Num_m, &node_Num_n);
}
void info(){
	node_point[1][0].pos.x[0] = 0.0;
	node_point[1][0].pos.x[1] = 0.0;
	node_point[1][1].pos.x[0] = 3.5;
	node_point[1][1].pos.x[1] = 0.0;
	node_point[1][2].pos.x[1] = 7.0;
	node_point[0][0].pos.x[0] = 0.0;
	node_point[0][0].pos.x[1] = 0.0;
	node_point[0][1].pos.x[0] = 3.5;
	node_point[0][1].pos.x[1] = 0;
	node_point[0][2].pos.x[1] = 7.0;
	node_Num_m[0] = 11;
	node_Num_n[0] = 21;
	node_Num_m[1] = 11;
	node_Num_n[1] = 21;
}
void mult_matrix3x1(double *c, double *a, double *b){
	for (int i = 0; i < 3; i++){
		//for(int j=0;j<3;j++){
		c[i] = 0.0;
		for (int k = 0; k < 3; k++){
			c[i] += a[3 * i + k] * b[k];
		}
		//}
	}
}
void gaiseki_9_3(double *a, double *b){
	a[0] = 0.0;
	a[1] = -b[2];
	a[2] = b[1];
	a[3] = b[2];
	a[4] = 0.0;
	a[5] = -b[0];
	a[6] = -b[1];
	a[7] = b[0];
	a[8] = 0.0;
}
void sphere(double R, double precise, GLfloat sph_col[10]){

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, sph_col);
	GLUquadricObj *sphere;

	sphere = gluNewQuadric();
	gluQuadricDrawStyle(sphere, GLU_FILL);

	gluSphere(sphere, R, precise, precise);
}
void View_control(bool vector_flag){
	double View_distance;
	double temp[5];
	temp[2] = View_from[2] - View_to[2];
	temp[1] = View_from[0] - View_to[0];
	temp[0] = pow(temp[1], 2.0) + pow(temp[2], 2.0);
	View_distance = pow(temp[0], 0.5);
	//	printf("%f\n", View_distance);
	temp[0] = View_from[2] - View_to[2];
	temp[3] = temp[0] / View_distance;
	temp[1] = View_from[0] - View_to[0];
	temp[4] = temp[1] / View_distance;
	temp[2] = atan2(temp[4], temp[3]);
	//temp[2] = acos(temp[1]);
	if (vector_flag) temp[2] = temp[2] + 0.01;
	else temp[2] = temp[2] - 0.01;
	temp[0] = View_distance * cos(temp[2]);
	temp[1] = View_distance * sin(temp[2]);
	View_from[2] = View_to[2] + temp[0];
	View_from[0] = View_to[0] + temp[1];
}
void View_control_up_down(bool vector_flag){
	double View_distance;
	double temp[5];
	temp[2] = View_from[1] - View_to[1];
	temp[1] = View_from[0] - View_to[0];
	temp[0] = pow(temp[1], 2.0) + pow(temp[2], 2.0);
	View_distance = pow(temp[0], 0.5);
	//	printf("%f\n", View_distance);
	temp[0] = View_from[1] - View_to[1];
	temp[3] = temp[0] / View_distance;
	temp[1] = View_from[0] - View_to[0];
	temp[4] = temp[1] / View_distance;
	temp[2] = atan2(temp[4], temp[3]);
	//temp[2] = acos(temp[1]);
	if (vector_flag) temp[2] = temp[2] + 0.01;
	else temp[2] = temp[2] - 0.01;
	temp[0] = View_distance * cos(temp[2]);
	temp[1] = View_distance * sin(temp[2]);
	View_from[1] = View_to[1] + temp[0];
	View_from[0] = View_to[0] + temp[1];
}
void View_control2(bool vector_flag){
	double View_distance;
	double temp[5];
	temp[2] = View_from2[2] - View_to2[2];
	temp[1] = View_from2[0] - View_to2[0];
	temp[0] = pow(temp[1], 2.0) + pow(temp[2], 2.0);
	View_distance = pow(temp[0], 0.5);
	temp[0] = View_from2[2] - View_to2[2];
	temp[3] = temp[0] / View_distance;
	temp[1] = View_from2[0] - View_to2[0];
	temp[4] = temp[1] / View_distance;
	temp[2] = atan2(temp[4], temp[3]);
	if (vector_flag) temp[2] = temp[2] + 0.01;
	else temp[2] = temp[2] - 0.01;
	temp[0] = View_distance * cos(temp[2]);
	temp[1] = View_distance * sin(temp[2]);
	View_from2[2] = View_to2[2] + temp[0];
	View_from2[0] = View_to2[0] + temp[1];
}
void initiation(){

	int i = 0;
	int j = 0;
	int k = 0;
	int h = 0;
	int s = 0;
	int c = 0;
	int trirem1[3];
	int trirem2[3];
	int trirem3[3];
	int trirem4[3];
	int trirem5[3];
	int flag = 0;
	int tri_flag1;
	int tri_flag2;
	int tri_flag3;
	int tri_flag4;
	int tri_flag5;
	double tritemp_x;
	double tritemp_y;
	double natural_length_x[2];
	double natural_length_y[2];
	double natural_length = 1.0;




	/*node_point[1][0].pos.x[0] = 0.035;
	node_point[1][0].pos.x[1] = 0.035;
	node_point[1][1].pos.x[0] = 3.465;
	node_point[1][1].pos.x[1] = 0.035;
	node_point[1][2].pos.x[1] = 6.965;
	node_point[0][0].pos.x[0] = 0.0;
	node_point[0][0].pos.x[1] = 0.0;
	node_point[0][1].pos.x[0] = 3.5;
	node_point[0][1].pos.x[1] = 0;
	node_point[0][2].pos.x[1] = 7.0;
	node_Num_m[0] = 11;
	node_Num_n[0] = 21;
	node_Num_m[1] = 9;
	node_Num_n[1] = 19;*/

	/*for (s = 0; s < 2; s++){
	natural_length_x[s] = fabs((double)(node_point[s][1].pos.x[0] - node_point[s][0].pos.x[0]) / (double)(node_Num_m - 1));
	natural_length_y[s] = fabs((double)(node_point[s][2].pos.x[1] - node_point[s][1].pos.x[1]) / (double)(node_Num_n - 1));
	}*/

	///rectangle
	//for (s = 0; s < 2; s++){
	//	for (i = 0; i < node_Num_m[s]; i++){
	//		for (j = 0; j < node_Num_n[s]; j++){
	//				//if (i == 0 || j == 0 || i == node_Num_m[s] - 1 || j == node_Num_n[s] - 1){
	//				//	if (s == 0){
	//				//		node_surface[i][j][s].edge_flag = 1;
	//				//		node_surface[i][j][s].none_flag = 0;
	//				//	}
	//				//	else{
	//				//		node_surface[i][j][s].edge_flag = 0;
	//				//		node_surface[i][j][s].none_flag = 1;
	//				//	}
	//			//	}
	//		//	else{
	//			node_surface[i][j][s].edge_flag = 0;
	//			node_surface[i][j][s].none_flag = 0;
	//			node_surface[i][j][s].pos.x[0] = (double)(node_point[s][0].pos.x[0] + (double)(i * natural_length_x[s]));
	//			node_surface[i][j][s].pos.x[1] = (double)(node_point[s][0].pos.x[1] + (double)(j * natural_length_y[s]));
	//			node_surface[i][j][s].pos.x[2] = (double)0.0;
	//		}
	//	}
	//}
	//printf("node_point[1][0] = %f\n", node_point[0][0].pos.x[0]);
	//printf("node_surface[i][j][s] = %f, %f, %f\n", node_surface[0][0][0].pos.x[0], node_surface[0][0][0].pos.x[1], node_surface[0][0][0].pos.x[0]);
	///triangle node position
#if 1
	for (s = 0; s < 2; s++){
		for (i = 0; i < side_num; i++){
			for (j = 0; j <= i; j++){
				if (i % 2 == 0){
					node_surface[i][j][s].pos.x[0] = (double)((0.0) + (j - (i / 2))*natural_length);
				    //printf("x = %f, i = %d, j = %d\n", node_surface[i][j][s].pos.x[0], i, j);
				}
				else{
					node_surface[i][j][s].pos.x[0] = (double)(((-i * 0.5) + j)*natural_length);
					//printf("x = %f, i = %d, j = %d\n", node_surface[i][j][s].pos.x[0], i, j);
				}
				node_surface[i][j][s].pos.x[1] = (double)(i * sqrt(3) * 0.5);
				node_surface[i][j][s].pos.x[2] = (double)(0.0);
			}
		}
	}
#endif
	//sealing edge
#if 1
	for (s = 0; s < 2; s++){
		for (i = 0; i < side_num; i++){
			for (j = 0; j <= i; j++){
				if (j == 0 || j == i || i == side_num - 1){
					node_surface[i][j][s].edge_flag = 1;
					node_surface[i][j][s].none_flag = 0;
				}
				else{
					node_surface[i][j][s].edge_flag = 0;
					node_surface[i][j][s].none_flag = 1;
				}
			}
		}
	}
#endif
	//그냥 오리지널 버전으로 했을때 삼각형
#if 0
	for (s = 0; s < 2; s++){
		for (i = 0; i < 5; i++){
			for (j = 0; j <= i; j++){
				if (j == 0 || j == i || i == 4){
					if (s == 0){
						node_surface[i][j][s].edge_flag = 1;
						node_surface[i][j][s].none_flag = 0;
					}
					else{
						node_surface[i][j][s].edge_flag = 0;
						node_surface[i][j][s].none_flag = 1;
					}
				}
				else{
					node_surface[i][j][s].edge_flag = 0;
					node_surface[i][j][s].none_flag = 0;
				}
				if (i % 2 == 0){
					node_surface[i][j][s].pos.x[0] = (double)((0.0) + (j - (i / 2))*natural_length);
					//printf("x = %f, i = %d, j = %d\n", node_surface[i][j][s].pos.x[0], i, j);
				}
				else{
					node_surface[i][j][s].pos.x[0] = (double)(((-i * 0.5) + j)*natural_length);
					//printf("x = %f, i = %d, j = %d\n", node_surface[i][j][s].pos.x[0], i, j);
				}
				node_surface[i][j][s].pos.x[1] = (double)(i * sqrt(3) * 0.5);
				node_surface[i][j][s].pos.x[2] = (double)(0.0);
			}
		}
	}
#endif
	for (s = 0; s < 2; s++){
		for (i = 0; i < side_num; i++){
			for (j = 0; j <= i; j++){
				node_surface[i][j][s].number = num_count;
				num_count++;
			}
		}
	}
	printf("num_count = %d\n", num_count);
	////////////////////////////////////////////////////////////////////////////////////////////
	//////// Mesh
	///////////////////////////////////////////////////////////////////////////////////////////
	//*****************************************************************************************************
	//表
#if 0
	for (h = 0; h < num_count; h++){
		tri_flag3 = 0;
		if (h < 15){
			for (i = 0; i < 5; i++){
				for (j = 0; j <= i; j++){
					for (s = 0; s < 1; s++){
						if (node_surface[i][j][s].number == h && node_surface[i][j][s].none_flag != 1){
							trirem1[0] = i;
							trirem1[1] = j;
							trirem1[2] = s;
							tri_flag3 = 1;
							printf("trirem1 = %d, %d, %d\n", trirem1[0], trirem1[1], trirem1[2]);
						}
					}
				}
			}
			tri_flag1 = 0;
			tri_flag2 = 0;
			tri_flag4 = 0;
			tri_flag5 = 0;
			for (i = 0; i < 5; i++){
				for (j = 0; j <= i; j++){
					for (s = 0; s < 1; s++){
						if (node_surface[i][j][s].none_flag != 1){
							tritemp_x = node_surface[trirem1[0]][trirem1[1]][trirem1[2]].pos.x[0] - node_surface[i][j][s].pos.x[0];
							tritemp_y = node_surface[trirem1[0]][trirem1[1]][trirem1[2]].pos.x[1] - node_surface[i][j][s].pos.x[1];
							if (fabs(tritemp_x + natural_length * 0.5) < judge && fabs(tritemp_y + (sqrt(3) * natural_length * 0.5)) < judge){
								trirem2[0] = i;
								trirem2[1] = j;
								trirem2[2] = s;
								tri_flag1 = 1;
							}
							if (fabs(tritemp_x - natural_length * 0.5) < judge && fabs(tritemp_y + (sqrt(3) * natural_length * 0.5)) < judge){
								trirem3[0] = i;
								trirem3[1] = j;
								trirem3[2] = s;
								tri_flag2 = 1;
							}
							if (fabs(tritemp_x + natural_length) < judge && fabs(tritemp_y) < judge){
								trirem4[0] = i;
								trirem4[1] = j;
								trirem4[2] = s;
								tri_flag4 = 1;
							}
							if (fabs(tritemp_x + natural_length * 0.5) < judge && fabs(tritemp_y + (sqrt(3) * natural_length * 0.5)) < judge){
								trirem5[0] = i;
								trirem5[1] = j;
								trirem5[2] = s;
								tri_flag5 = 1;
							}
						}
					}
				}
			}
			if (tri_flag1 == 1 && tri_flag3 == 1 && tri_flag2 == 1){
				if (tri_count == 0){
					triangle_data[tri_count].t[0] = h;
					triangle_data[tri_count].t[1] = node_surface[trirem2[0]][trirem2[1]][trirem2[2]].number;
					triangle_data[tri_count].t[2] = node_surface[trirem3[0]][trirem3[1]][trirem3[2]].number;
					triangle_data[tri_count].color = 1;
					tri_count++;
				}
				else{
					flag = 0;
					for (i = 0; i < tri_count; i++){
						if ((triangle_data[i].t[0] == h) && (triangle_data[i].t[1] == node_surface[trirem2[0]][trirem2[1]][trirem2[2]].number)
							&& (triangle_data[i].t[2] = node_surface[trirem3[0]][trirem3[1]][trirem3[2]].number)){
							flag = 1;
						}
					}
					if (flag == 0){
						triangle_data[tri_count].t[0] = h;
						triangle_data[tri_count].t[1] = node_surface[trirem2[0]][trirem2[1]][trirem2[2]].number;
						triangle_data[tri_count].t[2] = node_surface[trirem3[0]][trirem3[1]][trirem3[2]].number;
						triangle_data[tri_count].color = 1;
						tri_count++;
					}
					else{
						flag = 0;
					}
				}
			}
			if (tri_flag3 == 1 && tri_flag5 == 1 && tri_flag4 == 1){
				if (tri_count == 0){
					triangle_data[tri_count].t[0] = h;
					triangle_data[tri_count].t[1] = node_surface[trirem4[0]][trirem4[1]][trirem4[2]].number;
					triangle_data[tri_count].t[2] = node_surface[trirem5[0]][trirem5[1]][trirem5[2]].number;
					triangle_data[tri_count].color = 1;
					tri_count++;
				}
				else{
					flag = 0;
					for (i = 0; i < tri_count; i++){
						if ((triangle_data[i].t[0] == h) && (triangle_data[i].t[1] == node_surface[trirem5[0]][trirem5[1]][trirem5[2]].number)
							&& (triangle_data[i].t[2] == node_surface[trirem4[0]][trirem4[1]][trirem4[2]].number)){
							flag = 1;
						}
					}
					if (flag == 0){
						triangle_data[tri_count].t[0] = h;
						triangle_data[tri_count].t[1] = node_surface[trirem4[0]][trirem4[1]][trirem4[2]].number;
						triangle_data[tri_count].t[2] = node_surface[trirem5[0]][trirem5[1]][trirem5[2]].number;
						triangle_data[tri_count].color = 1;
						tri_count++;
					}
					else{
						flag = 0;
					}
				}
			}
		}
	}

#endif
	//*****************************************************************************************************
	//裏
	//*****************************************************************************************************
#if 0
	for (h = 0; h < num_count; h++){
		tri_flag3 = 0;
		if (h >= 15){
			for (i = 0; i < 5; i++){
				for (j = 0; j <= i; j++){
					for (s = 1; s < 2; s++){
						if (node_surface[i][j][s].number == h && node_surface[i][j][s].none_flag != 1){
							trirem1[0] = i;
							trirem1[1] = j;
							trirem1[2] = s;
							tri_flag3 = 1;
						}
					}
				}
			}
			tri_flag1 = 0;
			tri_flag2 = 0;
			tri_flag4 = 0;
			tri_flag5 = 0;
			for (i = 0; i < 5; i++){
				for (j = 0; j <= i; j++){
					for (s = 0; s < 2; s++){
						if (node_surface[i][j][s].none_flag != 1){
							if (node_surface[i][j][s].edge_flag == 1 || s == 1){
								tritemp_x = node_surface[trirem1[0]][trirem1[1]][trirem1[2]].pos.x[0] - node_surface[i][j][s].pos.x[0];
								tritemp_y = node_surface[trirem1[0]][trirem1[1]][trirem1[2]].pos.x[1] - node_surface[i][j][s].pos.x[1];
								if (fabs(tritemp_x - natural_length * 0.5) < judge && fabs(tritemp_y + (sqrt(3) * natural_length * 0.5)) < judge){
									trirem2[0] = i;
									trirem2[1] = j;
									trirem2[2] = s;
									tri_flag1 = 1;
								}
								if (fabs(tritemp_x + natural_length * 0.5) < judge && fabs(tritemp_y + (sqrt(3) * natural_length * 0.5)) < judge){
									trirem3[0] = i;
									trirem3[1] = j;
									trirem3[2] = s;
									tri_flag2 = 1;
								}
								if (fabs(tritemp_x + natural_length * 0.5) < judge && fabs(tritemp_y + (sqrt(3) * natural_length * 0.5)) < judge){
									trirem4[0] = i;
									trirem4[1] = j;
									trirem4[2] = s;
									tri_flag4 = 1;
								}
								if (fabs(tritemp_x + natural_length) < judge && fabs(tritemp_y) < judge){
									trirem5[0] = i;
									trirem5[1] = j;
									trirem5[2] = s;
									tri_flag5 = 1;
								}
							}
						}
					}
				}
			}
			if (tri_flag1 == 1 && tri_flag3 == 1 && tri_flag2 == 1){
				if (tri_count == 0){
					triangle_data[tri_count].t[0] = h;
					triangle_data[tri_count].t[1] = node_surface[trirem2[0]][trirem2[1]][trirem2[2]].number;
					triangle_data[tri_count].t[2] = node_surface[trirem3[0]][trirem3[1]][trirem3[2]].number;
					triangle_data[tri_count].color = 1;
					tri_count++;
				}
				else{
					flag = 0;
					for (i = 0; i < tri_count; i++){
						if ((triangle_data[i].t[0] == h) && (triangle_data[i].t[1] == node_surface[trirem2[0]][trirem2[1]][trirem2[2]].number)
							&& (triangle_data[i].t[2] = node_surface[trirem3[0]][trirem3[1]][trirem3[2]].number)){
							flag = 1;
						}
					}
					if (flag == 0){
						triangle_data[tri_count].t[0] = h;
						triangle_data[tri_count].t[1] = node_surface[trirem2[0]][trirem2[1]][trirem2[2]].number;
						triangle_data[tri_count].t[2] = node_surface[trirem3[0]][trirem3[1]][trirem3[2]].number;
						triangle_data[tri_count].color = 1;
						tri_count++;
					}
					else{
						flag = 0;
					}
				}
			}
			if (tri_flag3 == 1 && tri_flag5 == 1 && tri_flag4 == 1){
				if (tri_count == 0){
					triangle_data[tri_count].t[0] = h;
					triangle_data[tri_count].t[1] = node_surface[trirem4[0]][trirem4[1]][trirem4[2]].number;
					triangle_data[tri_count].t[2] = node_surface[trirem5[0]][trirem5[1]][trirem5[2]].number;
					triangle_data[tri_count].color = 1;
					tri_count++;
				}
				else{
					flag = 0;
					for (i = 0; i < tri_count; i++){
						if ((triangle_data[i].t[0] == h) && (triangle_data[i].t[1] == node_surface[trirem5[0]][trirem5[1]][trirem5[2]].number)
							&& (triangle_data[i].t[2] == node_surface[trirem4[0]][trirem4[1]][trirem4[2]].number)){
							flag = 1;
						}
					}
					if (flag == 0){
						triangle_data[tri_count].t[0] = h;
						triangle_data[tri_count].t[1] = node_surface[trirem4[0]][trirem4[1]][trirem4[2]].number;
						triangle_data[tri_count].t[2] = node_surface[trirem5[0]][trirem5[1]][trirem5[2]].number;
						triangle_data[tri_count].color = 1;
						tri_count++;
					}
					else{
						flag = 0;
					}
				}
			}
		}
	}
#endif

	//puttng mesh on rectangle
#if 0
	for (h = 0; h < num_count; h++){
		tri_flag3 = 0;
		for (s = 0; s < 2; s++){
			if (h >= s * node_Num_m[s] * node_Num_n[s] && h < (s + 1) * node_Num_m[s] * node_Num_n[s]){
				for (i = 0; i <= (node_Num_m[s] - 1) / 2; i++){
					for (j = 0; j <= (node_Num_n[s] - 1) / 2; j++){
						if (node_surface[i][j][s].number == h && node_surface[i][j][s].none_flag != 1){
							//if (node_surface[i][j][s].number == h){
							trirem1[0] = i;
							trirem1[1] = j;
							trirem1[2] = s;
							tri_flag3 = 1;
						}
					}
				}
				//printf("trirem1[0]=%d\n", trirem1[0]);
				tri_flag1 = 0;
				tri_flag2 = 0;
				tri_flag4 = 0;
				tri_flag5 = 0;
				//for (s = 0; s < 2; s++){
				for (i = 0; i <= (node_Num_m[s] - 1) / 2; i++){
					for (j = 0; j <= (node_Num_n[s] - 1) / 2; j++){
						if (node_surface[i][j][s].none_flag != 1){
							tritemp_x = node_surface[trirem1[0]][trirem1[1]][trirem1[2]].pos.x[0] - node_surface[i][j][s].pos.x[0];
							tritemp_y = node_surface[trirem1[0]][trirem1[1]][trirem1[2]].pos.x[1] - node_surface[i][j][s].pos.x[1];
							if (fabs(tritemp_x + natural_length_x[s]) < judge && fabs(tritemp_y) < judge){
								trirem2[0] = i;
								trirem2[1] = j;
								trirem2[2] = s;
								tri_flag1 = 1;
							}
							if (fabs(tritemp_x + natural_length_x[s]) < judge && fabs(tritemp_y + natural_length_y[s]) < judge){
								trirem3[0] = i;
								trirem3[1] = j;
								trirem3[2] = s;
								tri_flag2 = 1;
							}
							if (fabs(tritemp_x + natural_length_x[s]) < judge && fabs(tritemp_y + natural_length_y[s]) < judge){
								trirem4[0] = i;
								trirem4[1] = j;
								trirem4[2] = s;
								tri_flag4 = 1;
							}
							if (fabs(tritemp_x) < judge && fabs(tritemp_y + natural_length_y[s]) < judge){
								trirem5[0] = i;
								trirem5[1] = j;
								trirem5[2] = s;
								tri_flag5 = 1;
							}
						}
					}
				}
				//}
				if (tri_flag1 == 1 && tri_flag2 == 1 && tri_flag3 == 1){
					if (tri_count == 0){
						triangle_data[tri_count].t[0] = h;
						triangle_data[tri_count].t[1] = node_surface[trirem2[0]][trirem2[1]][trirem2[2]].number;
						triangle_data[tri_count].t[2] = node_surface[trirem3[0]][trirem3[1]][trirem3[2]].number;
						triangle_data[tri_count].color = 1;
						tri_count++;
					}
					else{
						flag = 0;
						for (i = 0; i < tri_count; i++){
							if ((triangle_data[i].t[0] == h) && (triangle_data[i].t[1] == node_surface[trirem2[0]][trirem2[1]][trirem2[2]].number)
								&& (triangle_data[i].t[2] = node_surface[trirem3[0]][trirem3[1]][trirem3[2]].number)){
								flag = 1;
							}
							//printf("a\n");
						}
						if (flag == 0){
							triangle_data[tri_count].t[0] = h;
							triangle_data[tri_count].t[1] = node_surface[trirem2[0]][trirem2[1]][trirem2[2]].number;
							triangle_data[tri_count].t[2] = node_surface[trirem3[0]][trirem3[1]][trirem3[2]].number;
							triangle_data[tri_count].color = 1;
							tri_count++;
						}
						else{
							flag = 0;
						}
					}
				}
				if (tri_flag4 == 1 && tri_flag5 == 1 && tri_flag3 == 1){
					if (tri_count == 0){
						triangle_data[tri_count].t[0] = h;
						triangle_data[tri_count].t[1] = node_surface[trirem4[0]][trirem4[1]][trirem4[2]].number;
						triangle_data[tri_count].t[2] = node_surface[trirem5[0]][trirem5[1]][trirem5[2]].number;
						triangle_data[tri_count].color = 1;
						tri_count++;
					}
					else{
						flag = 0;
						for (i = 0; i < tri_count; i++){
							if ((triangle_data[i].t[0] == h) && (triangle_data[i].t[1] == node_surface[trirem4[0]][trirem4[1]][trirem4[2]].number)
								&& (triangle_data[i].t[2] = node_surface[trirem5[0]][trirem5[1]][trirem5[2]].number)){
								flag = 1;
							}
							//printf("a\n");
						}
						if (flag == 0){
							triangle_data[tri_count].t[0] = h;
							triangle_data[tri_count].t[1] = node_surface[trirem4[0]][trirem4[1]][trirem4[2]].number;
							triangle_data[tri_count].t[2] = node_surface[trirem5[0]][trirem5[1]][trirem5[2]].number;
							triangle_data[tri_count].color = 1;
							tri_count++;
						}
						else{
							flag = 0;
						}
					}
				}
			}
		}
	}





	//for (s = 0; s < 2; s++){
	//	upperAndleft(s);
	//}

	//右上
	for (h = 0; h < num_count; h++){
		tri_flag3 = 0;
		for (s = 0; s < 2; s++){
			if (h >= s * node_Num_m[s] * node_Num_n[s] && h < (s + 1) * node_Num_m[s] * node_Num_n[s]){
				for (i = (node_Num_m[s] - 1) / 2; i <= node_Num_m[s]; i++){
					for (j = 0; j <= (node_Num_n[s] - 1) / 2; j++){
						if (node_surface[i][j][s].number == h && node_surface[i][j][s].none_flag != 1){
							//if (node_surface[i][j][s].number == h){
							trirem1[0] = i;
							trirem1[1] = j;
							trirem1[2] = s;
							tri_flag3 = 1;
						}
					}
				}
				//printf("trirem1[0]=%d\n", trirem1[0]);
				tri_flag1 = 0;
				tri_flag2 = 0;
				tri_flag4 = 0;
				tri_flag5 = 0;
				for (i = (node_Num_m[s] - 1) / 2; i <= node_Num_m[s]; i++){
					for (j = 0; j <= (node_Num_n[s] - 1) / 2; j++){
						//for (s = 0; s < 1; s++){
						if (node_surface[i][j][s].none_flag != 1){
							tritemp_x = node_surface[trirem1[0]][trirem1[1]][trirem1[2]].pos.x[0] - node_surface[i][j][s].pos.x[0];
							tritemp_y = node_surface[trirem1[0]][trirem1[1]][trirem1[2]].pos.x[1] - node_surface[i][j][s].pos.x[1];
							if (fabs(tritemp_x + natural_length_x[s]) < judge && fabs(tritemp_y) < judge){
								trirem2[0] = i;
								trirem2[1] = j;
								trirem2[2] = s;
								tri_flag1 = 1;
							}
							if (fabs(tritemp_x) < judge && fabs(tritemp_y + natural_length_y[s]) < judge){
								trirem3[0] = i;
								trirem3[1] = j;
								trirem3[2] = s;
								tri_flag2 = 1;
							}
							if (fabs(tritemp_x - natural_length_x[s]) < judge && fabs(tritemp_y) < judge){
								trirem4[0] = i;
								trirem4[1] = j;
								trirem4[2] = s;
								tri_flag4 = 1;
							}
							if (fabs(tritemp_x) < judge && fabs(tritemp_y - natural_length_y[s]) < judge){
								trirem5[0] = i;
								trirem5[1] = j;
								trirem5[2] = s;
								tri_flag5 = 1;
							}
						}
					}
				}
				if (tri_flag1 == 1 && tri_flag2 == 1 && tri_flag3 == 1){
					flag = 0;
					for (i = 0; i < tri_count; i++){
						if ((triangle_data[i].t[0] == h) && (triangle_data[i].t[1] == node_surface[trirem2[0]][trirem2[1]][trirem2[2]].number)
							&& (triangle_data[i].t[2] = node_surface[trirem3[0]][trirem3[1]][trirem3[2]].number)){
							flag = 1;
						}
						//printf("a\n");
					}
					if (flag == 0){
						triangle_data[tri_count].t[0] = h;
						triangle_data[tri_count].t[1] = node_surface[trirem2[0]][trirem2[1]][trirem2[2]].number;
						triangle_data[tri_count].t[2] = node_surface[trirem3[0]][trirem3[1]][trirem3[2]].number;
						triangle_data[tri_count].color = 1;
						tri_count++;
					}
					else{
						flag = 0;
					}
				}
				if (tri_flag4 == 1 && tri_flag5 == 1 && tri_flag3 == 1){
					flag = 0;
					for (i = 0; i < tri_count; i++){
						if ((triangle_data[i].t[0] == h) && (triangle_data[i].t[1] == node_surface[trirem4[0]][trirem4[1]][trirem4[2]].number)
							&& (triangle_data[i].t[2] = node_surface[trirem5[0]][trirem5[1]][trirem5[2]].number)){
							flag = 1;
						}
						//printf("a\n");
					}
					if (flag == 0){
						triangle_data[tri_count].t[0] = h;
						triangle_data[tri_count].t[1] = node_surface[trirem4[0]][trirem4[1]][trirem4[2]].number;
						triangle_data[tri_count].t[2] = node_surface[trirem5[0]][trirem5[1]][trirem5[2]].number;
						triangle_data[tri_count].color = 1;
						tri_count++;
					}
					else{
						flag = 0;
					}
				}
			}
		}
	}
	//左下
	for (h = 0; h < num_count; h++){
		tri_flag3 = 0;
		for (s = 0; s < 2; s++){
			if (h >= s * node_Num_m[s] * node_Num_n[s] && h < (s + 1) * node_Num_m[s] * node_Num_n[s]){
				for (i = 0; i <= (node_Num_m[s] - 1) / 2; i++){
					for (j = (node_Num_n[s] - 1) / 2; j <= node_Num_n[s]; j++){
						if (node_surface[i][j][s].number == h && node_surface[i][j][s].none_flag != 1){
							//if (node_surface[i][j][s].number == h){
							trirem1[0] = i;
							trirem1[1] = j;
							trirem1[2] = s;
							tri_flag3 = 1;
						}
					}
				}
				tri_flag1 = 0;
				tri_flag2 = 0;
				tri_flag4 = 0;
				tri_flag5 = 0;
				for (i = 0; i <= (node_Num_m[s] - 1) / 2; i++){
					for (j = (node_Num_n[s] - 1) / 2; j <= node_Num_n[s]; j++){
						//for (s = 0; s < 1; s++){
						if (node_surface[i][j][s].none_flag != 1){
							tritemp_x = node_surface[trirem1[0]][trirem1[1]][trirem1[2]].pos.x[0] - node_surface[i][j][s].pos.x[0];
							tritemp_y = node_surface[trirem1[0]][trirem1[1]][trirem1[2]].pos.x[1] - node_surface[i][j][s].pos.x[1];
							if (fabs(tritemp_x + natural_length_x[s]) < judge && fabs(tritemp_y) < judge){
								trirem2[0] = i;
								trirem2[1] = j;
								trirem2[2] = s;
								tri_flag1 = 1;
							}
							if (fabs(tritemp_x) < judge && fabs(tritemp_y + natural_length_y[s]) < judge){
								trirem3[0] = i;
								trirem3[1] = j;
								trirem3[2] = s;
								tri_flag2 = 1;
							}
							if (fabs(tritemp_x - natural_length_x[s]) < judge && fabs(tritemp_y) < judge){
								trirem4[0] = i;
								trirem4[1] = j;
								trirem4[2] = s;
								tri_flag4 = 1;
							}
							if (fabs(tritemp_x) < judge && fabs(tritemp_y - natural_length_y[s]) < judge){
								trirem5[0] = i;
								trirem5[1] = j;
								trirem5[2] = s;
								tri_flag5 = 1;
							}
						}
					}
				}
				if (tri_flag1 == 1 && tri_flag2 == 1 && tri_flag3 == 1){
					if (tri_count == 0){
						triangle_data[tri_count].t[0] = h;
						triangle_data[tri_count].t[1] = node_surface[trirem2[0]][trirem2[1]][trirem2[2]].number;
						triangle_data[tri_count].t[2] = node_surface[trirem3[0]][trirem3[1]][trirem3[2]].number;
						triangle_data[tri_count].color = 1;
						tri_count++;
					}
					else{
						flag = 0;
						for (i = 0; i < tri_count; i++){
							if ((triangle_data[i].t[0] == h) && (triangle_data[i].t[1] == node_surface[trirem2[0]][trirem2[1]][trirem2[2]].number)
								&& (triangle_data[i].t[2] = node_surface[trirem3[0]][trirem3[1]][trirem3[2]].number)){
								flag = 1;
							}
							//printf("a\n");
						}
						if (flag == 0){
							triangle_data[tri_count].t[0] = h;
							triangle_data[tri_count].t[1] = node_surface[trirem2[0]][trirem2[1]][trirem2[2]].number;
							triangle_data[tri_count].t[2] = node_surface[trirem3[0]][trirem3[1]][trirem3[2]].number;
							triangle_data[tri_count].color = 1;
							tri_count++;
						}
						else{
							flag = 0;
						}
					}
				}
				if (tri_flag4 == 1 && tri_flag5 == 1 && tri_flag3 == 1){
					if (tri_count == 0){
						triangle_data[tri_count].t[0] = h;
						triangle_data[tri_count].t[1] = node_surface[trirem4[0]][trirem4[1]][trirem4[2]].number;
						triangle_data[tri_count].t[2] = node_surface[trirem5[0]][trirem5[1]][trirem5[2]].number;
						triangle_data[tri_count].color = 1;
						tri_count++;
					}
					else{
						flag = 0;
						for (i = 0; i < tri_count; i++){
							if ((triangle_data[i].t[0] == h) && (triangle_data[i].t[1] == node_surface[trirem4[0]][trirem4[1]][trirem4[2]].number)
								&& (triangle_data[i].t[2] = node_surface[trirem5[0]][trirem5[1]][trirem5[2]].number)){
								flag = 1;
							}
							//printf("a\n");
						}
						if (flag == 0){
							triangle_data[tri_count].t[0] = h;
							triangle_data[tri_count].t[1] = node_surface[trirem4[0]][trirem4[1]][trirem4[2]].number;
							triangle_data[tri_count].t[2] = node_surface[trirem5[0]][trirem5[1]][trirem5[2]].number;
							triangle_data[tri_count].color = 1;
							tri_count++;
						}
						else{
							flag = 0;
						}
					}
				}
			}
		}
	}
	//右下
	for (h = 0; h < num_count; h++){
		tri_flag3 = 0;
		for (s = 0; s < 2; s++){
			if (h >= s * node_Num_m[s] * node_Num_n[s] && h < (s + 1) * node_Num_m[s] * node_Num_n[s]){
				for (i = (node_Num_m[s] - 1) / 2; i <= node_Num_m[s]; i++){
					for (j = (node_Num_n[s] - 1) / 2; j <= node_Num_n[s]; j++){
						if (node_surface[i][j][s].number == h && node_surface[i][j][s].none_flag != 1){
							//if (node_surface[i][j][s].number == h){
							trirem1[0] = i;
							trirem1[1] = j;
							trirem1[2] = s;
							tri_flag3 = 1;
						}
					}
				}
				tri_flag1 = 0;
				tri_flag2 = 0;
				tri_flag4 = 0;
				tri_flag5 = 0;
				for (i = (node_Num_m[s] - 1) / 2; i <= node_Num_m[s]; i++){
					for (j = (node_Num_n[s] - 1) / 2; j <= node_Num_n[s]; j++){
						//for (s = 0; s < 1; s++){
						if (node_surface[i][j][s].none_flag != 1){
							tritemp_x = node_surface[trirem1[0]][trirem1[1]][trirem1[2]].pos.x[0] - node_surface[i][j][s].pos.x[0];
							tritemp_y = node_surface[trirem1[0]][trirem1[1]][trirem1[2]].pos.x[1] - node_surface[i][j][s].pos.x[1];
							if (fabs(tritemp_x + natural_length_x[s]) < judge && fabs(tritemp_y) < judge){
								trirem2[0] = i;
								trirem2[1] = j;
								trirem2[2] = s;
								tri_flag1 = 1;
							}
							if (fabs(tritemp_x + natural_length_x[s]) < judge && fabs(tritemp_y + natural_length_y[s]) < judge){
								trirem3[0] = i;
								trirem3[1] = j;
								trirem3[2] = s;
								tri_flag2 = 1;
							}
							if (fabs(tritemp_x + natural_length_x[s]) < judge && fabs(tritemp_y + natural_length_y[s]) < judge){
								trirem4[0] = i;
								trirem4[1] = j;
								trirem4[2] = s;
								tri_flag4 = 1;
							}
							if (fabs(tritemp_x) < judge && fabs(tritemp_y + natural_length_y[s]) < judge){
								trirem5[0] = i;
								trirem5[1] = j;
								trirem5[2] = s;
								tri_flag5 = 1;
							}
						}
					}
				}
				if (tri_flag1 == 1 && tri_flag2 == 1 && tri_flag3 == 1){
					flag = 0;
					for (i = 0; i < tri_count; i++){
						if ((triangle_data[i].t[0] == h) && (triangle_data[i].t[1] == node_surface[trirem2[0]][trirem2[1]][trirem2[2]].number)
							&& (triangle_data[i].t[2] = node_surface[trirem3[0]][trirem3[1]][trirem3[2]].number)){
							flag = 1;
						}
						//printf("a\n");
					}
					if (flag == 0){
						triangle_data[tri_count].t[0] = h;
						triangle_data[tri_count].t[1] = node_surface[trirem2[0]][trirem2[1]][trirem2[2]].number;
						triangle_data[tri_count].t[2] = node_surface[trirem3[0]][trirem3[1]][trirem3[2]].number;
						triangle_data[tri_count].color = 1;
						tri_count++;
					}
					else{
						flag = 0;
					}
				}
				if (tri_flag4 == 1 && tri_flag5 == 1 && tri_flag3 == 1){
					flag = 0;
					for (i = 0; i < tri_count; i++){
						if ((triangle_data[i].t[0] == h) && (triangle_data[i].t[1] == node_surface[trirem4[0]][trirem4[1]][trirem4[2]].number)
							&& (triangle_data[i].t[2] = node_surface[trirem5[0]][trirem5[1]][trirem5[2]].number)){
							flag = 1;
						}
					}
					if (flag == 0){
						triangle_data[tri_count].t[0] = h;
						triangle_data[tri_count].t[1] = node_surface[trirem4[0]][trirem4[1]][trirem4[2]].number;
						triangle_data[tri_count].t[2] = node_surface[trirem5[0]][trirem5[1]][trirem5[2]].number;
						triangle_data[tri_count].color = 1;
						tri_count++;
					}
					else{
						flag = 0;
					}
				}
			}
		}
	}
	printf("tri_count = %d\n", tri_count);

#endif

#if 0
	//ノードに重複するかしないか区別		
	for (h = 0; h < num_count - 1; h++){
		for (s = 0; s < 2; s++){
			for (i = 0; i < node_Num_m[s]; i++){
				for (j = 0; j < node_Num_n[s]; j++){
					if (s = 0){
						node_surface[i][j][s].dup_flag = 0;
						node_surface[i][j][s].sin_flag = 1;
					}
					else{
						if (i == 0 || j == 0 || i == node_Num_m[s] - 1 || j == node_Num_n[s] - 1)
						{
							node_surface[i][j][s].dup_flag = 1;
							node_surface[i][j][s].sin_flag = 0;
						}
						else{
							node_surface[i][j][s].dup_flag = 0;
							node_surface[i][j][s].sin_flag = 1;
						}
					}
				}
			}
		}
	}
#endif
	//inset to node_surface2 for rectangle
#if 0	
	for (s = 0; s < 2; s++){
		for (i = 0; i < node_Num_m[s]; i++){
			for (j = 0; j <= i; j++){
				//	if (node_surface[i][j][s].none_flag != 1){
				for (h = 0; h < 3; h++){
					node_surface2[node_surface[i][j][s].number].pos.x[h] = node_surface[i][j][s].pos.x[h];
					//		}
				}
			}
		}
	}
#endif
	//put meshes on triangle
#if 1
	for (h = 0; h < num_count; h++){
		for (s = 0; s < 2; s++){
			tri_flag3 = 0;
			for (i = 0; i < side_num; i++){
				for (j = 0; j <= i; j++){
					if (node_surface[i][j][s].number == h){
						trirem1[0] = i;
						trirem1[1] = j;
						trirem1[2] = s;
						tri_flag3 = 1;
					}
				}
			}
			tri_flag1 = 0;
			tri_flag2 = 0;
			tri_flag4 = 0;
			tri_flag5 = 0;
			for (i = 0; i < side_num; i++){
				for (j = 0; j <= i; j++){
					tritemp_x = node_surface[trirem1[0]][trirem1[1]][trirem1[2]].pos.x[0] - node_surface[i][j][s].pos.x[0];
					tritemp_y = node_surface[trirem1[0]][trirem1[1]][trirem1[2]].pos.x[1] - node_surface[i][j][s].pos.x[1];
					if (fabs(tritemp_x + natural_length * 0.5) < judge && fabs(tritemp_y + (sqrt(3) * natural_length * 0.5)) < judge){
						trirem2[0] = i;
						trirem2[1] = j;
						trirem2[2] = s;
						tri_flag1 = 1;
					}
					if (fabs(tritemp_x - natural_length * 0.5) < judge && fabs(tritemp_y + (sqrt(3) * natural_length * 0.5)) < judge){
						trirem3[0] = i;
						trirem3[1] = j;
						trirem3[2] = s;
						tri_flag2 = 1;
					}
					if (fabs(tritemp_x + natural_length) < judge && fabs(tritemp_y) < judge){
						trirem4[0] = i;
						trirem4[1] = j;
						trirem4[2] = s;
						tri_flag4 = 1;
					}
					if (fabs(tritemp_x + natural_length * 0.5) < judge && fabs(tritemp_y + (sqrt(3) * natural_length * 0.5)) < judge){
						trirem5[0] = i;
						trirem5[1] = j;
						trirem5[2] = s;
						tri_flag5 = 1;
					}
				}
			}
			if (tri_flag1 == 1 && tri_flag2 == 1 && tri_flag3 == 1){
				if (tri_count == 0){
					triangle_data[tri_count].t[0] = h;
					triangle_data[tri_count].t[1] = node_surface[trirem2[0]][trirem2[1]][trirem2[2]].number;
					triangle_data[tri_count].t[2] = node_surface[trirem3[0]][trirem3[1]][trirem3[2]].number;
					triangle_data[tri_count].color = 1;
					tri_count++;
				}
				else{
					flag = 0;
					for (i = 0; i < tri_count; i++){
						if ((triangle_data[i].t[0] == h) && (triangle_data[i].t[1] == node_surface[trirem2[0]][trirem2[1]][trirem2[2]].number)
							&& (triangle_data[i].t[2] = node_surface[trirem3[0]][trirem3[1]][trirem3[2]].number)){
							flag = 1;
							//printf("tri_data = %d, %d, %d\n", triangle_data[tri_count].t[0], triangle_data[tri_count].t[1], triangle_data[tri_count].t[2]);
						}
					}
					if (flag == 0){
						triangle_data[tri_count].t[0] = h;
						triangle_data[tri_count].t[1] = node_surface[trirem2[0]][trirem2[1]][trirem2[2]].number;
						triangle_data[tri_count].t[2] = node_surface[trirem3[0]][trirem3[1]][trirem3[2]].number;
						triangle_data[tri_count].color = 1;
						tri_count++;
					}
					else{
						flag = 0;
					}
				}
			}
			if (tri_flag3 == 1 && tri_flag5 == 1 && tri_flag4 == 1){
				if (tri_count == 0){
					triangle_data[tri_count].t[0] = h;
					triangle_data[tri_count].t[1] = node_surface[trirem4[0]][trirem4[1]][trirem4[2]].number;
					triangle_data[tri_count].t[2] = node_surface[trirem5[0]][trirem5[1]][trirem5[2]].number;
					triangle_data[tri_count].color = 1;
					tri_count++;
				}
				else{
					flag = 0;
					for (i = 0; i < tri_count; i++){
						if ((triangle_data[i].t[0] == h) && (triangle_data[i].t[1] == node_surface[trirem4[0]][trirem4[1]][trirem4[2]].number)
							&& (triangle_data[i].t[2] == node_surface[trirem5[0]][trirem5[1]][trirem5[2]].number)){
							flag = 1;
						}
					}
					if (flag == 0){
						triangle_data[tri_count].t[0] = h;
						triangle_data[tri_count].t[1] = node_surface[trirem4[0]][trirem4[1]][trirem4[2]].number;
						triangle_data[tri_count].t[2] = node_surface[trirem5[0]][trirem5[1]][trirem5[2]].number;
						triangle_data[tri_count].color = 1;
						tri_count++;
					}
					else{
						flag = 0;
					}
				}
			}
		}
	}
	//edge
#if 0
	for (s = 0; s < 2; s++){
		for (i = 0; i < 13; i++){
			for (j = 0; j <= i; j++){
				if (s == 0 && node_surface[i][j][s].edge_flag == 1){
					trirem1[0] = i;
					trirem1[1] = j;
					trirem1[2] = s;
				}
			}
		}
		for (k = 0; k < 3; k++){
			node_surface[trirem1[0]][trirem1[1]][0].pos.x[k] = node_surface[trirem1[0]][trirem1[1]][1].pos.x[k];
		}
	}
	
#endif
#endif
	printf("%d\n", tri_count);
	//printf("node_surface = %d, %d\n", node_surface[1][1][0].number, node_surface[1][1][1]);
	//numbering on triangle
#if 1
	for (s = 0; s < 2; s++){
		for (i = 0; i < side_num; i++){
			for (j = 0; j <= i; j++){
				for (h = 0; h < 3; h++){
					node_surface2[node_surface[i][j][s].number].pos.x[h] = node_surface[i][j][s].pos.x[h];
				}
			}
		}
	}
	for (s = 0; s < 2; s++){
		for (i = 0; i < side_num; i++){
			for (j = 0; j <= i; j++){
				if (j == 0 || j == i || i == side_num - 1){
					node_surface2[node_surface[i][j][s].number].edge_flag = 1;
					node_surface2[node_surface[i][j][s].number].none_flag = 0;
				}
				else{
					node_surface2[node_surface[i][j][s].number].edge_flag = 0;
					node_surface2[node_surface[i][j][s].number].none_flag = 1;
				}
			}
		}
	}
#endif	
#if 1
	////Neighbor vertex Px

	//forward
	for (i = 0; i < num_count * 0.5; i++){
	//	if (node_surface2[i].edge_flag == 0){
		if (i != 0 && i != num_count * 0.5 - 1 && i != num_count * 0.5 - side_num){
			for (j = 0; j < num_count * 0.5; j++){
				if (i != j){
					if (sqrt(pow((node_surface2[i].pos.x[0] - node_surface2[j].pos.x[0]), 2.0) + pow((node_surface2[i].pos.x[1] - node_surface2[j].pos.x[1]), 2.0) + pow((node_surface2[i].pos.x[2] - node_surface2[j].pos.x[2]), 2.0)) < 1.5){
						if (node_surface2[i].pos.x[0] < node_surface2[j].pos.x[0] && node_surface2[i].pos.x[1] > node_surface2[j].pos.x[1]){
							node_surface2[i].N[0] = j;
							//printf("node_surface2[i].N[0] = %d, %d\n", node_surface2[i].N[0], i);
						}
						if (node_surface2[i].pos.x[0] < node_surface2[j].pos.x[0] && node_surface2[i].pos.x[1] - node_surface2[j].pos.x[1] == 0){
							node_surface2[i].N[1] = j;
							//printf("N[1] = %d, %d\n", node_surface2[i].N[1], i);
						}
						if (node_surface2[i].pos.x[0] < node_surface2[j].pos.x[0] && node_surface2[i].pos.x[1] < node_surface2[j].pos.x[1]){
							node_surface2[i].N[2] = j;
							//printf("N[1] = %d, %d \n", node_surface2[i].N[2], i);
						}
						if (node_surface2[i].pos.x[0] > node_surface2[j].pos.x[0] && node_surface2[i].pos.x[1] < node_surface2[j].pos.x[1]){
							node_surface2[i].N[3] = j;
							//printf("N[1] = %d, %d \n", node_surface2[i].N[3], i);
						}
						if (node_surface2[i].pos.x[0] > node_surface2[j].pos.x[0] && node_surface2[i].pos.x[1] - node_surface2[j].pos.x[1] == 0){
							node_surface2[i].N[4] = j;
							//printf("N[1] = %d, %d \n", node_surface2[i].N[4], i);
						}
						if (node_surface2[i].pos.x[0] > node_surface2[j].pos.x[0] && node_surface2[i].pos.x[1] > node_surface2[j].pos.x[1]){
							node_surface2[i].N[5] = j;
							//printf("N[5] = %d, %d \n", node_surface2[i].N[5], i);
						}
					}
				}
			}
		}
	}
	//backward
	for (i = num_count * 0.5; i < num_count; i++){
		//if (node_surface2[i].edge_flag == 0){
		if (i != num_count * 0.5 && i != num_count - 1 && i != num_count - side_num){
			for (j = num_count * 0.5; j < num_count; j++){
				if (i != j){
					if (sqrt(pow((node_surface2[i].pos.x[0] - node_surface2[j].pos.x[0]), 2.0) + pow((node_surface2[i].pos.x[1] - node_surface2[j].pos.x[1]), 2.0) + pow((node_surface2[i].pos.x[2] - node_surface2[j].pos.x[2]), 2.0)) < 1.5){
						if (node_surface2[i].pos.x[0] < node_surface2[j].pos.x[0] && node_surface2[i].pos.x[1] > node_surface2[j].pos.x[1]){
							node_surface2[i].N[0] = j;
							//printf("node_surface2[i].N[0] = %d, %d\n", node_surface2[i].N[0], i);
						}
						if (node_surface2[i].pos.x[0] < node_surface2[j].pos.x[0] && node_surface2[i].pos.x[1] - node_surface2[j].pos.x[1] == 0){
							node_surface2[i].N[1] = j;
							//printf("N[1] = %d, %d\n", node_surface2[i].N[1], i);
						}
						if (node_surface2[i].pos.x[0] < node_surface2[j].pos.x[0] && node_surface2[i].pos.x[1] < node_surface2[j].pos.x[1]){
							node_surface2[i].N[2] = j;
							//printf("N[1] = %d, %d \n", node_surface2[i].N[2], i);
						}
						if (node_surface2[i].pos.x[0] > node_surface2[j].pos.x[0] && node_surface2[i].pos.x[1] < node_surface2[j].pos.x[1]){
							node_surface2[i].N[3] = j;
							//printf("N[1] = %d, %d \n", node_surface2[i].N[3], i);
						}
						if (node_surface2[i].pos.x[0] > node_surface2[j].pos.x[0] && node_surface2[i].pos.x[1] - node_surface2[j].pos.x[1] == 0){
							node_surface2[i].N[4] = j;
							//printf("N[1] = %d, %d \n", node_surface2[i].N[4], i);
						}
						if (node_surface2[i].pos.x[0] > node_surface2[j].pos.x[0] && node_surface2[i].pos.x[1] > node_surface2[j].pos.x[1]){
							node_surface2[i].N[5] = j;
							//printf("N[5] = %d, %d \n", node_surface2[i].N[5], i);
						}
					}
				}
			}
		}
	}
	//triangle side neighboring 

	//for (s = 0; s < 2; s++){
	//	if (s == 0){
	//		for (i = 0; i < side_num; i++){
	//			for (j = 0; j <= i; j++){
	//				if (node_surface[i][j][s].number != 0 && node_surface[i][j][s].number != num_count * 0.5 - 1 && node_surface[i][j][s].number != num_count * 0.5 - side_num){
	//					if (node_surface[i][j][s].edge_flag == 1 && j == 0){
	//						node_surface2[node_surface[i][j][s].number].N[0] = node_surface[i - 1][j][s].number;
	//						node_surface2[node_surface[i][j][s].number].N[1] = node_surface[i][j + 1][s].number;
	//						node_surface2[node_surface[i][j][s].number].N[2] = node_surface[i + 1][j + 1][s].number;
	//						node_surface2[node_surface[i][j][s].number].N[3] = node_surface[i + 1][j][s].number;
	//						node_surface2[node_surface[i][j][s].number].N[4] = node_surface[i + 1][j + 1][s + 1].number;
	//						node_surface2[node_surface[i][j][s].number].N[5] = node_surface[i][j + 1][s + 1].number;
	//						//printf("i = %d, N = %d\n", node_surface[i][j][s].number, node_surface2[node_surface[i][j][s].number].N[1]);
	//					}
	//					if (node_surface[i][j][s].edge_flag == 1 && j == i){
	//						node_surface2[node_surface[i][j][s].number].N[0] = node_surface[i][j - 1][s + 1].number;
	//						node_surface2[node_surface[i][j][s].number].N[1] = node_surface[i + 1][j - 1][s + 1].number;
	//						node_surface2[node_surface[i][j][s].number].N[2] = node_surface[i + 1][j][s].number;
	//						node_surface2[node_surface[i][j][s].number].N[3] = node_surface[i + 1][j - 1][s].number;
	//						node_surface2[node_surface[i][j][s].number].N[4] = node_surface[i][j - 1][s].number;
	//						node_surface2[node_surface[i][j][s].number].N[5] = node_surface[i - 1][j][s].number;
	//						//printf("i = %d, N = %d\n", node_surface[i][j][s].number, node_surface2[node_surface[i][j][s].number].N[0]);
	//					}
	//					if (node_surface[i][j][s].edge_flag == 1 && i == side_num - 1){
	//						node_surface2[node_surface[i][j][s].number].N[0] = node_surface[i - 1][j][s].number;
	//						node_surface2[node_surface[i][j][s].number].N[1] = node_surface[i][j + 1][s].number;
	//						node_surface2[node_surface[i][j][s].number].N[2] = node_surface[i - 1][j - 1][s + 1].number;
	//						node_surface2[node_surface[i][j][s].number].N[3] = node_surface[i - 1][j][s + 1].number;
	//						node_surface2[node_surface[i][j][s].number].N[4] = node_surface[i][j - 1][s].number;
	//						node_surface2[node_surface[i][j][s].number].N[5] = node_surface[i - 1][j - 1][s].number;
	//						//printf("i = %d, N = %d\n", node_surface[i][j][s].number, node_surface2[node_surface[i][j][s].number].N[0]);
	//					}
	//				}
	//			}
	//		}
	//	}
	//	else{
	//		for (i = 0; i < side_num; i++){
	//			for (j = 0; j <= i; j++){
	//				if (node_surface[i][j][s].number != 0 && node_surface[i][j][s].number != num_count * 0.5 - 1 && node_surface[i][j][s].number != num_count * 0.5 - side_num){
	//					if (node_surface[i][j][s].edge_flag == 1 && j == 0){
	//						node_surface2[node_surface[i][j][s].number].N[0] = node_surface[i - 1][j][s].number;
	//						node_surface2[node_surface[i][j][s].number].N[1] = node_surface[i][j + 1][s].number;
	//						node_surface2[node_surface[i][j][s].number].N[2] = node_surface[i + 1][j + 1][s].number;
	//						node_surface2[node_surface[i][j][s].number].N[3] = node_surface[i + 1][j][s].number;
	//						node_surface2[node_surface[i][j][s].number].N[4] = node_surface[i + 1][j + 1][s - 1].number;
	//						node_surface2[node_surface[i][j][s].number].N[5] = node_surface[i][j + 1][s - 1].number;
	//					}
	//					if (node_surface[i][j][s].edge_flag == 1 && j == i){
	//						node_surface2[node_surface[i][j][s].number].N[0] = node_surface[i][j - 1][s - 1].number;
	//						node_surface2[node_surface[i][j][s].number].N[1] = node_surface[i + 1][j - 1][s - 1].number;
	//						node_surface2[node_surface[i][j][s].number].N[2] = node_surface[i + 1][j][s].number;
	//						node_surface2[node_surface[i][j][s].number].N[3] = node_surface[i + 1][j - 1][s].number;
	//						node_surface2[node_surface[i][j][s].number].N[4] = node_surface[i][j - 1][s].number;
	//						node_surface2[node_surface[i][j][s].number].N[5] = node_surface[i - 1][j][s].number;
	//					}
	//					if (node_surface[i][j][s].edge_flag == 1 && i == side_num){
	//						node_surface2[node_surface[i][j][s].number].N[0] = node_surface[i - 1][j][s].number;
	//						node_surface2[node_surface[i][j][s].number].N[1] = node_surface[i][j + 1][s].number;
	//						node_surface2[node_surface[i][j][s].number].N[2] = node_surface[i - 1][j - 1][s - 1].number;
	//						node_surface2[node_surface[i][j][s].number].N[3] = node_surface[i - 1][j][s - 1].number;
	//						node_surface2[node_surface[i][j][s].number].N[4] = node_surface[i][j - 1][s].number;
	//						node_surface2[node_surface[i][j][s].number].N[5] = node_surface[i - 1][j - 1][s].number;
	//					}
	//				}
	//			}
	//		}
	//	}
	//}
	
	//edge neighbor initialization
	//for (i = 0; i < num_count; i++){
	//	if (node_surface2[i].edge_flag == 1){
	//		if (i != 0 && i != num_count * 0.5 - 1 && i != num_count * 0.5 - side_num){
	//			if (i < num_count * 0.5){
	//				//printf("%d\n", i);
	//				for (j = 0; j < num_count * 0.5; j++){
	//					if (i != j){
	//						if (sqrt(pow((node_surface2[i].pos.x[0] - node_surface2
	//							[j].pos.x[0]), 2.0) + pow((node_surface2[i].pos.x[1] - node_surface2[j].pos.x[1]), 2.0) + pow((node_surface2[i].pos.x[2] - node_surface2[j].pos.x[2]), 2.0)) < 1.5){
	//							if (node_surface2[i].pos.x[0] < node_surface2[j].pos.x[0] && node_surface2[i].pos.x[1] > node_surface2[j].pos.x[1]){
	//								node_surface2[i].N[0] = j;
	//								//printf("i = %d, N[0] = %d\n", i, node_surface2[i].N[0]);
	//							}
	//							if (node_surface2[i].pos.x[0] < node_surface2[j].pos.x[0] && node_surface2[i].pos.x[1] - node_surface2[j].pos.x[1] == 0){
	//								node_surface2[i].N[1] = j;
	//								//printf("i = %d, N[1] = %d\n", i, node_surface2[i].N[1]);
	//							}
	//							if (node_surface2[i].pos.x[0] < node_surface2[j].pos.x[0] && node_surface2[i].pos.x[1] < node_surface2[j].pos.x[1]){
	//								node_surface2[i].N[2] = j;
	//								//printf("N[1] = %d, %d \n", node_surface2[i].N[2], i);
	//							}
	//							if (node_surface2[i].pos.x[0] > node_surface2[j].pos.x[0] && node_surface2[i].pos.x[1] < node_surface2[j].pos.x[1]){
	//								node_surface2[i].N[3] = j;
	//								//printf("N[1] = %d, %d \n", node_surface2[i].N[3], i);
	//							}
	//							if (node_surface2[i].pos.x[0] > node_surface2[j].pos.x[0] && node_surface2[i].pos.x[1] - node_surface2[j].pos.x[1] == 0){
	//								node_surface2[i].N[4] = j;
	//								//printf("N[1] = %d, %d \n", node_surface2[i].N[4], i);
	//							}
	//							if (node_surface2[i].pos.x[0] > node_surface2[j].pos.x[0] && node_surface2[i].pos.x[1] > node_surface2[j].pos.x[1]){
	//								node_surface2[i].N[5] = j;
	//								//printf("N[5] = %d, %d \n", node_surface2[i].N[5], i);
	//							}
	//						}
	//					}
	//				}
	//			}
	//			else{
	//				if (i != num_count * 0.5 && i != num_count - 1 && i != num_count - side_num){
	//					for (j = num_count * 0.5; j < num_count; j++){
	//						if (i != j){
	//							if (sqrt(pow((node_surface2[i].pos.x[0] - node_surface2[j].pos.x[0]), 2.0) + pow((node_surface2[i].pos.x[1] - node_surface2[j].pos.x[1]), 2.0) + pow((node_surface2[i].pos.x[2] - node_surface2[j].pos.x[2]), 2.0)) < 1.5){
	//								if (node_surface2[i].pos.x[0] < node_surface2[j].pos.x[0] && node_surface2[i].pos.x[1] > node_surface2[j].pos.x[1]){
	//									node_surface2[i].N[0] = j;
	//									//printf("i = %d, N[0] = %d\n", i, node_surface2[i].N[0]);
	//								}
	//								if (node_surface2[i].pos.x[0] < node_surface2[j].pos.x[0] && node_surface2[i].pos.x[1] - node_surface2[j].pos.x[1] == 0){
	//									node_surface2[i].N[1] = j;
	//									//printf("i = %d, N[1] = %d\n", i, node_surface2[i].N[1]);
	//								}
	//								if (node_surface2[i].pos.x[0] < node_surface2[j].pos.x[0] && node_surface2[i].pos.x[1] < node_surface2[j].pos.x[1]){
	//									node_surface2[i].N[2] = j;
	//									//printf("N[1] = %d, %d \n", node_surface2[i].N[2], i);
	//								}
	//								if (node_surface2[i].pos.x[0] > node_surface2[j].pos.x[0] && node_surface2[i].pos.x[1] < node_surface2[j].pos.x[1]){
	//									node_surface2[i].N[3] = j;
	//									//printf("N[1] = %d, %d \n", node_surface2[i].N[3], i);
	//								}
	//								if (node_surface2[i].pos.x[0] > node_surface2[j].pos.x[0] && node_surface2[i].pos.x[1] - node_surface2[j].pos.x[1] == 0){
	//									node_surface2[i].N[4] = j;
	//									//printf("N[1] = %d, %d \n", node_surface2[i].N[4], i);
	//								}
	//								if (node_surface2[i].pos.x[0] > node_surface2[j].pos.x[0] && node_surface2[i].pos.x[1] > node_surface2[j].pos.x[1]){
	//									node_surface2[i].N[5] = j;
	//									//printf("N[5] = %d, %d \n", node_surface2[i].N[5], i);
	//								}
	//							}
	//						}
	//					}
	//				}
	//			}
	//		}
	//	}
	//}
	for (i = 0; i < num_count; i++){
		if (node_surface2[i].edge_flag == 1){
			if (i != 0 && i != num_count * 0.5 - 1 && i != num_count * 0.5 - side_num){
				if (i < num_count * 0.5){
					//printf("%d\n", i);
					for (j = 0; j < num_count * 0.5; j++){
						if (i != j){
							if (node_surface2[i].N[0] == NULL && i != 1){
								node_surface2[i].N[0] = i - 1 + num_count * 0.5;
								//printf("i = %d, N[0] = %d\n", i, node_surface2[i].N[0]);
							}
							if (node_surface2[i].N[1] == NULL){
								node_surface2[i].N[1] = node_surface2[i].N[3] + num_count * 0.5;
								//printf("i = %d, N[1] = %d\n", i, node_surface2[i].N[1]);
							}
							if (node_surface2[i].N[2] == NULL){
								node_surface2[i].N[2] = node_surface2[i].N[0] + num_count * 0.5;
								//printf("i = %d, N[2] = %d\n", i, node_surface2[i].N[2]);
							}
							if (node_surface2[i].N[3] == NULL){
								node_surface2[i].N[3] = node_surface2[i].N[5] + num_count * 0.5;
								//printf("i = %d, N[3] = %d\n", i, node_surface2[i].N[3]);
							}
							if (node_surface2[i].N[4] == NULL){
								node_surface2[i].N[4] = node_surface2[i].N[2] + num_count * 0.5;
								//printf("i = %d, N[4] = %d\n", i, node_surface2[i].N[4]);
							}
							if (node_surface2[i].N[5] == NULL && i != 2){
								node_surface2[i].N[5] = node_surface2[i].N[1] + num_count * 0.5;
							}
						}
					}
				}
				else{
					if (i != num_count * 0.5 && i != num_count - 1 && i != num_count - side_num){
						for (j = num_count * 0.5; j < num_count; j++){
							if (i != j){
								if (node_surface2[i].N[0] == NULL && i != 1){
									node_surface2[i].N[0] = i - num_count * 0.5 - 1;
									//printf("i = %d, N[0] = %d\n", i, node_surface2[i].N[0]);
								}
								if (node_surface2[i].N[1] == NULL){
									node_surface2[i].N[1] = node_surface2[i].N[3] - num_count * 0.5;
									//printf("i = %d, N[1] = %d\n", i, node_surface2[i].N[1]);
								}
								if (node_surface2[i].N[2] == NULL){
									node_surface2[i].N[2] = node_surface2[i].N[0] - num_count * 0.5;
									//printf("i = %d, N[2] = %d\n", i, node_surface2[i].N[2]);
								}
								if (node_surface2[i].N[3] == NULL){
									node_surface2[i].N[3] = node_surface2[i].N[5] - num_count * 0.5;
									//printf("i = %d, N[3] = %d\n", i, node_surface2[i].N[3]);
								}
								if (node_surface2[i].N[4] == NULL){
									node_surface2[i].N[4] = node_surface2[i].N[2] - num_count * 0.5;
									//printf("i = %d, N[4] = %d\n", i, node_surface2[i].N[4]);
								}
								if (node_surface2[i].N[5] == NULL && i != 2){
									node_surface2[i].N[5] = node_surface2[i].N[1] - num_count * 0.5;
								}
							}
						}
					}
				}
			}
		}
	}
	///3 edges of triangle
	/*for (i = 0; i < num_count * 0.5; i++){
		if (node_surface2[i].edge_flag == 1){
			if (i == 0 || i == num_count * 0.5 - 1 || i == num_count * 0.5 - side_num){
				if (node_surface2[i].N[2] != NULL && node_surface2[i].N[3] != NULL){
					node_surface2[i].N[0] = node_surface2[i].N[3] + num_count * 0.5;
					node_surface2[i].N[1] = node_surface2[i].N[2] + num_count * 0.5;
				}
				if (node_surface2[i].N[0] != NULL && node_surface2[i].N[1] != NULL){
					node_surface2[i].N[2] = node_surface2[i].N[1] + num_count * 0.5;
					node_surface2[i].N[3] = node_surface2[i].N[0] + num_count * 0.5;
				}
				if (node_surface2[i].N[4] != NULL && node_surface2[i].N[5] != NULL){
					node_surface2[i].N[0] = node_surface2[i].N[5] + num_count * 0.5;
					node_surface2[i].N[1] = node_surface2[i].N[4] + num_count * 0.5;
				}
			}
		}
	}*/

	//printf("N[4] = %d\n", node_surface2[0].N[4]);
	for (i = 0; i < num_count; i++){
		for (j = 0; j < 6; j++){
			node_surface2[i].T[j] = 0;
		}
	}
	//printf("%d", node_surface2[16].N[1]);
	//Neighbor Triangle forward
	for (i = 0; i < num_count * 0.5; i++){
		//if (node_surface2[i].edge_flag == 0){
			for (j = 0; j < tri_count * 0.5; j++){
				if (triangle_data[j].t[2] == i){
					//printf("i = %d, j = %d\n", i, j);
					if (node_surface2[i].N[1] == triangle_data[j].t[1]){
						node_surface2[i].T[0] = j;
						//printf("i = %d, j = %d\n", i, j);
					}
					else{
						node_surface2[i].T[5] = j;
						//printf("i = %d, j = %d\n", i, j);
					}
				}
				if (triangle_data[j].t[0] == i){
					if (node_surface2[i].N[1] == triangle_data[j].t[1]){
						node_surface2[i].T[1] = j;
					}
					else{
						node_surface2[i].T[2] = j;
						//printf("i = %d, j = %d\n", i, j);
					}
				}
				if (triangle_data[j].t[1] == i){
					if (node_surface2[i].N[3] == triangle_data[j].t[2]){
						node_surface2[i].T[3] = j;
					}
					else{
						node_surface2[i].T[4] = j;
						//printf("i = %d, j = %d\n", i, j);
					}
				}
			}
		//}
	}
	//Neigbor Triangle backward
	for (i = num_count * 0.5; i < num_count; i++){
		//if (node_surface2[i].edge_flag == 0){
			for (j = tri_count * 0.5; j < tri_count; j++){
				if (triangle_data[j].t[2] == i){
					if (node_surface2[i].N[1] == triangle_data[j].t[1]){
						node_surface2[i].T[0] = j;
						//printf("i = %d, j = %d\n", i, j);
					}
					else{
						node_surface2[i].T[5] = j;
						//printf("i = %d, j = %d\n", i, j);
					}
				}
				if (triangle_data[j].t[0] == i){
					if (node_surface2[i].N[1] == triangle_data[j].t[1]){
						node_surface2[i].T[1] = j;
					}
					else{
						node_surface2[i].T[2] = j;
						//printf("i = %d, j = %d\n", i, j);
					}
				}
				if (triangle_data[j].t[1] == i){
					if (node_surface2[i].N[3] == triangle_data[j].t[2]){
						node_surface2[i].T[3] = j;
					}
					else{
						node_surface2[i].T[4] = j;
						//printf("i = %d, j = %d\n", i, j);
					}
				}
			}
		//}
	}
	//for (i = 0; i < num_count * 0.5; i++){
	//	for (j = 0; j < tri_count * 0.5; j++){
	//		if (triangle_data[j].t[0] == i || triangle_data[j].t[1] == i || triangle_data[j].t[2] == i){
	//			//printf("i = %d, j = %d\n", i, j);
	//			if (triangle_data[j].t[0] == node_surface2[i].N[0] && triangle_data[j].t[1] == node_surface2[i].N[1]){
	//				node_surface2[i].T[0] = j;
	//				//printf("T[0] = %d, %d\n", i, node_surface2[i].T[0]);
	//				//printf("N[0] = %d, N[1] = %d\n", node_surface2[i].N[0], node_surface2[i].N[1]);
	//			}
	//			if (triangle_data[j].t[1] == node_surface2[i].N[1] && triangle_data[j].t[2] == node_surface2[i].N[2]){
	//				node_surface2[i].T[1] = j;
	//				//printf("T[0] = %d, %d\n", i, node_surface2[i].T[1]);
	//			}
	//			if (triangle_data[j].t[1] == node_surface2[i].N[2] && triangle_data[j].t[2] == node_surface2[i].N[3]){
	//				node_surface2[i].T[2] = j;
	//				//printf("T[0] = %d, %d\n", i, node_surface2[i].T[2]);
	//			}
	//			if (triangle_data[j].t[0] == node_surface2[i].N[4] && triangle_data[j].t[2] == node_surface2[i].N[3]){
	//				if (node_surface2[i].N[4] != i){
	//					node_surface2[i].T[3] = j;
	//					//printf("T[3] = %d, %d\n", i, node_surface2[i].T[3]);
	//					//printf("N[4] = %d, N[3] = %d\n", node_surface2[i].N[4], node_surface2[i].N[3]);
	//				}
	//			}
	//			if (triangle_data[j].t[2] == node_surface2[i].N[4]){
	//				//if (node_surface2[i].N[4] != i){
	//					node_surface2[i].T[4] = j;
	//					//printf("T[4] = %d, %d\n", i, node_surface2[i].T[4]);
	//					//printf("N[4] = %d, N[5] = %d, i = %d\n", node_surface2[i].N[4], node_surface2[i].N[5], i);
	//				//}
	//			}
	//			if (triangle_data[j].t[0] == node_surface2[i].N[5]){// && triangle_data[j].t[2] == i){ 
	//				//node_surface2[i].T[5] = j;
	//				node_surface2[i].T[5] = j;
	//				//printf("T[5] = %d,i = %d\n", node_surface2[i].T[5], i);
	//				//printf("i = %d,T[5] = %d, N[5] = %d\n", i, node_surface2[i].T[5], node_surface2[i].N[5]);
	//			}
	//		}
	//	}
	//}
	//printf("%d", node_surface2[2].T[0]);
	// Neighbor Triangle backward
	//for (i = num_count * 0.5; i < num_count; i++){
	//	if (node_surface2[i].edge_flag == 0){
	//		for (j = tri_count * 0.5; j < tri_count; j++){
	//			if (triangle_data[j].t[0] == i || triangle_data[j].t[1] == i || triangle_data[j].t[2] == i){
	//				if (triangle_data[j].t[0] == node_surface2[i].N[0] && triangle_data[j].t[1] == node_surface2[i].N[1]){
	//					node_surface2[i].T[0] = j;
	//					//printf("T[0] = %d, %d\n", i, node_surface2[i].T[0]);
	//					//printf("N[0] = %d, N[1] = %d\n", node_surface2[i].N[0], node_surface2[i].N[1]);
	//				}
	//				if (triangle_data[j].t[1] == node_surface2[i].N[1] && triangle_data[j].t[2] == node_surface2[i].N[2]){
	//					node_surface2[i].T[1] = j;
	//					//printf("T[0] = %d, %d\n", i, node_surface2[i].T[1]);
	//				}
	//				if (triangle_data[j].t[1] == node_surface2[i].N[2] && triangle_data[j].t[2] == node_surface2[i].N[3]){
	//					node_surface2[i].T[2] = j;
	//					//printf("T[0] = %d, %d\n", i, node_surface2[i].T[2]);
	//				}
	//				if (triangle_data[j].t[0] == node_surface2[i].N[4] && triangle_data[j].t[2] == node_surface2[i].N[3]){
	//					if (node_surface2[i].N[4] != i){
	//						node_surface2[i].T[3] = j;
	//						//printf("T[3] = %d, %d\n", i, node_surface2[i].T[3]);
	//						//printf("N[4] = %d, N[3] = %d\n", node_surface2[i].N[4], node_surface2[i].N[3]);
	//					}
	//				}
	//				if (triangle_data[j].t[2] == node_surface2[i].N[4] && triangle_data[j].t[0] == node_surface2[i].N[5]){
	//					//if (node_surface2[i].N[4] != i){
	//					node_surface2[i].T[4] = j;
	//					//printf("T[4] = %d, %d\n", i, node_surface2[i].T[4]);
	//					//printf("N[4] = %d, N[5] = %d\n", node_surface2[i].N[4], node_surface2[i].N[5]);
	//					//}
	//				}
	//				if (triangle_data[j].t[0] == node_surface2[i].N[5] && triangle_data[j].t[1] == node_surface2[i].N[0]){// && triangle_data[j].t[2] == i){ 
	//					//node_surface2[i].T[5] = j;
	//					node_surface2[i].T[5] = j;
	//					//printf("i = %d,T[5] = %d, N[5] = %d\n", i, node_surface2[i].T[5], node_surface2[i].N[5]);
	//				}
	//			}
	//		}
	//	}
	//}
	//printf("%d", node_surface2[2].N[0]);
	//printf("%d\n", node_surface2[1].T[3]);
	//printf(" T[4] = %d,T[5] =  %d,T[0] = %d\n",  node_surface2[16].T[4], node_surface2[16].T[5], node_surface2[16].T[0]);
	//edge triangle neighbor


	for (i = 0; i < num_count; i++){
		if (node_surface2[i].edge_flag == 1){
			if (i < num_count * 0.5){
				if (i != 0 && i != num_count * 0.5 - 1 && i != num_count * 0.5 - side_num){
					for (j = 0; j < tri_count * 0.5; j++){
						if (triangle_data[j].t[2] == i){
							//printf("i = %d\n", i);
							if (node_surface2[i].T[5] == NULL){
								//printf("i = %d\n", i);
								node_surface2[i].T[5] = node_surface2[i].T[0] + tri_count * 0.5;
								node_surface2[i].T[3] = node_surface2[i].T[2] + tri_count * 0.5;
								node_surface2[i].T[4] = node_surface2[i].T[1] + tri_count * 0.5;
								//printf("i = %d, T[5] = %d,T[4] =  %d,T[3] = %d\n", i, node_surface2[i].T[5], node_surface2[i].T[4], node_surface2[i].T[3]);
								//printf("i = %d\n", i);
							}
							if (node_surface2[i].T[3] == NULL){
								if (node_surface2[i].T[2] == NULL){
									node_surface2[i].T[3] = node_surface2[i].T[4] + tri_count * 0.5;
									node_surface2[i].T[2] = node_surface2[i].T[5] + tri_count * 0.5;
									node_surface2[i].T[1] = node_surface2[i].T[0] + tri_count * 0.5;
									//printf("i = %d, T[3] = %d,T[2] =  %d,T[1] = %d\n", i, node_surface2[i].T[3], node_surface2[i].T[2], node_surface2[i].T[1]);
									//printf("i = %d\n", i);
								}
							}
						}
						if (triangle_data[j].t[1] == i){
							if (node_surface2[i].T[5] == NULL){
								node_surface2[i].T[5] = node_surface2[i].T[4] + tri_count * 0.5;
								node_surface2[i].T[0] = node_surface2[i].T[3] + tri_count * 0.5;
								node_surface2[i].T[1] = node_surface2[i].T[2] + tri_count * 0.5;
								//printf("i = %d, T[5] = %d,T[0] =  %d,T[1] = %d\n", i, node_surface2[i].T[5], node_surface2[i].T[0], node_surface2[i].T[1]);
							}
						}
					}
				}
			}
			else{
				if (i != num_count * 0.5 && i != num_count - 1 && i != num_count - side_num){
					for (j = tri_count * 0.5; j < tri_count; j++){
						if (triangle_data[j].t[2] == i){
							//printf("i = %d\n", i);
							if (node_surface2[i].T[5] == NULL){
								//printf("i = %d\n", i);
								node_surface2[i].T[5] = node_surface2[i].T[0] - tri_count * 0.5;
								node_surface2[i].T[3] = node_surface2[i].T[2] - tri_count * 0.5;
								//printf("i = %d,T[0] = %d\n", i, node_surface2[i].T[5]);
								node_surface2[i].T[4] = node_surface2[i].T[1] - tri_count * 0.5;
								//printf("i = %d\n", i);
							}
							if (node_surface2[i].T[3] == NULL){
								if (node_surface2[i].T[2] == NULL){
									node_surface2[i].T[3] = node_surface2[i].T[4] - tri_count * 0.5;
									node_surface2[i].T[2] = node_surface2[i].T[5] - tri_count * 0.5;
									node_surface2[i].T[1] = node_surface2[i].T[0] - tri_count * 0.5;
									//printf("i = %d\n", i);
								}
							}
						}
						if (triangle_data[j].t[1] == i){
							if (node_surface2[i].T[5] == NULL){
								node_surface2[i].T[5] = node_surface2[i].T[4] - tri_count * 0.5;
								node_surface2[i].T[0] = node_surface2[i].T[3] - tri_count * 0.5;
								node_surface2[i].T[1] = node_surface2[i].T[2] - tri_count * 0.5;
								//printf("i = %d\n", i);
							}
						}
					}
				}
			}
		}
	}

	for (i = 0; i < num_count * 0.5; i++){
		for (j = 0; j < 6; j++){
			//printf("i = %d, j = %d, T = %d\n", i, j, node_surface2[i].T[j]);
		}
	}

	//for (i = 0; i < num_count * 0.5; i++){
	//	if (node_surface2[i].edge_flag == 0){
	//		if (i == 0 || i == num_count * 0.5 - 1 || i == num_count * 0.5 - side_num){
	//			for (j = 0; j < 6; j++){
	//				node_surface2[i].T[j] = 0;
	//			}
	//		}
	//	}
	//}
	/*for (i = 0; i < num_count * 0.5; i++){
		if (node_surface2[i].edge_flag == 1){
			if (i == 0){
				node_surface2[i].T[0] = 0;
				node_surface2[i].T[1] = tri_count * 0.5;
			}
			if (i == num_count * 0.5 - 1){
				node_surface2[i].T[0] = tri_count * 0.5 - side_num - 2;
				node_surface2[i].T[1] = tri_count - side_num - 2;
			}
			if (i == num_count * 0.5 - side_num){
				node_surface2[i].T[0] = tri_count * 0.5 - 1;
				node_surface2[i].T[1] = tri_count - 1;
			}
		}
	}*/
	/*for (i = 0; i < num_count; i++){
			for (j = 0; j < 6; j++){
				printf("i = %d,j = %d,N[j] = %d\n", i, j ,node_surface2[i].N[j]);
			}
		}*/
	////revise 1
	//for (i = 0; i < num_count * 0.5; i++){
	//	if (node_surface2[i].edge_flag == 0){
	//		for (j = 0; j < num_count * 0.5; j++){
	//			if (i != j){
	//				if (sqrt(pow((node_surface2[i].pos.x[0] - node_surface2[j].pos.x[0]), 2.0) + pow((node_surface2[i].pos.x[1] - node_surface2[j].pos.x[1]), 2.0) + pow((node_surface2[i].pos.x[2] - node_surface2[j].pos.x[2]), 2.0)) < 1.5){
	//					if (node_surface2[i].pos.x[0] > node_surface2[j].pos.x[0] && node_surface2[i].pos.x[1] > node_surface2[j].pos.x[1]){
	//						node_surface2[i].N[5] = j;
	//					}
	//				}
	//			}
	//		}
	//	}
	//}
	////revise 2
	//for (i = num_count * 0.5; i < num_count; i++){
	//	if (node_surface2[i].edge_flag == 0){
	//		for (j = num_count * 0.5; j < num_count; j++){
	//			if (i != j){
	//				if (sqrt(pow((node_surface2[i].pos.x[0] - node_surface2[j].pos.x[0]), 2.0) + pow((node_surface2[i].pos.x[1] - node_surface2[j].pos.x[1]), 2.0) + pow((node_surface2[i].pos.x[2] - node_surface2[j].pos.x[2]), 2.0)) < 1.5){
	//					if (node_surface2[i].pos.x[0] > node_surface2[j].pos.x[0] && node_surface2[i].pos.x[1] > node_surface2[j].pos.x[1]){
	//						node_surface2[i].N[5] = j;
	//					}
	//				}
	//			}
	//		}
	//	}
	//}
	//for (i = 0; i < num_count * 0.5; i++){
	//	if (node_surface2[i].edge_flag == 1) {
	//		if (i != 0 && i != num_count * 0.5 - 1 && i != num_count * 0.5 - side_num){
	//			if (node_surface2[i].N[4] > num_count * 0.5){
	//				node_surface2[i].N[5] = node_surface2[i].N[1] + num_count * 0.5;
	//				//printf("i = %d\n", i);
	//			}
	//			if (node_surface2[i].N[1] > num_count * 0.5){
	//				node_surface2[i].N[5] = node_surface2[i - 1].N[0];
	//				//printf("i = %d\n", i);
	//			}
	//			if (node_surface2[i].N[3] > num_count * 0.5){
	//				node_surface2[i].N[5] = node_surface2[i - 1].N[0];
	//				//
	//					//printf("i = %d, N[5] = %d\n", i, node_surface2[i].N[5]);
	//			}
	//		}
	//	}
	//}
	//for (i = 0; i < num_count; i++){
	//	if (node_surface2[i].edge_flag == 1){
	//		for (j = 0; j < 6; j++){
	//			//printf("i = %d,j = %d,N[j] = %d\n", i, j, node_surface2[i].N[j]);
	//		}
	//	}
	//}

	//printf("node_surface2[8].N[5]1 = %d\n", node_surface2[8].N[5]);
	//for (i = 0; i < num_count * 0.5; i++){
	//	if (node_surface2[i].edge_flag == 0){
	//		for (j = 0; j < 6; j++){
	//			printf("i = %d, j= %d, T = %d\n", i, j, node_surface2[i].T[j]);
	//		}
	//	}
	//}
	/*for (i = 0; i < num_count * 0.5; i++){

		for (j = 0; j < 6; j++){
			for (k = 0; k < 3; k++){
				node_surface2[i].m_normal[k] += triangle_data[node_surface2[i].T[j]].normal[k] / (sqrt(pow(triangle_data[node_surface2[i].T[j]].normal[0], 2) + pow(triangle_data[node_surface2[i].T[j]].normal[1], 2) + pow(triangle_data[node_surface2[i].T[j]].normal[2], 2)));
			}
		}
	}*/

	//double P0P1[3];
	//double P0P2[3];
	//double P3P2[3];
	//double P3P1[3];

	//for (i = 0; i < num_count * 0.5; i++){
	//	for (j = 1; j < 5; j++){
	//		for (k = 0; k < 3; k++){
	//			P0P1[k] = (node_surface2[node_surface2[i].N[j]].pos.x[k] - node_surface2[node_surface2[i].N[j - 1]].pos.x[k]);
	//			P0P2[k] = (node_surface2[i].pos.x[k] - node_surface2[node_surface2[i].N[j - 1]].pos.x[k]);
	//			P3P2[k] = (node_surface2[i].pos.x[k] - node_surface2[node_surface2[i].N[j + 1]].pos.x[k]);
	//			P3P1[k] = (node_surface2[node_surface2[i].N[j]].pos.x[k] - node_surface2[node_surface2[i].N[j + 1]].pos.x[k]);
	//			node_surface2[i].cosa[j] += (P0P1[k] * P0P2[k]) / (sqrt(pow(P0P1[0], 2) + pow(P0P1[1], 2) + pow(P0P1[2], 2)) * sqrt(pow(P0P2[0], 2) + pow(P0P2[1], 2) + pow(P0P2[2], 2)));
	//			node_surface2[i].cosb[j] += (P3P2[k] * P3P2[k]) / (sqrt(pow(P3P2[0], 2) + pow(P3P2[1], 2) + pow(P3P2[2], 2)) * sqrt(pow(P3P1[0], 2) + pow(P3P1[1], 2) + pow(P3P1[2], 2)));
	//			//printf("cosa = %f, i = %d,j = %d\n", node_surface2[i].cosa[j], i, j);
	//		}
	//	}
	//	if (j = 0){
	//		for (k = 0; k < 3; k++){
	//			P0P1[k] = (node_surface2[node_surface2[i].N[0]].pos.x[k] - node_surface2[node_surface2[i].N[5]].pos.x[k]);
	//			P0P2[k] = (node_surface2[i].pos.x[k] - node_surface2[node_surface2[i].N[5]].pos.x[k]);
	//			P3P2[k] = (node_surface2[i].pos.x[k] - node_surface2[node_surface2[i].N[j + 1]].pos.x[k]);
	//			P3P1[k] = (node_surface2[node_surface2[i].N[j]].pos.x[k] - node_surface2[node_surface2[i].N[j + 1]].pos.x[k]);
	//			node_surface2[i].cosa[j] += (P0P1[k] * P0P2[k]) / (sqrt(pow(P0P1[0], 2) + pow(P0P1[1], 2) + pow(P0P1[2], 2)) * sqrt(pow(P0P2[0], 2) + pow(P0P2[1], 2) + pow(P0P2[2], 2)));
	//			node_surface2[i].cosb[j] += (P3P2[k] * P3P2[k]) / (sqrt(pow(P3P2[0], 2) + pow(P3P2[1], 2) + pow(P3P2[2], 2)) * sqrt(pow(P3P1[0], 2) + pow(P3P1[1], 2) + pow(P3P1[2], 2)));
	//		}
	//	}
	//	if (j = 5){
	//		for (k = 0; k < 3; k++){
	//			P0P1[k] = (node_surface2[node_surface2[i].N[j]].pos.x[k] - node_surface2[node_surface2[i].N[j - 1]].pos.x[k]);
	//			P0P2[k] = (node_surface2[i].pos.x[k] - node_surface2[node_surface2[i].N[j - 1]].pos.x[k]);
	//			P3P2[k] = (node_surface2[i].pos.x[k] - node_surface2[node_surface2[i].N[0]].pos.x[k]);
	//			P3P1[k] = (node_surface2[node_surface2[i].N[j]].pos.x[k] - node_surface2[node_surface2[i].N[0]].pos.x[k]);
	//			node_surface2[i].cosa[j] += (P0P1[k] * P0P2[k]) / (sqrt(pow(P0P1[0], 2) + pow(P0P1[1], 2) + pow(P0P1[2], 2)) * sqrt(pow(P0P2[0], 2) + pow(P0P2[1], 2) + pow(P0P2[2], 2)));
	//			node_surface2[i].cosb[j] += (P3P2[k] * P3P2[k]) / (sqrt(pow(P3P2[0], 2) + pow(P3P2[1], 2) + pow(P3P2[2], 2)) * sqrt(pow(P3P1[0], 2) + pow(P3P1[1], 2) + pow(P3P1[2], 2)));
	//		}
	//	}
	//}
	//for (i = 0; i < num_count * 0.5; i++){
	//	for (j = 0; j < 6; j++){
	//		node_surface2[i].cota[j] = node_surface2[i].cosa[j] / sqrt(1 - pow(node_surface2[i].cosa[j], 2));
	//		node_surface2[i].cotb[j] = node_surface2[i].cosb[j] / sqrt(1 - pow(node_surface2[i].cosb[j], 2));
	//	}
	//}
	
#endif
	for (i = 0; i <= num_count - 1; i++){
		for (j = 0; j <= num_count - 1; j++){
			edge[i][j].torf = false;
		}
	}
	for (i = 0; i < tri_count; i++){
		edge[triangle_data[i].t[0]][triangle_data[i].t[1]].torf = true;
		edge[triangle_data[i].t[1]][triangle_data[i].t[0]].torf = true;
		edge[triangle_data[i].t[1]][triangle_data[i].t[2]].torf = true;
		edge[triangle_data[i].t[2]][triangle_data[i].t[1]].torf = true;
		edge[triangle_data[i].t[2]][triangle_data[i].t[0]].torf = true;
		edge[triangle_data[i].t[0]][triangle_data[i].t[2]].torf = true;
		edge[triangle_data[i].t[0]][triangle_data[i].t[1]].len = sqrt(
			pow((node_surface2[triangle_data[i].t[0]].pos.x[0] - node_surface2[triangle_data[i].t[1]].pos.x[0]), 2.0) +
			pow((node_surface2[triangle_data[i].t[0]].pos.x[1] - node_surface2[triangle_data[i].t[1]].pos.x[1]), 2.0) +
			pow((node_surface2[triangle_data[i].t[0]].pos.x[2] - node_surface2[triangle_data[i].t[1]].pos.x[2]), 2.0));
		edge[triangle_data[i].t[1]][triangle_data[i].t[0]].len = sqrt(
			pow((node_surface2[triangle_data[i].t[1]].pos.x[0] - node_surface2[triangle_data[i].t[0]].pos.x[0]), 2.0) +
			pow((node_surface2[triangle_data[i].t[1]].pos.x[1] - node_surface2[triangle_data[i].t[0]].pos.x[1]), 2.0) +
			pow((node_surface2[triangle_data[i].t[1]].pos.x[2] - node_surface2[triangle_data[i].t[0]].pos.x[2]), 2.0));
		edge[triangle_data[i].t[1]][triangle_data[i].t[2]].len = sqrt(
			pow((node_surface2[triangle_data[i].t[1]].pos.x[0] - node_surface2[triangle_data[i].t[2]].pos.x[0]), 2.0) +
			pow((node_surface2[triangle_data[i].t[1]].pos.x[1] - node_surface2[triangle_data[i].t[2]].pos.x[1]), 2.0) +
			pow((node_surface2[triangle_data[i].t[1]].pos.x[2] - node_surface2[triangle_data[i].t[2]].pos.x[2]), 2.0));
		edge[triangle_data[i].t[2]][triangle_data[i].t[1]].len = sqrt(
			pow((node_surface2[triangle_data[i].t[2]].pos.x[0] - node_surface2[triangle_data[i].t[1]].pos.x[0]), 2.0) +
			pow((node_surface2[triangle_data[i].t[2]].pos.x[1] - node_surface2[triangle_data[i].t[1]].pos.x[1]), 2.0) +
			pow((node_surface2[triangle_data[i].t[2]].pos.x[2] - node_surface2[triangle_data[i].t[1]].pos.x[2]), 2.0));
		edge[triangle_data[i].t[2]][triangle_data[i].t[0]].len = sqrt(
			pow((node_surface2[triangle_data[i].t[2]].pos.x[0] - node_surface2[triangle_data[i].t[0]].pos.x[0]), 2.0) +
			pow((node_surface2[triangle_data[i].t[2]].pos.x[1] - node_surface2[triangle_data[i].t[0]].pos.x[1]), 2.0) +
			pow((node_surface2[triangle_data[i].t[2]].pos.x[2] - node_surface2[triangle_data[i].t[0]].pos.x[2]), 2.0));
		edge[triangle_data[i].t[0]][triangle_data[i].t[2]].len = sqrt(
			pow((node_surface2[triangle_data[i].t[0]].pos.x[0] - node_surface2[triangle_data[i].t[2]].pos.x[0]), 2.0) +
			pow((node_surface2[triangle_data[i].t[0]].pos.x[1] - node_surface2[triangle_data[i].t[2]].pos.x[1]), 2.0) +
			pow((node_surface2[triangle_data[i].t[0]].pos.x[2] - node_surface2[triangle_data[i].t[2]].pos.x[2]), 2.0));
		//printf("edge = %f\n", edge[triangle_data[i].t[0]][triangle_data[i].t[1]].len);
	}
}
void node_simulation(int view_con){
	if (first_count == 1){
		initiation();
		first_count--;
	}
	int i = 0;
	int j = 0;
	int k = 0;
	int l = 0;
	int h = 0;
	int s = 0;
	int E = i + num_count * 0.5;
	double min = 100000.0;
	double max = -10.0;
	double mass = 1000;
	double kizami = 0.01;
	double total_force[3] = { 0.0, 0.0, 0.0 };
	double temp_len;
	double normal_force[3];
	double normal_temp[9];
	double normal_temp3[3];
	double pressure;

	for (i = 0; i <= num_count - 1; i++){
		for (j = 0; j < 3; j++){
			node_surface2[i].acc.x[j] = 0.0;
		}
	}


	for (i = 0; i <= num_count - 1; i++){
		node_surface2[i].color_grad = 0.0;
		for (j = 0; j < num_count; j++){
			if (edge[i][j].torf == 1){
				//printf("node_surface2[%d] = %f, %f, %f\n", i, node_surface2[i].pos.x[0], node_surface2[i].pos.x[1], node_surface2[i].pos.x[2]);
				temp_len = sqrt(pow((node_surface2[i].pos.x[0] - node_surface2[j].pos.x[0]), 2.0) + pow((node_surface2[i].pos.x[1] - node_surface2[j].pos.x[1]), 2.0) + pow((node_surface2[i].pos.x[2] - node_surface2[j].pos.x[2]), 2.0));
				//printf("%d %d %lf \n", i, j, temp_len);
				//printf("a");
				if (temp_len > edge[i][j].len){
					for (k = 0; k < 3; k++){
						node_surface2[i].color_grad += fabs(-1000.0 * damp_k * (node_surface2[i].pos.x[k] - node_surface2[j].pos.x[k]) * (temp_len - edge[i][j].len) / temp_len / mass);
						node_surface2[i].acc.x[k] += -1000.0 * damp_k * (node_surface2[i].pos.x[k] - node_surface2[j].pos.x[k]) * (temp_len - edge[i][j].len) / temp_len / mass;
						//printf("%lf\n", -1.0 * damp_k * (node_surface2[i].pos.x[k] - node_surface2[j].pos.x[k]) * (temp_len - edge[i][j].len) / temp_len / mass);
					}
				}
			}
			/*if (node_surface2[i].edge_flag == 1){
				if (temp_len > edge[i][j].len){
				for (k = 0; k < 2; k++){
				node_surface2[i].color_grad += fabs(-1000.0 * damp_k * (node_surface2[i].pos.x[k] - node_surface2[j].pos.x[k]) * (temp_len - edge[i][j].len) / temp_len / mass);
				node_surface2[i].acc.x[k] += -1000.0 * damp_k * (node_surface2[i].pos.x[k] - node_surface2[j].pos.x[k]) * (temp_len - edge[i][j].len) / temp_len / mass;
				}
				printf("node_surface_y = %d, %f\n", i,  node_surface2[i].acc.x[1]);
				}*/
		}
	}

	//edge node
#if 0
	for (i = 0; i < num_count; i++){
		node_surface2[i].color_grad = 0.0;
		for (j = 0; j <= num_count - 1; j++){
			if (edge[i][j].torf == 1){
				//printf("node_surface2[%d] = %f, %f, %f\n", i, node_surface2[i].pos.x[0], node_surface2[i].pos.x[1], node_surface2[i].pos.x[2]);
				temp_len = sqrt(pow((node_surface2[i].pos.x[0] - node_surface2[j].pos.x[0]), 2.0) + pow((node_surface2[i].pos.x[1] - node_surface2[j].pos.x[1]), 2.0) + pow((node_surface2[i].pos.x[2] - node_surface2[j].pos.x[2]), 2.0));
				//printf("%d %d %lf %lf\n", i, j, temp_len, edge[i][j].len);
				//printf("a");
				if (temp_len > edge[i][j].len){
					for (k = 0; k < 3; k++){
						if (node_surface2[i].pos.x[k] - node_surface2[j].pos.x[k] == 0){
							node_surface2[i].color_grad += fabs(-1000.0 * damp_k * (node_surface2[i].pos.x[k] - node_surface2[j].pos.x[k]) * (temp_len - edge[i][j].len) / temp_len / mass);
							node_surface2[i].acc.x[k] += -1000.0 * damp_k * (node_surface2[i].pos.x[k] - node_surface2[j].pos.x[k]) * (temp_len - edge[i][j].len) / temp_len / mass;
							//printf("%lf\n", -1.0 * damp_k * (node_surface2[i].pos.x[k] - node_surface2[j].pos.x[k]) * (temp_len - edge[i][j].len) / temp_len / mass);
						}
					}
				}
			}
		}
#endif
		if (open_flag == true){
			wall_z += 0.01;
		}
		if (close_flag == true){
			wall_z -= 0.01;
		}
		for (i = 0; i < num_count; i++){
			for (k = 0; k < 3; k++){
				node_surface2[i].acc.x[k] += -dv * node_surface2[i].del_pos.x[k];
			}
			/*	else if (node_surface2[i].edge_flag == 1){
					for (k = 0; k < 2; k++){
					node_surface2[i].acc.x[k] += -dv * node_surface2[i].del_pos.x[k];
					}
					}*/
		}
		//printf("node_surface2 = %f, %f\n", node_surface2[1].del_pos.x[2], node_surface2[56].del_pos.x[2]);
		//pressure (external first noraml_temp3 will be b in a form of a x b)
#if 1

		for (i = 0; i < tri_count; i++){
			if (i < tri_count * 0.5){
				if (triangle_data[i].color == 1){
					//printf("a");
					for (j = 0; j < 3; j++){
						normal_temp3[j] = node_surface2[triangle_data[i].t[1]].pos.x[j] - node_surface2[triangle_data[i].t[0]].pos.x[j];
					}
					gaiseki_9_3(normal_temp, normal_temp3);
					for (j = 0; j < 3; j++){
						normal_temp3[j] = node_surface2[triangle_data[i].t[2]].pos.x[j] - node_surface2[triangle_data[i].t[0]].pos.x[j];
					}
					mult_matrix3x1(normal_force, normal_temp, normal_temp3);
				}
				else if (triangle_data[i].color == 2){
					for (j = 0; j < 3; j++){
						normal_temp3[j] = node_surface2[triangle_data[i].t[2]].pos.x[j] - node_surface2[triangle_data[i].t[0]].pos.x[j];
					}
					gaiseki_9_3(normal_temp, normal_temp3);
					for (j = 0; j < 3; j++){
						normal_temp3[j] = node_surface2[triangle_data[i].t[1]].pos.x[j] - node_surface2[triangle_data[i].t[0]].pos.x[j];
					}
					mult_matrix3x1(normal_force, normal_temp, normal_temp3);
				}
			}
			if (i >= tri_count * 0.5){
				if (triangle_data[i].color == 1){
					//printf("a");
					for (j = 0; j < 3; j++){
						normal_temp3[j] = node_surface2[triangle_data[i].t[2]].pos.x[j] - node_surface2[triangle_data[i].t[0]].pos.x[j];
					}
					gaiseki_9_3(normal_temp, normal_temp3);
					for (j = 0; j < 3; j++){
						normal_temp3[j] = node_surface2[triangle_data[i].t[1]].pos.x[j] - node_surface2[triangle_data[i].t[0]].pos.x[j];
					}
					mult_matrix3x1(normal_force, normal_temp, normal_temp3);
				}
				else if (triangle_data[i].color == 2){
					for (j = 0; j < 3; j++){
						normal_temp3[j] = node_surface2[triangle_data[i].t[1]].pos.x[j] - node_surface2[triangle_data[i].t[0]].pos.x[j];
					}
					gaiseki_9_3(normal_temp, normal_temp3);
					for (j = 0; j < 3; j++){
						normal_temp3[j] = node_surface2[triangle_data[i].t[2]].pos.x[j] - node_surface2[triangle_data[i].t[0]].pos.x[j];
					}
					mult_matrix3x1(normal_force, normal_temp, normal_temp3);
				}
			}
			for (j = 0; j < 3; j++){
				triangle_data[i].normal[j] = normal_force[j] / sqrt(pow(normal_force[0], 2.0) + pow(normal_force[1], 2.0) + pow(normal_force[2], 2.0));// / normal_force[j];
				for (k = 0; k < 3; k++){
					node_surface2[triangle_data[i].t[j]].acc.x[k] += 0.1 * damp_k_normal * normal_force[k];
				}
			}
		}
		/*for (i = 0; i < tri_count; i++){
			triangle_data[i].A = sqrt(pow(normal_force[0], 2.0) + pow(normal_force[1], 2.0) + pow(normal_force[2], 2.0)) * 0.5;
			triangle_data[i].total = 3 * 0.1 * damp_k_normal * sqrt(pow(normal_force[0], 2.0) + pow(normal_force[1], 2.0) + pow(normal_force[2], 2.0));
			printf("pressure = %d, %f\n", i, (triangle_data[i].total / triangle_data[i].A));
			}*/

#endif
#if 0
		for (i = 0; i < num_count; i++){
			for (k = 0; k < 3; k++){
				if (node_surface2[i].edge_flag == 1){
					for (k = 0; k < 3; k++){
						//printf("i = %d\n", i);
						if (i < num_count * 0.5){
							for (k = 0; k < 3; k++){
								int j = i + num_count * 0.5;
								node_surface2[i].pos.x[k] = node_surface2[j].pos.x[k];
								//	printf("i = %d,j = %d\n", i, j);
							}
							node_surface2[i].del_pos.x[k] = node_surface2[i].del_pos.x[k] + node_surface2[i].acc.x[k] * kizami;
							node_surface2[i].pos.x[k] = node_surface2[i].pos.x[k] + node_surface2[i].del_pos.x[k] * kizami + 1.0 / 2.0 * node_surface2[i].acc.x[k] * kizami * kizami;
						}
					}
				}
				else{
					node_surface2[i].del_pos.x[k] = node_surface2[i].del_pos.x[k] + node_surface2[i].acc.x[k] * kizami;
					node_surface2[i].pos.x[k] = node_surface2[i].pos.x[k] + node_surface2[i].del_pos.x[k] * kizami + 1.0 / 2.0 * node_surface2[i].acc.x[k] * kizami * kizami;
				}
			}
		}
#endif

#if 1
		//printf("node_surface_z = %f, %f\n", node_surface2[0].acc.x[1], node_surface2[55].acc.x[1]);
		for (i = 0; i < num_count; i++){
			if (node_surface2[i].none_flag == 1){
				for (k = 0; k < 3; k++){
					node_surface2[i].del_pos.x[k] = node_surface2[i].del_pos.x[k] + node_surface2[i].acc.x[k] * kizami;
					node_surface2[i].pos.x[k] = node_surface2[i].pos.x[k] + node_surface2[i].del_pos.x[k] * kizami + 1.0 / 2.0 * node_surface2[i].acc.x[k] * kizami * kizami;
				}
			}
		}
		for (i = 0; i < num_count * 0.5; i++){
			if (node_surface2[i].edge_flag == 1){
				for (k = 0; k < 3; k++){
					int j = i + num_count * 0.5;
					//printf("j = %d, i = %d\n", j, i);
					//node_surface2[i].pos.x[k] = node_surface2[j].pos.x[k];
					node_surface2[i].del_pos.x[k] = node_surface2[i].del_pos.x[k] + (node_surface2[i].acc.x[k] * 0.5 + node_surface2[j].acc.x[k] * 0.5) * kizami;
					node_surface2[j].del_pos.x[k] = node_surface2[j].del_pos.x[k] + (node_surface2[i].acc.x[k] * 0.5 + node_surface2[j].acc.x[k] * 0.5) * kizami;
					node_surface2[i].pos.x[k] = node_surface2[i].pos.x[k] + node_surface2[i].del_pos.x[k] * kizami + 1.0 / 4.0 * node_surface2[i].acc.x[k] * kizami * kizami + node_surface2[j].del_pos.x[k] * kizami + 1.0 / 4.0 * node_surface2[j].acc.x[k] * kizami * kizami;
					node_surface2[j].pos.x[k] = node_surface2[j].pos.x[k] + node_surface2[i].del_pos.x[k] * kizami + 1.0 / 4.0 * node_surface2[i].acc.x[k] * kizami * kizami + node_surface2[j].del_pos.x[k] * kizami + 1.0 / 4.0 * node_surface2[j].acc.x[k] * kizami * kizami;
				}
			}
		}
#endif
		/// normal mean
#if 1
		double P0P1[3];
		double P0P2[3];
		double P3P2[3];
		double P3P1[3];

		for (i = 0; i < num_count * 0.5; i++){
			for (j = 0; j < 6; j++){
				//printf("i = %d,j = %d,N[j] = %d\n", i, j, node_surface2[i].N[j]);
			}
		}
		for (i = 0; i < num_count * 0.5; i++){
			for (j = 0; j < 6; j++){
				//printf("i = %d, j = %d, T[j] = %d\n", i, j, node_surface2[i].T[j]);
			}
		}
		//printf("node_surface2[8].N[5] = %d\n", node_surface2[8].N[5]);
		for (i = 0; i < num_count; i++){
			if (i != 0 && i != num_count * 0.5 - 1 && i != num_count * 0.5 - side_num && i != num_count * 0.5 && i != num_count - 1 && i != num_count - side_num){
				for (j = 1; j < 5; j++){
					//for (k = 0; k < 3; k++){
					//P0P1[k] = (node_surface2[node_surface2[i].N[j]].pos.x[k] - node_surface2[node_surface2[i].N[j - 1]].pos.x[k]);
					//P0P2[k] = (node_surface2[i].pos.x[k] - node_surface2[node_surface2[i].N[j - 1]].pos.x[k]);
					//P3P2[k] = (node_surface2[i].pos.x[k] - node_surface2[node_surface2[i].N[j + 1]].pos.x[k]);
					//P3P1[k] = (node_surface2[node_surface2[i].N[j]].pos.x[k] - node_surface2[node_surface2[i].N[j + 1]].pos.x[k]);
					P0P1[0] = (node_surface2[node_surface2[i].N[j]].pos.x[0] - node_surface2[node_surface2[i].N[j - 1]].pos.x[0]);
					P0P1[1] = (node_surface2[node_surface2[i].N[j]].pos.x[1] - node_surface2[node_surface2[i].N[j - 1]].pos.x[1]);
					P0P1[2] = (node_surface2[node_surface2[i].N[j]].pos.x[2] - node_surface2[node_surface2[i].N[j - 1]].pos.x[2]);
					P0P2[0] = (node_surface2[i].pos.x[0] - node_surface2[node_surface2[i].N[j - 1]].pos.x[0]);
					P0P2[1] = (node_surface2[i].pos.x[1] - node_surface2[node_surface2[i].N[j - 1]].pos.x[1]);
					P0P2[2] = (node_surface2[i].pos.x[2] - node_surface2[node_surface2[i].N[j - 1]].pos.x[2]);
					P3P2[0] = (node_surface2[i].pos.x[0] - node_surface2[node_surface2[i].N[j + 1]].pos.x[0]);
					P3P2[1] = (node_surface2[i].pos.x[1] - node_surface2[node_surface2[i].N[j + 1]].pos.x[1]);
					P3P2[2] = (node_surface2[i].pos.x[2] - node_surface2[node_surface2[i].N[j + 1]].pos.x[2]);
					P3P1[0] = (node_surface2[node_surface2[i].N[j]].pos.x[0] - node_surface2[node_surface2[i].N[j + 1]].pos.x[0]);
					P3P1[1] = (node_surface2[node_surface2[i].N[j]].pos.x[1] - node_surface2[node_surface2[i].N[j + 1]].pos.x[1]);
					P3P1[2] = (node_surface2[node_surface2[i].N[j]].pos.x[2] - node_surface2[node_surface2[i].N[j + 1]].pos.x[2]);
					//printf("i = %d, p0p1-1 = %f, p0p1-2 = %f, p0p1-3 = %f\n",i, P0P1[0], P0P1[1], P0P1[2]);
					node_surface2[i].cosa[j] = ((P0P1[0] * P0P2[0]) + (P0P1[1] * P0P2[1]) + (P0P1[2] * P0P2[2])) / (sqrt(pow(P0P1[0], 2) + pow(P0P1[1], 2) + pow(P0P1[2], 2)) * sqrt(pow(P0P2[0], 2) + pow(P0P2[1], 2) + pow(P0P2[2], 2)));
					node_surface2[i].cosb[j] = ((P3P2[0] * P3P1[0]) + (P3P2[1] * P3P1[1]) + (P3P2[2] * P3P1[2])) / (sqrt(pow(P3P2[0], 2) + pow(P3P2[1], 2) + pow(P3P2[2], 2)) * sqrt(pow(P3P1[0], 2) + pow(P3P1[1], 2) + pow(P3P1[2], 2)));
					//printf("cosa = %f, cosb = %f, i = %d,j = %d\n", node_surface2[i].cosa[j], node_surface2[i].cosb[j], i, j);
				}
				for (j = 0; j < 1; j++){
					for (k = 0; k < 3; k++){
						/*P0P1[k] = (node_surface2[node_surface2[i].N[0]].pos.x[k] - node_surface2[node_surface2[i].N[5]].pos.x[k]);
						P0P2[k] = (node_surface2[i].pos.x[k] - node_surface2[node_surface2[i].N[5]].pos.x[k]);
						P3P2[k] = (node_surface2[i].pos.x[k] - node_surface2[node_surface2[i].N[j + 1]].pos.x[k]);
						P3P1[k] = (node_surface2[node_surface2[i].N[j]].pos.x[k] - node_surface2[node_surface2[i].N[j + 1]].pos.x[k]);*/
						P0P1[0] = (node_surface2[node_surface2[i].N[j]].pos.x[0] - node_surface2[node_surface2[i].N[5]].pos.x[0]);
						P0P1[1] = (node_surface2[node_surface2[i].N[j]].pos.x[1] - node_surface2[node_surface2[i].N[5]].pos.x[1]);
						P0P1[2] = (node_surface2[node_surface2[i].N[j]].pos.x[2] - node_surface2[node_surface2[i].N[5]].pos.x[2]);
						P0P2[0] = (node_surface2[i].pos.x[0] - node_surface2[node_surface2[i].N[5]].pos.x[0]);
						P0P2[1] = (node_surface2[i].pos.x[1] - node_surface2[node_surface2[i].N[5]].pos.x[1]);
						P0P2[2] = (node_surface2[i].pos.x[2] - node_surface2[node_surface2[i].N[5]].pos.x[2]);
						P3P2[0] = (node_surface2[i].pos.x[0] - node_surface2[node_surface2[i].N[j + 1]].pos.x[0]);
						P3P2[1] = (node_surface2[i].pos.x[1] - node_surface2[node_surface2[i].N[j + 1]].pos.x[1]);
						P3P2[2] = (node_surface2[i].pos.x[2] - node_surface2[node_surface2[i].N[j + 1]].pos.x[2]);
						P3P1[0] = (node_surface2[node_surface2[i].N[j]].pos.x[0] - node_surface2[node_surface2[i].N[j + 1]].pos.x[0]);
						P3P1[1] = (node_surface2[node_surface2[i].N[j]].pos.x[1] - node_surface2[node_surface2[i].N[j + 1]].pos.x[1]);
						P3P1[2] = (node_surface2[node_surface2[i].N[j]].pos.x[2] - node_surface2[node_surface2[i].N[j + 1]].pos.x[2]);
						node_surface2[i].cosa[j] = ((P0P1[0] * P0P2[0]) + (P0P1[1] * P0P2[1]) + (P0P1[2] * P0P2[2])) / (sqrt(pow(P0P1[0], 2) + pow(P0P1[1], 2) + pow(P0P1[2], 2)) * sqrt(pow(P0P2[0], 2) + pow(P0P2[1], 2) + pow(P0P2[2], 2)));
						node_surface2[i].cosb[j] = ((P3P2[0] * P3P1[0]) + (P3P2[1] * P3P1[1]) + (P3P2[2] * P3P1[2])) / (sqrt(pow(P3P2[0], 2) + pow(P3P2[1], 2) + pow(P3P2[2], 2)) * sqrt(pow(P3P1[0], 2) + pow(P3P1[1], 2) + pow(P3P1[2], 2)));
						//printf("cosa = %f, i = %d,j = %d\n", node_surface2[i].cosa[j], i, j);
						//printf("cosa = %f, cosb = %f, i = %d,j = %d\n", node_surface2[i].cosa[j], node_surface2[i].cosb[j], i, j);
					}
				}
				for (j = 5; j < 6; j++){
					for (k = 0; k < 3; k++){
						P0P1[k] = (node_surface2[node_surface2[i].N[j]].pos.x[k] - node_surface2[node_surface2[i].N[j - 1]].pos.x[k]);
						P0P2[k] = (node_surface2[i].pos.x[k] - node_surface2[node_surface2[i].N[j - 1]].pos.x[k]);
						P3P2[k] = (node_surface2[i].pos.x[k] - node_surface2[node_surface2[i].N[0]].pos.x[k]);
						P3P1[k] = (node_surface2[node_surface2[i].N[j]].pos.x[k] - node_surface2[node_surface2[i].N[0]].pos.x[k]);
						/*P0P1[0] = (node_surface2[node_surface2[i].N[j]].pos.x[0] - node_surface2[node_surface2[i].N[j - 1]].pos.x[0]);
						P0P1[1] = (node_surface2[node_surface2[i].N[j]].pos.x[1] - node_surface2[node_surface2[i].N[j - 1]].pos.x[1]);
						P0P1[2] = (node_surface2[node_surface2[i].N[j]].pos.x[2] - node_surface2[node_surface2[i].N[j - 1]].pos.x[2]);
						P0P2[0] = (node_surface2[i].pos.x[0] - node_surface2[node_surface2[i].N[j - 1]].pos.x[0]);
						P0P2[1] = (node_surface2[i].pos.x[1] - node_surface2[node_surface2[i].N[j - 1]].pos.x[1]);
						P0P2[2] = (node_surface2[i].pos.x[2] - node_surface2[node_surface2[i].N[j - 1]].pos.x[2]);
						P3P2[0] = (node_surface2[i].pos.x[0] - node_surface2[node_surface2[i].N[j + 1]].pos.x[0]);
						P3P2[1] = (node_surface2[i].pos.x[1] - node_surface2[node_surface2[i].N[j + 1]].pos.x[1]);
						P3P2[2] = (node_surface2[i].pos.x[2] - node_surface2[node_surface2[i].N[j + 1]].pos.x[2]);
						P3P1[0] = (node_surface2[node_surface2[i].N[j]].pos.x[0] - node_surface2[node_surface2[i].N[j + 1]].pos.x[0]);
						P3P1[1] = (node_surface2[node_surface2[i].N[j]].pos.x[1] - node_surface2[node_surface2[i].N[j + 1]].pos.x[1]);
						P3P1[2] = (node_surface2[node_surface2[i].N[j]].pos.x[2] - node_surface2[node_surface2[i].N[j + 1]].pos.x[2]);*/
						node_surface2[i].cosa[j] = ((P0P1[0] * P0P2[0]) + (P0P1[1] * P0P2[1]) + (P0P1[2] * P0P2[2])) / (sqrt(pow(P0P1[0], 2) + pow(P0P1[1], 2) + pow(P0P1[2], 2)) * sqrt(pow(P0P2[0], 2) + pow(P0P2[1], 2) + pow(P0P2[2], 2)));
						node_surface2[i].cosb[j] = ((P3P2[0] * P3P1[0]) + (P3P2[1] * P3P1[1]) + (P3P2[2] * P3P1[2])) / (sqrt(pow(P3P2[0], 2) + pow(P3P2[1], 2) + pow(P3P2[2], 2)) * sqrt(pow(P3P1[0], 2) + pow(P3P1[1], 2) + pow(P3P1[2], 2)));
						//printf("cosa = %f, cosb = %f, i = %d,j = %d\n", node_surface2[i].cosa[j], node_surface2[i].cosb[j], i, j);
					}
				}
			}
			//else{
			//	if (i < num_count * 0.5){
			//		if (i != 0 && i != num_count * 0.5 - 1 && i != num_count * 0.5 - side_num){
			//			for (j = 1; j < 5; j++){
			//				for (k = 0; k < 3; k++){
			//					P0P1[k] = (node_surface2[node_surface2[i].N[j]].pos.x[k] - node_surface2[node_surface2[i].N[j - 1]].pos.x[k]);
			//					P0P2[k] = (node_surface2[i].pos.x[k] - node_surface2[node_surface2[i].N[j - 1]].pos.x[k]);
			//					P3P2[k] = (node_surface2[i].pos.x[k] - node_surface2[node_surface2[i].N[j + 1]].pos.x[k]);
			//					P3P1[k] = (node_surface2[node_surface2[i].N[j]].pos.x[k] - node_surface2[node_surface2[i].N[j + 1]].pos.x[k]);
			//					node_surface2[i].cosa[j] = ((P0P1[0] * P0P2[0]) + (P0P1[1] * P0P2[1]) + (P0P1[2] * P0P2[2])) / (sqrt(pow(P0P1[0], 2) + pow(P0P1[1], 2) + pow(P0P1[2], 2)) * sqrt(pow(P0P2[0], 2) + pow(P0P2[1], 2) + pow(P0P2[2], 2)));
			//					node_surface2[i].cosb[j] = ((P3P2[0] * P3P1[0]) + (P3P2[1] * P3P1[1]) + (P3P2[2] * P3P1[2])) / (sqrt(pow(P3P2[0], 2) + pow(P3P2[1], 2) + pow(P3P2[2], 2)) * sqrt(pow(P3P1[0], 2) + pow(P3P1[1], 2) + pow(P3P1[2], 2)));
			//					//printf("cosa = %f, cosb = %f, i = %d,j = %d\n", node_surface2[i].cosa[j], node_surface2[i].cosb[j], i, j);
			//				}
			//			}
			//			for (j = 0; j < 1; j++){
			//				for (k = 0; k < 3; k++){
			//					P0P1[k] = (node_surface2[node_surface2[i].N[0]].pos.x[k] - node_surface2[node_surface2[i].N[5]].pos.x[k]);
			//					P0P2[k] = (node_surface2[i].pos.x[k] - node_surface2[node_surface2[i].N[5]].pos.x[k]);
			//					P3P2[k] = (node_surface2[i].pos.x[k] - node_surface2[node_surface2[i].N[j + 1]].pos.x[k]);
			//					P3P1[k] = (node_surface2[node_surface2[i].N[j]].pos.x[k] - node_surface2[node_surface2[i].N[j + 1]].pos.x[k]);
			//					node_surface2[i].cosa[j] = ((P0P1[0] * P0P2[0]) + (P0P1[1] * P0P2[1]) + (P0P1[2] * P0P2[2])) / (sqrt(pow(P0P1[0], 2) + pow(P0P1[1], 2) + pow(P0P1[2], 2)) * sqrt(pow(P0P2[0], 2) + pow(P0P2[1], 2) + pow(P0P2[2], 2)));
			//					node_surface2[i].cosb[j] = ((P3P2[0] * P3P1[0]) + (P3P2[1] * P3P1[1]) + (P3P2[2] * P3P1[2])) / (sqrt(pow(P3P2[0], 2) + pow(P3P2[1], 2) + pow(P3P2[2], 2)) * sqrt(pow(P3P1[0], 2) + pow(P3P1[1], 2) + pow(P3P1[2], 2)));
			//				}
			//			}
			//			for (j = 5; j < 6; j++){
			//				for (k = 0; k < 3; k++){
			//					P0P1[k] = (node_surface2[node_surface2[i].N[j]].pos.x[k] - node_surface2[node_surface2[i].N[j - 1]].pos.x[k]);
			//					P0P2[k] = (node_surface2[i].pos.x[k] - node_surface2[node_surface2[i].N[j - 1]].pos.x[k]);
			//					P3P2[k] = (node_surface2[i].pos.x[k] - node_surface2[node_surface2[i].N[0]].pos.x[k]);
			//					P3P1[k] = (node_surface2[node_surface2[i].N[j]].pos.x[k] - node_surface2[node_surface2[i].N[0]].pos.x[k]);
			//					node_surface2[i].cosa[j] = ((P0P1[0] * P0P2[0]) + (P0P1[1] * P0P2[1]) + (P0P1[2] * P0P2[2])) / (sqrt(pow(P0P1[0], 2) + pow(P0P1[1], 2) + pow(P0P1[2], 2)) * sqrt(pow(P0P2[0], 2) + pow(P0P2[1], 2) + pow(P0P2[2], 2)));
			//					node_surface2[i].cosb[j] = ((P3P2[0] * P3P1[0]) + (P3P2[1] * P3P1[1]) + (P3P2[2] * P3P1[2])) / (sqrt(pow(P3P2[0], 2) + pow(P3P2[1], 2) + pow(P3P2[2], 2)) * sqrt(pow(P3P1[0], 2) + pow(P3P1[1], 2) + pow(P3P1[2], 2)));
			//					//printf("i = %d,j = %d, cosa = %f, cosb = %f\n", i, j, node_surface2[i].cosa[j], node_surface2[i].cosb[j]);
			//				}
			//			}
			//		}
			//	}
			//	else{
			//		if (i != num_count * 0.5 && i != num_count - 1 && i != num_count - side_num){
			//			for (j = 1; j < 5; j++){
			//				for (k = 0; k < 3; k++){
			//					P0P1[k] = (node_surface2[node_surface2[i].N[j]].pos.x[k] - node_surface2[node_surface2[i].N[j - 1]].pos.x[k]);
			//					P0P2[k] = (node_surface2[i].pos.x[k] - node_surface2[node_surface2[i].N[j - 1]].pos.x[k]);
			//					P3P2[k] = (node_surface2[i].pos.x[k] - node_surface2[node_surface2[i].N[j + 1]].pos.x[k]);
			//					P3P1[k] = (node_surface2[node_surface2[i].N[j]].pos.x[k] - node_surface2[node_surface2[i].N[j + 1]].pos.x[k]);
			//					node_surface2[i].cosa[j] = ((P0P1[0] * P0P2[0]) + (P0P1[1] * P0P2[1]) + (P0P1[2] * P0P2[2])) / (sqrt(pow(P0P1[0], 2) + pow(P0P1[1], 2) + pow(P0P1[2], 2)) * sqrt(pow(P0P2[0], 2) + pow(P0P2[1], 2) + pow(P0P2[2], 2)));
			//					node_surface2[i].cosb[j] = ((P3P2[0] * P3P1[0]) + (P3P2[1] * P3P1[1]) + (P3P2[2] * P3P1[2])) / (sqrt(pow(P3P2[0], 2) + pow(P3P2[1], 2) + pow(P3P2[2], 2)) * sqrt(pow(P3P1[0], 2) + pow(P3P1[1], 2) + pow(P3P1[2], 2)));
			//					//printf("cosa = %f, cosb = %f, i = %d,j = %d\n", node_surface2[i].cosa[j], node_surface2[i].cosb[j], i, j);
			//				}
			//			}
			//			for (j = 0; j < 1; j++){
			//				for (k = 0; k < 3; k++){
			//					P0P1[k] = (node_surface2[node_surface2[i].N[0]].pos.x[k] - node_surface2[node_surface2[i].N[5]].pos.x[k]);
			//					P0P2[k] = (node_surface2[i].pos.x[k] - node_surface2[node_surface2[i].N[5]].pos.x[k]);
			//					P3P2[k] = (node_surface2[i].pos.x[k] - node_surface2[node_surface2[i].N[j + 1]].pos.x[k]);
			//					P3P1[k] = (node_surface2[node_surface2[i].N[j]].pos.x[k] - node_surface2[node_surface2[i].N[j + 1]].pos.x[k]);
			//					node_surface2[i].cosa[j] = ((P0P1[0] * P0P2[0]) + (P0P1[1] * P0P2[1]) + (P0P1[2] * P0P2[2])) / (sqrt(pow(P0P1[0], 2) + pow(P0P1[1], 2) + pow(P0P1[2], 2)) * sqrt(pow(P0P2[0], 2) + pow(P0P2[1], 2) + pow(P0P2[2], 2)));
			//					node_surface2[i].cosb[j] = ((P3P2[0] * P3P1[0]) + (P3P2[1] * P3P1[1]) + (P3P2[2] * P3P1[2])) / (sqrt(pow(P3P2[0], 2) + pow(P3P2[1], 2) + pow(P3P2[2], 2)) * sqrt(pow(P3P1[0], 2) + pow(P3P1[1], 2) + pow(P3P1[2], 2)));
			//				}
			//			}
			//			for (j = 5; j < 6; j++){
			//				for (k = 0; k < 3; k++){
			//					P0P1[k] = (node_surface2[node_surface2[i].N[j]].pos.x[k] - node_surface2[node_surface2[i].N[j - 1]].pos.x[k]);
			//					P0P2[k] = (node_surface2[i].pos.x[k] - node_surface2[node_surface2[i].N[j - 1]].pos.x[k]);
			//					P3P2[k] = (node_surface2[i].pos.x[k] - node_surface2[node_surface2[i].N[0]].pos.x[k]);
			//					P3P1[k] = (node_surface2[node_surface2[i].N[j]].pos.x[k] - node_surface2[node_surface2[i].N[0]].pos.x[k]);
			//					node_surface2[i].cosa[j] = ((P0P1[0] * P0P2[0]) + (P0P1[1] * P0P2[1]) + (P0P1[2] * P0P2[2])) / (sqrt(pow(P0P1[0], 2) + pow(P0P1[1], 2) + pow(P0P1[2], 2)) * sqrt(pow(P0P2[0], 2) + pow(P0P2[1], 2) + pow(P0P2[2], 2)));
			//					node_surface2[i].cosb[j] = ((P3P2[0] * P3P1[0]) + (P3P2[1] * P3P1[1]) + (P3P2[2] * P3P1[2])) / (sqrt(pow(P3P2[0], 2) + pow(P3P2[1], 2) + pow(P3P2[2], 2)) * sqrt(pow(P3P1[0], 2) + pow(P3P1[1], 2) + pow(P3P1[2], 2)));
			//					//printf("i = %d,j = %d, cosa = %f, cosb = %f\n", i, j, node_surface2[i].cosa[j], node_surface2[i].cosb[j]);
			//				}
			//			}
			//		}
			//	}
			//}
		}

		for (i = 0; i < num_count; i++){
			if (i != 0 && i != num_count * 0.5 - 1 && i != num_count * 0.5 - side_num && i != num_count * 0.5 && i != num_count - 1 && i != num_count - side_num){
				for (j = 0; j < 6; j++){
					node_surface2[i].cota[j] = node_surface2[i].cosa[j] / sqrt(1 - pow(node_surface2[i].cosa[j], 2));
					node_surface2[i].cotb[j] = node_surface2[i].cosb[j] / sqrt(1 - pow(node_surface2[i].cosb[j], 2));
					//printf("cota = %f, cotb = %f, %d, %d\n", node_surface2[i].cota[j], node_surface2[i].cotb[j], i, j);
					//printf("sqrt1 = %f, sqrt2 = %f, i = %d, j = %d\n", sqrt(1 - pow(node_surface2[i].cosa[j], 2)), sqrt(1 - pow(node_surface2[i].cosb[j], 2)), i , j);
					//printf("sqrt1 = %f, sqrt2 = %f, i = %d, j = %d\n", pow(node_surface2[i].cosa[j], 2), pow(node_surface2[i].cosb[j], 2), i, j);
				}
			}
		}
	//	else{
	//		if (i < num_count * 0.5){
	//			if (i != 0 && i != num_count * 0.5 - 1 && i != num_count * 0.5 - side_num){
	//				//printf("i = %d\n", i);
	//				for (j = 0; j < 6; j++){
	//					node_surface2[i].cota[j] = node_surface2[i].cosa[j] / sqrt(1 - pow(node_surface2[i].cosa[j], 2));
	//					node_surface2[i].cotb[j] = node_surface2[i].cosb[j] / sqrt(1 - pow(node_surface2[i].cosb[j], 2));
	//					//printf("i = %d, j = %d, cota = %f, cotb = %f\n", i, j, node_surface2[i].cota[j], node_surface2[i].cotb[j]);
	//					//printf("sqrt1 = %f, sqrt2 = %f, cosb = %f, i = %d, j = %d\n", sqrt(1 - pow(node_surface2[i].cosa[j], 2)), sqrt(1 - pow(node_surface2[i].cosb[j], 2)), node_surface2[i].cosb[j], i, j);
	//				}
	//			}
	//		}
	//		else{
	//			if (i != num_count * 0.5 && i != num_count - 1 && i != num_count - side_num){
	//				for (j = 0; j < 6; j++){
	//					node_surface2[i].cota[j] = node_surface2[i].cosa[j] / sqrt(1 - pow(node_surface2[i].cosa[j], 2));
	//					node_surface2[i].cotb[j] = node_surface2[i].cosb[j] / sqrt(1 - pow(node_surface2[i].cosb[j], 2));
	//					//printf("i = %d, j = %d, cota = %f, cotb = %f\n", i, j, node_surface2[i].cota[j], node_surface2[i].cotb[j]);
	//				}
	//			}
	//		}
	//	}
	//}
	for (j = 0; j < 6; j++){
		//printf("cosb = %f\n", node_surface2[2].cota[j]);
	}
	/*for (i = 0; i < num_count; i++){
		for (j = 0; j < 6; j++){
			printf("%d, %d, %d\n", node_surface2[i].T[j], i, j);
		}
	}*/
	for (i = 0; i < num_count; i++){
		if (node_surface2[i].edge_flag == 0){
			for (j = 0; j < 6; j++){
				for (k = 0; k < 3; k++){
					node_surface2[i].m_normal[k] = triangle_data[node_surface2[i].T[j]].normal[k] / (sqrt(pow(triangle_data[node_surface2[i].T[j]].normal[0], 2) + pow(triangle_data[node_surface2[i].T[j]].normal[1], 2) + pow(triangle_data[node_surface2[i].T[j]].normal[2], 2)));
					//printf("n1 = %f, n2 = %f, n3 = %f, %d\n", node_surface2[i].m_normal[0], node_surface2[i].m_normal[1], node_surface2[i].m_normal[2], i);
				}
			}
		}
		else{
			if (i < num_count * 0.5){
				if (i != 0 && i != num_count * 0.5 - 1 && i != num_count * 0.5 - side_num){
					for (j = 0; j < 6; j++){
						for (k = 0; k < 3; k++){
							node_surface2[i].m_normal[k] = triangle_data[node_surface2[i].T[j]].normal[k] / (sqrt(pow(triangle_data[node_surface2[i].T[j]].normal[0], 2) + pow(triangle_data[node_surface2[i].T[j]].normal[1], 2) + pow(triangle_data[node_surface2[i].T[j]].normal[2], 2)));
							//printf("n1 = %f, n2 = %f, n3 = %f, %d\n", node_surface2[i].m_normal[0], node_surface2[i].m_normal[1], node_surface2[i].m_normal[2], i);
						}
					}
				}
			}
			else{
				if (i != num_count * 0.5 && i != num_count - 1 && i != num_count - side_num){
					for (j = 0; j < 6; j++){
						for (k = 0; k < 3; k++){
							node_surface2[i].m_normal[k] = triangle_data[node_surface2[i].T[j]].normal[k] / (sqrt(pow(triangle_data[node_surface2[i].T[j]].normal[0], 2) + pow(triangle_data[node_surface2[i].T[j]].normal[1], 2) + pow(triangle_data[node_surface2[i].T[j]].normal[2], 2)));
						}
					}
				}
			}
		}
	}

	/*for (i = 0; i < num_count * 0.5; i++){
		for (j = 0; j < 6; j++){
			printf("xXn = %f, %d\n", (node_surface2[i].pos.x[0] - node_surface2[node_surface2[i].N[j]].pos.x[0]) * node_surface2[i].m_normal[0] , i);
		}
	}*/
	/*for (i = 0; i < num_count * 0.5; i++){
		for (j = 0; j < 6; j++){
			if (node_surface2[i].cota[j] != NULL && node_surface2[i].cotb[j] != NULL){
				printf("cota + cotb = %f, %f, %f, %d, %d\n", (node_surface2[i].cota[j] + node_surface2[i].cotb[j]), node_surface2[i].cota[j], node_surface2[i].cotb[j], i, j);
			}
		}
	}*/
	for (i = 0; i < num_count; i++){
		if (i != 0 && i != num_count * 0.5 - 1 && i != num_count * 0.5 - side_num && i != num_count * 0.5 && i != num_count - 1 && i != num_count - side_num){
			for (j = 0; j < 6; j++){
				node_surface2[i].K += ((node_surface2[i].cota[j] + node_surface2[i].cotb[j]) * 0.25 * ((node_surface2[i].pos.x[0] - node_surface2[node_surface2[i].N[j]].pos.x[0]) * node_surface2[i].m_normal[0]
					+ (node_surface2[i].pos.x[1] - node_surface2[node_surface2[i].N[j]].pos.x[1]) * node_surface2[i].m_normal[1]
					+ (node_surface2[i].pos.x[2] - node_surface2[node_surface2[i].N[j]].pos.x[2]) * node_surface2[i].m_normal[2]));
				//printf("k = %f, %d\n", node_surface2[i].K, i);
				//node_surface2[i].K = node_surface2[i].K;
				//printf("i = %d, K = %f\n", i, node_surface2[i].K);
			}
		}
	}
	//	else{
	//		if (i < num_count * 0.5){
	//			if (i != 0 && i != num_count * 0.5 - 1 && i != num_count * 0.5 - side_num){
	//				for (j = 0; j < 6; j++){
	//					node_surface2[i].K += ((node_surface2[i].cota[j] + node_surface2[i].cotb[j]) * 0.25 * ((node_surface2[i].pos.x[0] - node_surface2[node_surface2[i].N[j]].pos.x[0]) * node_surface2[i].m_normal[0]
	//						+ (node_surface2[i].pos.x[1] - node_surface2[node_surface2[i].N[j]].pos.x[1]) * node_surface2[i].m_normal[1]
	//						+ (node_surface2[i].pos.x[2] - node_surface2[node_surface2[i].N[j]].pos.x[2]) * node_surface2[i].m_normal[2]));
	//					//printf("i = %d, K = %f\n",i, node_surface2[i].K);
	//				}
	//			}
	//		}
	//		else{
	//			if (i != num_count * 0.5 && i != num_count - 1 && i != num_count - side_num){
	//				for (j = 0; j < 6; j++){
	//					node_surface2[i].K += ((node_surface2[i].cota[j] + node_surface2[i].cotb[j]) * 0.25 * ((node_surface2[i].pos.x[0] - node_surface2[node_surface2[i].N[j]].pos.x[0]) * node_surface2[i].m_normal[0]
	//						+ (node_surface2[i].pos.x[1] - node_surface2[node_surface2[i].N[j]].pos.x[1]) * node_surface2[i].m_normal[1]
	//						+ (node_surface2[i].pos.x[2] - node_surface2[node_surface2[i].N[j]].pos.x[2]) * node_surface2[i].m_normal[2]));
	//				}
	//			}
	//		}
	//	}
	//}
	for (i = 0; i < num_count; i++){
		//printf("i = %d, K = %d\n",i, node_surface2[i].K);
	}
#endif
	//for (i = 0; i < num_count; i++){
	//	glPushMatrix();
	//	glColor3d(0.0, 1.0, 1.0);
	//	/*printf("%lf, %lf, %lf\n", node_surface2[i].pos.x[0], node_surface2[i].pos.x[1], node_surface2[i].pos.x[2]);*/
	//	glTranslated((GLdouble)node_surface2[i].pos.x[0], (GLdouble)node_surface2[i].pos.x[2], (GLdouble)node_surface2[i].pos.x[1]);
	//	if (view_con == 1){
	//		glutSolidSphere(0.08, 10, 10); 
	//	}
	//	glPopMatrix();
	//}
	//glPopMatrix();
	GLfloat changing[] = { 0.5, 0.5, 1.0, 1.0 }; // Blue

	for (i = 0; i < num_count; i++){
		//node_surface2[i].color_grad = sqrt(pow(node_surface2[i].acc.x[0], 2.0) + pow(node_surface2[i].acc.x[1], 2.0) + pow(node_surface2[i].acc.x[2], 2.0));
		if (node_surface2[i].color_grad > max){
			max = node_surface2[i].color_grad;
		}
		if (node_surface2[i].color_grad < min){
			min = node_surface2[i].color_grad;
		}
	}
	//printf("node_surface2[0] = %f, %f, %f\n", node_surface2[0].pos.x[0], node_surface2[0].pos.x[1], node_surface2[0].pos.x[2]);
	/*printf("max = %lf min = %lf\n", max, min);*/
	for (i = 0; i < num_count * 0.5; i++){
		glPushMatrix();
		glCullFace(GL_BACK);
		changing[0] = node_surface2[i].K * 2.0;
		changing[1] = 0.05;
		changing[2] = 0.1;
		//printf("changing = %f\n", changing[0]);
		//changing[0] = (node_surface2[i].color_grad - min) / (max - min);
		//changing[1] = 0.0;
		//changing[2] = 1.0 - (node_surface2[i].color_grad) / (max - min);
		//if (node_surface2[i].color_grad < (max - min) / 2.0){
		//	changing[0] = (node_surface2[i].color_grad - min) / ((max + min) / 2.0 - min);
		//	changing[1] = 0.1;
		//	changing[2] = ((max + min) / 2.0 - node_surface2[i].color_grad) / ((max + min) / 2.0 - min);
		//}
		//else{
		//	changing[0] = (max - node_surface2[i].color_grad) / ((max + min) / 2.0 - min);
		//	changing[1] = (node_surface2[i].color_grad - (max + min) / 2.0) / ((max + min) / 2.0 - min);
		//	changing[2] = 0.1;
		//	//changing[0] = 1.0;
		//	//changing[1] = 0.1;
		//	//changing[2] = 0.1;
		//	//printf("%f %f %f\n", changing[0], changing[1], changing[2]);
		//}
		glTranslated((GLdouble)node_surface2[i].pos.x[0], (GLdouble)node_surface2[i].pos.x[2], (GLdouble)node_surface2[i].pos.x[1]);
		/*if (view_con == 1) sphere(node_Radius, 10, changing);
		else if (view_con == 2){
			glMaterialfv(GL_FRONT, GL_DIFFUSE, changing);
			glutSolidCube(node_Radius * 4.0);
		}*/
		glPopMatrix();
	}
	glPushMatrix();
	//glTranslated( - rec_x / 2.0,0.0,  - rec_y / 2.0);
	for (i = 0; i < tri_count; i++){
		if (i < tri_count * 0.5){
			if (triangle_data[i].color == 1){
				glCullFace(GL_FRONT);
			}
			else{
				glCullFace(GL_BACK);
			}
		}
		else if (i >= tri_count * 0.5){
			if (triangle_data[i].color == 1){
				glCullFace(GL_BACK);
			}
			else {
				glCullFace(GL_FRONT);
			}
		}
		//glEnable(GL_BLEND);
		//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		if (view_con == 2){
			//glMaterialfv(GL_FRONT, GL_DIFFUSE, white);
			glColorMaterial(GL_FRONT, GL_DIFFUSE);
		}
		else{
			//GLfloat curvature[] = { 5.0 * node_surface2[i].K, 0.0, 0.0, 0.5 };
			//printf("%d\n", triangle_data[i].t[0]);
			//blue2[0] = node_surface2[triangle_data[i].t[0]].K * 2.0;
			//glMaterialfv(GL_FRONT, GL_DIFFUSE, blue2);
			glColorMaterial(GL_FRONT, GL_DIFFUSE);
		}
		glDisable(GL_LIGHTING);
		glBegin(GL_TRIANGLES);
		//printf("%f %f %f %f\n", blue[0], blue[1], blue[2], blue[3]);
		if (1){
			////glNormal3d(-triangle_data[i].normal[0], -triangle_data[i].normal[2], -triangle_data[i].normal[1]);
			//glNormal3d(triangle_data[i].normal[0], triangle_data[i].normal[2], triangle_data[i].normal[1]);
			////glNormal3d(-triangle_data[i].normal[0],- triangle_data[i].normal[1], -triangle_data[i].normal[2]);
			//glColor3d(node_surface2[triangle_data[i].t[0]].K - 0.3, 0.09, abs(node_surface2[triangle_data[i].t[0]].K - 0.9));
			//glVertex3d(node_surface2[triangle_data[i].t[0]].pos.x[0], node_surface2[triangle_data[i].t[0]].pos.x[2], node_surface2[triangle_data[i].t[0]].pos.x[1]);
			//glColor3d(node_surface2[triangle_data[i].t[1]].K - 0.3, 0.09, abs(node_surface2[triangle_data[i].t[1]].K - 0.9));
			//glVertex3d(node_surface2[triangle_data[i].t[1]].pos.x[0], node_surface2[triangle_data[i].t[1]].pos.x[2], node_surface2[triangle_data[i].t[1]].pos.x[1]);
			//glColor3d(node_surface2[triangle_data[i].t[2]].K - 0.3, 0.09, abs(node_surface2[triangle_data[i].t[2]].K - 0.9));
			//glVertex3d(node_surface2[triangle_data[i].t[2]].pos.x[0], node_surface2[triangle_data[i].t[2]].pos.x[2], node_surface2[triangle_data[i].t[2]].pos.x[1]);
			////glMaterialfv(GL_FRONT, GL_DIFFUSE,);
			for (j = 0; j < 3; j++){
				if (node_surface2[triangle_data[i].t[j]].K < 0){
					glColor3d(0.0, 1.0 + node_surface2[triangle_data[i].t[j]].K * 0.05, 0.0 - node_surface2[triangle_data[i].t[j]].K * 0.05);
					glVertex3d(node_surface2[triangle_data[i].t[j]].pos.x[0], node_surface2[triangle_data[i].t[j]].pos.x[2], node_surface2[triangle_data[i].t[j]].pos.x[1]);
				}
				if (node_surface2[triangle_data[i].t[j]].K < 0){
					glColor3d(0.0 + node_surface2[triangle_data[i].t[j]].K * 0.05, 1.0 - node_surface2[triangle_data[i].t[j]].K * 0.05, 1.0);
					glVertex3d(node_surface2[triangle_data[i].t[j]].pos.x[0], node_surface2[triangle_data[i].t[j]].pos.x[2], node_surface2[triangle_data[i].t[j]].pos.x[1]);
				}
				else{
					glColor3d(0.0 + node_surface2[triangle_data[i].t[j]].K * 0.05, 1.0 - node_surface2[triangle_data[i].t[j]].K * 0.05, 1.0 - node_surface2[triangle_data[i].t[j]].K * 0.05);
					glVertex3d(node_surface2[triangle_data[i].t[j]].pos.x[0], node_surface2[triangle_data[i].t[j]].pos.x[2], node_surface2[triangle_data[i].t[j]].pos.x[1]);
				}
			}
		}
		glEnd();
	}
	glPopMatrix();
}
void idle(void)
{
	glutPostRedisplay();
}
void display(void)
{

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	if (MouseFlagLeft){
		if (View_point_flag) View_control(false);
		else View_control2(false);
	}
	else if (MouseFlagRight){
		if (View_point_flag) View_control(true);
		else View_control2(true);
	}
	if (up_flag) View_control_up_down(true);
	if (down_flag) View_control_up_down(false);

	gluLookAt(View_from[0], View_from[1], View_from[2], View_to[0], View_to[1], View_to[2], 0.0, 1.0, 0.0);
	glViewport(0, 0, w_view * 2.0 / 3.0, h_view);
	glPushMatrix();
	node_simulation(1);
	glPopMatrix();


	//for (i = 0; i < num_count; i++){
	//		glPushMatrix();
	//		glColor3d(0.0, 1.0, 1.0);
	//		/*printf("%lf, %lf, %lf\n", node_surface2[i].pos.x[0], node_surface2[i].pos.x[1], node_surface2[i].pos.x[2]);*/
	//		glTranslated((GLdouble)node_surface2[i].pos.x[0], (GLdouble)node_surface2[i].pos.x[2], (GLdouble)node_surface2[i].pos.x[1]);
	//		glutSolidSphere(0.08, 10, 10);
	//		glPopMatrix();
	//	}
	//glPopMatrix();
	//glPushMatrix();

	//	for (i = 0; i < tri_count; i++){
	//		if (triangle_data[i].color == 1){
	//				glCullFace(GL_FRONT);
	//			}
	//			else{
	//				glCullFace(GL_BACK);
	//			}
	//			glEnable(GL_BLEND);
	//			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//			glBegin(GL_TRIANGLES);
	//			//printf("%f %f %f %f\n", blue[0], blue[1], blue[2], blue[3]);
	//			//glNormal3d(-triangle_data[i].normal[0], -triangle_data[i].normal[2], -triangle_data[i].normal[1]);
	//			glNormal3d(triangle_data[i].normal[0], triangle_data[i].normal[2], triangle_data[i].normal[1]);
	//			//glNormal3d(-triangle_data[i].normal[0],- triangle_data[i].normal[1], -triangle_data[i].normal[2]);
	//			glVertex3d(node_surface2[triangle_data[i].t[0]].pos.x[0], node_surface2[triangle_data[i].t[0]].pos.x[2], node_surface2[triangle_data[i].t[0]].pos.x[1]);
	//			glVertex3d(node_surface2[triangle_data[i].t[1]].pos.x[0], node_surface2[triangle_data[i].t[1]].pos.x[2], node_surface2[triangle_data[i].t[1]].pos.x[1]);
	//			glVertex3d(node_surface2[triangle_data[i].t[2]].pos.x[0], node_surface2[triangle_data[i].t[2]].pos.x[2], node_surface2[triangle_data[i].t[2]].pos.x[1]);
	//			/*glVertex3d(node_surface2[triangle_data[i].t[0]].pos.x[0] - rec_x / 2.0, node_surface2[triangle_data[i].t[0]].pos.x[2], node_surface2[triangle_data[i].t[0]].pos.x[1] - rec_y / 2.0);
	//			glVertex3d(node_surface2[triangle_data[i].t[1]].pos.x[0] - rec_x / 2.0, node_surface2[triangle_data[i].t[1]].pos.x[2], node_surface2[triangle_data[i].t[1]].pos.x[1] - rec_y / 2.0);
	//			glVertex3d(node_surface2[triangle_data[i].t[2]].pos.x[0] - rec_x / 2.0, node_surface2[triangle_data[i].t[2]].pos.x[2], node_surface2[triangle_data[i].t[2]].pos.x[1] - rec_y / 2.0);*/
	//		}
	//	glEnd();
	//glPopMatrix();

	glutSwapBuffers();

}
void mouse(int button, int state, int x, int y)
{
	switch (button) {
	case GLUT_LEFT_BUTTON:
		switch (state) {
		case GLUT_UP:
			if (MouseFlagLeft){
				MouseFlagLeft = false;
			}
			break;
		case GLUT_DOWN:
			MouseFlagLeft = true;
			if (x < window_size_x * 2 / 3) View_point_flag = true;
			else View_point_flag = false;
			break;
		default:
			break;
		}
		break;
	case GLUT_MIDDLE_BUTTON:
		switch (state) {
		case GLUT_UP:
			if (MouseFlagRight) MouseFlagMiddle = false;
			break;
		case GLUT_DOWN:
			MouseFlagMiddle = true;
			break;
		default:
			break;
		}
		break;
	case GLUT_RIGHT_BUTTON:
		switch (state) {
		case GLUT_UP:
			if (MouseFlagRight) MouseFlagRight = false;
			break;
		case GLUT_DOWN:
			MouseFlagRight = true;
			if (x < window_size_x * 2 / 3) View_point_flag = true;
			else View_point_flag = false;
			break;
		default:
			break;
		}
		break;
	default:
		break;
	}
}
void resize(int w, int h)
{
	w_view = w;
	h_view = h;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(50.0, (double)w / (double)h * 2.0 / 3.0, 1.0, 1000.0);

	glMatrixMode(GL_MODELVIEW);

}
void keyboard(unsigned char key, int x, int y){
	switch (key){
	case 'r':
		close_flag = true;
		break;
	case 'e':
		close_flag = false;
		break;
	case 'w':
		open_flag = true;
		break;
	case 'q':
		open_flag = false;
		break;
	case 'y':
		close_flag_n = true;
		break;
	case 'u':
		close_flag_n = false;
		break;
	case 'i':
		open_flag_n = true;
		break;
	case 'o':
		open_flag_n = false;
		break;
	}
}
void init(){

	glClearColor(1.0, 1.0, 1.0, 1.0);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	GLfloat direction[] = { 0.0, 1.0, 0.0 };
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);//アルファの設定
	glEnable(GL_BLEND);//アルファのブレンド有効
	glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, direction);
	glLightfv(GL_LIGHTING, GL_SPOT_DIRECTION, direction);
	glDisable(GL_LIGHT2);
	glDisable(GL_LIGHT3);
	glDisable(GL_LIGHT4);

}
int main(int argc, char *argv[])
{
	//get_info();

	glutInit(&argc, argv);
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(window_size_x, window_size_y);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutCreateWindow(argv[0]);
	glutInitWindowPosition(0, 0);
	glutDisplayFunc(display);
	glutReshapeFunc(resize);
	glutIdleFunc(idle);
	glutMouseFunc(mouse);
	glutKeyboardFunc(keyboard);
	/*node_simulation();*/
	init();
	glutMainLoop();

	return 0;

}




