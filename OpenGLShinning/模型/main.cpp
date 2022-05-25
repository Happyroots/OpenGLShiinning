//#include "glew.h"
//#include "freeglut.h"
//#include "../LoadShaders.h"
//#include "../vmath.h"
//#include <glad/glad.h>
//#include <GLFW/glfw3.h>
#include "mesh.h"
#include <fstream>
#include <math.h>
GLuint program[8];
Model *ourModel;
GLuint skyboxVao[1];
GLuint skyboxVbo[1];
GLuint skyboxIbo[1];
GLuint cubemapTexture;
unsigned int loadCubemap(vector<std::string> faces);
void initRC() {
	//创建加载着色程序
	{
		ShaderInfo shaders0[3] =
		{
			{ GL_VERTEX_SHADER, "res/VAO.vert" },
			{ GL_FRAGMENT_SHADER, "res/VAO.frag" },
			{ GL_NONE, NULL }
		};

		program[0] = LoadShaders(shaders0); //装载，添加（Att）？ 编译， 链接
		glUseProgram(program[0]); //激活

		ShaderInfo shaders1[3] =
		{
			{ GL_VERTEX_SHADER, "res/VAO1.vert" },
			{ GL_FRAGMENT_SHADER, "res/VAO1.frag" },
			{ GL_NONE, NULL }
		};

		program[1] = LoadShaders(shaders1); //装载，添加（Att）？ 编译， 链接
		glUseProgram(program[1]); //激活
	}
	//立方体贴图有一个非常有用的特性，
	//它可以通过一个方向向量来进行索引/采样。
	float skyboxVertices[] = {
		// positions          
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f
	};

	glGenVertexArrays(1, skyboxVao);
	glGenBuffers(1, skyboxVbo);
	glBindVertexArray(skyboxVao[0]);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	vector<std::string> faces
	{
		"res/skybox/right.jpg",
		"res/skybox/left.jpg",
		"res/skybox/top.jpg",
		"res/skybox/bottom.jpg",
		"res/skybox/front.jpg",
		"res/skybox/back.jpg"
	};
	cubemapTexture = loadCubemap(faces);

	//没有着色器程序，无法使用纹理 VAO等功能？
	ifstream  ifs("res/模型路径.txt");
	string path;
	if (ifs) {
		ifs >> path;
	}
	ifs.close();
	ourModel = new Model(path.c_str());
	glUseProgram(program[0]);
	glUniform1f(glGetUniformLocation(program[0], "material.shininess"), 32.0f);

	glUniform3f(glGetUniformLocation(program[0], "dirLight.direction"), -0.2f, -1.0f, -0.3f); //-0.2f, -1.0f, -0.3fSpotDir 4聚光的方向向量 camera.Front
	glUniform3f(glGetUniformLocation(program[0], "dirLight.ambient"), 0.2f, 0.2f, 0.2f); //0.05f, 0.05f, 0.05f
	glUniform3f(glGetUniformLocation(program[0], "dirLight.diffuse"), 0.2f, 0.2f, 0.2f); //估计是显示器的问题
	glUniform3f(glGetUniformLocation(program[0], "dirLight.specular"), 0.1f, 0.1f, 0.1f);

	std::vector<glm::vec3> pointLightPositions = {
		glm::vec3(0.7f,  0.2f,  2.0f),
		glm::vec3(2.3f, -3.3f, -4.0f), //前方右下角
		glm::vec3(-4.0f,  2.0f, -12.0f),
		glm::vec3(0.0f,  0.0f, 0.0f)
	};
	glUniform3f(glGetUniformLocation(program[0], "pointLights[0].position"), pointLightPositions[0].x, pointLightPositions[0].y, pointLightPositions[0].z); //SpotDir 4聚光的方向向量 camera.Front
	glUniform3f(glGetUniformLocation(program[0], "pointLights[0].ambient"), 0.05f, 0.05f, 0.05f);
	glUniform3f(glGetUniformLocation(program[0], "pointLights[0].diffuse"), 0.8f, 0.8f, 0.8f);
	glUniform3f(glGetUniformLocation(program[0], "pointLights[0].specular"), 0.5f, 0.5f, 0.5f);
	glUniform1f(glGetUniformLocation(program[0], "pointLights[0].constant"), 1.0f); //
	glUniform1f(glGetUniformLocation(program[0], "pointLights[0].linear"), 0.07f); //0.9
	glUniform1f(glGetUniformLocation(program[0], "pointLights[0].quadratic"), 0.017f);//0.032

	glUniform3f(glGetUniformLocation(program[0], "pointLights[1].position"), pointLightPositions[1].x, pointLightPositions[1].y, pointLightPositions[1].z); //SpotDir 4聚光的方向向量 camera.Front
	glUniform3f(glGetUniformLocation(program[0], "pointLights[1].ambient"), 0.5f, 0.5f, 0.5f);
	glUniform3f(glGetUniformLocation(program[0], "pointLights[1].diffuse"), 0.8f, 0.8f, 0.8f);
	glUniform3f(glGetUniformLocation(program[0], "pointLights[1].specular"), 0.5f, 0.5f, 0.5f);
	glUniform1f(glGetUniformLocation(program[0], "pointLights[1].constant"), 1.0f); //
	glUniform1f(glGetUniformLocation(program[0], "pointLights[1].linear"), 0.07f); //0.9
	glUniform1f(glGetUniformLocation(program[0], "pointLights[1].quadratic"), 0.017f);//0.032

	glUniform3f(glGetUniformLocation(program[0], "pointLights[2].position"), pointLightPositions[2].x, pointLightPositions[2].y, pointLightPositions[2].z); //SpotDir 4聚光的方向向量 camera.Front
	glUniform3f(glGetUniformLocation(program[0], "pointLights[2].ambient"), 0.05f, 0.05f, 0.05f);
	glUniform3f(glGetUniformLocation(program[0], "pointLights[2].diffuse"), 0.8f, 0.8f, 0.8f);
	glUniform3f(glGetUniformLocation(program[0], "pointLights[2].specular"), 0.5f, 0.5f, 0.5f);
	glUniform1f(glGetUniformLocation(program[0], "pointLights[2].constant"), 1.0f); //
	glUniform1f(glGetUniformLocation(program[0], "pointLights[2].linear"), 0.07f); //0.9
	glUniform1f(glGetUniformLocation(program[0], "pointLights[2].quadratic"), 0.017f);//0.032

	glUniform3f(glGetUniformLocation(program[0], "pointLights[3].position"), pointLightPositions[3].x, pointLightPositions[3].y, pointLightPositions[3].z); //SpotDir 4聚光的方向向量 camera.Front
	glUniform3f(glGetUniformLocation(program[0], "pointLights[3].ambient"), 0.05f, 0.05f, 0.05f);
	glUniform3f(glGetUniformLocation(program[0], "pointLights[3].diffuse"), 0.8f, 0.8f, 0.8f);
	glUniform3f(glGetUniformLocation(program[0], "pointLights[3].specular"), 0.5f, 0.5f, 0.5f);
	glUniform1f(glGetUniformLocation(program[0], "pointLights[3].constant"), 1.0f); //
	glUniform1f(glGetUniformLocation(program[0], "pointLights[3].linear"), 0.07f); //0.9
	glUniform1f(glGetUniformLocation(program[0], "pointLights[3].quadratic"), 0.017f);//0.032

	glUniform3f(glGetUniformLocation(program[0], "viewPos"), 0.0f, 0.0f, 20.0f);
	glUniform3f(glGetUniformLocation(program[0], "spotLight.position"), 0.0f, 0.0f, 20.0f); //camera.Position xyz坐标不对应
	glUniform3f(glGetUniformLocation(program[0], "spotLight.direction"), 0.0f, 0.0f, -15.0f); //SpotDir 4聚光的方向向量 camera.Front
	//glUniform3f(glGetUniformLocation(program[0], "spotLight.ambient"), 0.05f, 0.05f, 0.05f);
	//glUniform3f(glGetUniformLocation(program[0], "spotLight.diffuse"), 0.05f, 0.05f, 0.05f);
	glUniform3f(glGetUniformLocation(program[0], "spotLight.specular"), 0.5f, 0.5f, 0.5f);
	glUniform1f(glGetUniformLocation(program[0], "spotLight.constant"), 1.0f);
	glUniform1f(glGetUniformLocation(program[0], "spotLight.linear"), 0.07f);
	glUniform1f(glGetUniformLocation(program[0], "spotLight.quadratic"), 0.017f);
	glUniform1f(glGetUniformLocation(program[0], "spotLight.cutOff"), glm::cos(glm::radians(5.5f))); //
	glUniform1f(glGetUniformLocation(program[0], "spotLight.outerCutOff"), glm::cos(glm::radians(6.0f))); //12.5

}

