#include <stdio.h>
#include <GLFW/glfw3.h>
#include <stdlib.h>
#include<iostream>
#include<string.h>
#include <Winsock2.h>
#include<vector>
#pragma comment(lib, "WS2_32")  // 链接到WS2_32.lib
using namespace std;
char recvBuf[9999];

static void inputK(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) glfwSetWindowShouldClose(window, GL_TRUE);
}

static void error_callback(int error, const char* description) {
	fputs(description, stderr);
}

vector<string> split(const string& s, const string& seperator) {
	vector<string> result;
	typedef string::size_type string_size;
	string_size i = 0;

	while (i != s.size()) {
		//找到字符串中首个不等于分隔符的字母；
		int flag = 0;
		while (i != s.size() && flag == 0) {
			flag = 1;
			for (string_size x = 0; x < seperator.size(); ++x)
				if (s[i] == seperator[x]) {
					++i;
					flag = 0;
					break;
				}
		}

		//找到又一个分隔符，将两个分隔符之间的字符串取出；
		flag = 0;
		string_size j = i;
		while (j != s.size() && flag == 0) {
			for (string_size x = 0; x < seperator.size(); ++x)
				if (s[j] == seperator[x]) {
					flag = 1;
					break;
				}
			if (flag == 0)
				++j;
		}
		if (i != j) {
			result.push_back(s.substr(i, j - i));
			i = j;
		}
	}
	return result;
}
int getY(int y) {
	return 800 * (y) / 999;
}

void drawLine(vector<vector<int>> linedatas,int wWidth,int wHeight) {
	GLFWwindow* window;
	glfwSetErrorCallback(error_callback);
	if (!glfwInit())
		exit(EXIT_FAILURE);
	window = glfwCreateWindow(wWidth*4, wHeight, "line chart", NULL, NULL);
	if (!window) {
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);
	glfwSetKeyCallback(window, inputK);

	while (!glfwWindowShouldClose(window)) {
		float ratio;
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);
		ratio = width / (float)height;
		glViewport(0, 0, width, height);
		glClear(GL_COLOR_BUFFER_BIT);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();


		glOrtho(0, width, height, 0, 0, 1);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		//attempt to draw lines
		glBegin(GL_LINES);
		//glVertex2f(0, 0);
		//glVertex2f(200, 400);
		//glVertex2f(200, 400);
		//glVertex2f(200, 0);

		for (int i = 0; i < linedatas.size() - 2; i++) {
			glVertex2f(i, 1000- getY(linedatas[i][1]));
			glVertex2f(i+1, 1000-getY(linedatas[i + 1][1]));
			//cout << linedatas[i][1] << endl;
		}
		glEnd();
		glfwSwapBuffers(window);
		glfwPollEvents();

	}

	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);

}
void main()
{
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;

	wVersionRequested = MAKEWORD(1, 1);

	err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0) {
		return;
	}

	if (LOBYTE(wsaData.wVersion) != 1 ||
		HIBYTE(wsaData.wVersion) != 1) {
		WSACleanup();
		return;
	}
	SOCKET sockClient = socket(AF_INET, SOCK_STREAM, 0);

	SOCKADDR_IN addrSrv;
	addrSrv.sin_addr.S_un.S_addr = inet_addr("192.168.1.103");
	addrSrv.sin_family = AF_INET;
	addrSrv.sin_port = htons(6000);
	connect(sockClient, (SOCKADDR*)& addrSrv, sizeof(SOCKADDR));
	int width = 200;
	int height = 1000;
	int tStart = 1;
	int tEnd = 10000;
	char sendBuf[30];
	sprintf(sendBuf,"%d-%d-%d-%d", width,height,tStart,tEnd);
	cout << sendBuf<<endl;


	send(sockClient, sendBuf, strlen(sendBuf) + 1, 0);
	
	int len=recv(sockClient, recvBuf, 9999, 0);
	string str(recvBuf, 0, len);
	cout << str << endl;
	vector<string> strings = split(str, "*");
	vector<vector<string>> allString;
	int allSize= atoi(strings[0].c_str());
	for (int i = 1; i < strings.size(); i++) {
		allString.push_back(split(strings[i], "+"));
	}
	vector<vector<int>> drawLineData;
	for (int i = 0; i < allString.size(); i++) {
		vector<string> tempS = allString[i];
		vector<int> tempV;
		for (int j = 0; j < tempS.size(); j++) {
			tempV.push_back(atoi(tempS[j].c_str()));
		}
		drawLineData.push_back(tempV);
	}
	cout << drawLineData[drawLineData.size() - 1][1] << "ddd" << endl;

	closesocket(sockClient);
	WSACleanup();
	drawLine(drawLineData, width, height);
}