clock_t t;

void display(){
	// render
	// ------
	glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	// don't forget to enable shader before setting uniforms
	glUseProgram(program[0]);
	 
	// view/projection transformations
	glm::mat4 projection = glm::perspective(glm::radians((float)45.0f), (float)640 / (float)640, 0.1f, 100.0f); //camera.Zoom SCR_WIDTH SCR_HEIGHT
	glm::mat4 view = glm::lookAt(
		glm::vec3(0.0f, 0.0f, 20.0f),//摄像机的位置
		glm::vec3(0.0f, 0.0f, 0.0f), //视点位置
		glm::vec3(0.0f, 1.0f, 0.0f) //上方向; 
	);  //camera.GetViewMatrix()

	glUniformMatrix4fv(glGetUniformLocation(program[0], "projection"), 1, GL_FALSE, glm::value_ptr(projection));
	glUniformMatrix4fv(glGetUniformLocation(program[0], "view"), 1, GL_FALSE, glm::value_ptr(view));
	
	// render the loaded model
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
	model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it down
	model = glm::rotate(model, glm::radians((float)t), glm::vec3(0.0f, 5.0f, 0.0f));													//ourShader.setMat4("model", model);
	glUniformMatrix4fv(glGetUniformLocation(program[0], "model"), 1, GL_FALSE, glm::value_ptr(model));
	ourModel->Draw(program[0]); //ourShader




// configure global opengl state
// -----------------------------
	// ... 设置观察和投影矩阵
	glm::mat4 skyboxview = glm::lookAt(
		glm::vec3(20.0f, 0.0f, 00.0f),//摄像机的位置
		glm::vec3(0.0f, 0.0f, 0.0f),  //视点位置
		glm::vec3(0.0f, 1.0f, 0.0f) //上方向; 
	);  //camera.GetViewMatrix() 
	//这样不论玩家移动了多远，天空盒都不会变近，让玩家产生周围环境非常大的印象
	skyboxview = glm::mat4(glm::mat3(skyboxview)); //这将移除任何的位移，但保留旋转变换，让玩家仍然能够环顾场景。
	glm::mat4 skyboxprojection = projection;
	// draw skybox as last
	//提前深度测试(Early Depth Testing)轻松 *丢弃* 掉的片段能够节省我们很多宝贵的带宽
	glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
	glUseProgram(program[1]);
	glUniformMatrix4fv(glGetUniformLocation(program[1], "view"), 1, GL_FALSE, glm::value_ptr(skyboxview));
	glUniformMatrix4fv(glGetUniformLocation(program[1], "projection"), 1, GL_FALSE, glm::value_ptr(skyboxprojection));
	glBindVertexArray(skyboxVao[0]);
	glUniform1i(glGetUniformLocation(program[1], "skybox"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
	glDepthFunc(GL_LESS); // set depth function back to default
	//功能不支持？ 代码正确 但是文件不全应该 
	//	照片必须是正方形





	glutSwapBuffers();
}

void idle()
{
	t = clock() / 50 % 360;   //360度一循环，可省略吧？ 返回程序执行起（一般为程序的开头），处理器时钟所使用的时间。
	glm::vec3 lightColor;
	//sin(t * 2.0f) > 0 ? lightColor.x = sin(t * 2.0f) : lightColor.x = 0.5;
	//sin(t * 0.7f) > 0 ? lightColor.y = sin(t * 0.7f) : lightColor.y = 0.5;
	//sin(t * 1.3f) > 0 ? lightColor.z = sin(t * 1.3f) : lightColor.z = 0.5;
	lightColor.x = sin(t * 2.0f);
	lightColor.y = sin(t * 0.7f);
	lightColor.z = sin(t * 1.3f);
	glm::vec3  diffuseColor = lightColor * glm::vec3(0.5f); // 降低影响
	glm::vec3  ambientColor = diffuseColor * glm::vec3(0.2f); // 很低的影响
	glUseProgram(program[0]);

	//cout << lightColor.x << " " << lightColor.y << " " << lightColor.z << endl;
	
	//为什么会变黑？ 居然有负值！不是因为这个 少传了一个变量
	glUniform3f(glGetUniformLocation(program[0], "spotLight.ambient"), ambientColor.x, ambientColor.y, ambientColor.z);
	glUniform3f(glGetUniformLocation(program[0], "spotLight.diffuse"), diffuseColor.x, diffuseColor.y, diffuseColor.z);
	//glUniform3f(glGetUniformLocation(program[0], "spotLight.ambient"), 0.05f, 0.05f, 0.05f);
	//glUniform3f(glGetUniformLocation(program[0], "spotLight.diffuse"), 0.05f, 0.05f, 0.05f);

	glutPostRedisplay(); //重绘函数 起什么作用？
}

void reshape(int width, int height)
{
	glViewport(0, 0, width, height);
}
void Clear() {

}

int main(int argc, char* argv[]) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutInitWindowSize(640, 640);
	glutInitWindowPosition(300, 100);
	int mainwnd = glutCreateWindow("大老虎喵喵喵？");
	if (glewInit() != GLEW_OK) 
	{
		return -1;
	}
	initRC();

	glutDisplayFunc(display);
	glutIdleFunc(idle);
	glutReshapeFunc(reshape);
	glutMainLoop();

	Clear();
	return 0;
}

unsigned int loadCubemap(vector<std::string> faces)
{//加载立方体贴图
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	int width, height, nrChannels;
	for (unsigned int i = 0; i < faces.size(); i++)
	{
		unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
			);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
			stbi_image_free(data);
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	return textureID;
}