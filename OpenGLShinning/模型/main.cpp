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
	//����������ɫ����
	{
		ShaderInfo shaders0[3] =
		{
			{ GL_VERTEX_SHADER, "res/VAO.vert" },
			{ GL_FRAGMENT_SHADER, "res/VAO.frag" },
			{ GL_NONE, NULL }
		};

		program[0] = LoadShaders(shaders0); //װ�أ���ӣ�Att���� ���룬 ����
		glUseProgram(program[0]); //����

		ShaderInfo shaders1[3] =
		{
			{ GL_VERTEX_SHADER, "res/VAO1.vert" },
			{ GL_FRAGMENT_SHADER, "res/VAO1.frag" },
			{ GL_NONE, NULL }
		};

		program[1] = LoadShaders(shaders1); //װ�أ���ӣ�Att���� ���룬 ����
		glUseProgram(program[1]); //����
	}
	//��������ͼ��һ���ǳ����õ����ԣ�
	//������ͨ��һ��������������������/������
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

	//û����ɫ�������޷�ʹ������ VAO�ȹ��ܣ�
	ifstream  ifs("res/ģ��·��.txt");
	string path;
	if (ifs) {
		ifs >> path;
	}
	ifs.close();
	ourModel = new Model(path.c_str());
	glUseProgram(program[0]);
	glUniform1f(glGetUniformLocation(program[0], "material.shininess"), 32.0f);

	glUniform3f(glGetUniformLocation(program[0], "dirLight.direction"), -0.2f, -1.0f, -0.3f); //-0.2f, -1.0f, -0.3fSpotDir 4�۹�ķ������� camera.Front
	glUniform3f(glGetUniformLocation(program[0], "dirLight.ambient"), 0.2f, 0.2f, 0.2f); //0.05f, 0.05f, 0.05f
	glUniform3f(glGetUniformLocation(program[0], "dirLight.diffuse"), 0.2f, 0.2f, 0.2f); //��������ʾ��������
	glUniform3f(glGetUniformLocation(program[0], "dirLight.specular"), 0.1f, 0.1f, 0.1f);

	std::vector<glm::vec3> pointLightPositions = {
		glm::vec3(0.7f,  0.2f,  2.0f),
		glm::vec3(2.3f, -3.3f, -4.0f), //ǰ�����½�
		glm::vec3(-4.0f,  2.0f, -12.0f),
		glm::vec3(0.0f,  0.0f, 0.0f)
	};
	glUniform3f(glGetUniformLocation(program[0], "pointLights[0].position"), pointLightPositions[0].x, pointLightPositions[0].y, pointLightPositions[0].z); //SpotDir 4�۹�ķ������� camera.Front
	glUniform3f(glGetUniformLocation(program[0], "pointLights[0].ambient"), 0.05f, 0.05f, 0.05f);
	glUniform3f(glGetUniformLocation(program[0], "pointLights[0].diffuse"), 0.8f, 0.8f, 0.8f);
	glUniform3f(glGetUniformLocation(program[0], "pointLights[0].specular"), 0.5f, 0.5f, 0.5f);
	glUniform1f(glGetUniformLocation(program[0], "pointLights[0].constant"), 1.0f); //
	glUniform1f(glGetUniformLocation(program[0], "pointLights[0].linear"), 0.07f); //0.9
	glUniform1f(glGetUniformLocation(program[0], "pointLights[0].quadratic"), 0.017f);//0.032

	glUniform3f(glGetUniformLocation(program[0], "pointLights[1].position"), pointLightPositions[1].x, pointLightPositions[1].y, pointLightPositions[1].z); //SpotDir 4�۹�ķ������� camera.Front
	glUniform3f(glGetUniformLocation(program[0], "pointLights[1].ambient"), 0.5f, 0.5f, 0.5f);
	glUniform3f(glGetUniformLocation(program[0], "pointLights[1].diffuse"), 0.8f, 0.8f, 0.8f);
	glUniform3f(glGetUniformLocation(program[0], "pointLights[1].specular"), 0.5f, 0.5f, 0.5f);
	glUniform1f(glGetUniformLocation(program[0], "pointLights[1].constant"), 1.0f); //
	glUniform1f(glGetUniformLocation(program[0], "pointLights[1].linear"), 0.07f); //0.9
	glUniform1f(glGetUniformLocation(program[0], "pointLights[1].quadratic"), 0.017f);//0.032

	glUniform3f(glGetUniformLocation(program[0], "pointLights[2].position"), pointLightPositions[2].x, pointLightPositions[2].y, pointLightPositions[2].z); //SpotDir 4�۹�ķ������� camera.Front
	glUniform3f(glGetUniformLocation(program[0], "pointLights[2].ambient"), 0.05f, 0.05f, 0.05f);
	glUniform3f(glGetUniformLocation(program[0], "pointLights[2].diffuse"), 0.8f, 0.8f, 0.8f);
	glUniform3f(glGetUniformLocation(program[0], "pointLights[2].specular"), 0.5f, 0.5f, 0.5f);
	glUniform1f(glGetUniformLocation(program[0], "pointLights[2].constant"), 1.0f); //
	glUniform1f(glGetUniformLocation(program[0], "pointLights[2].linear"), 0.07f); //0.9
	glUniform1f(glGetUniformLocation(program[0], "pointLights[2].quadratic"), 0.017f);//0.032

	glUniform3f(glGetUniformLocation(program[0], "pointLights[3].position"), pointLightPositions[3].x, pointLightPositions[3].y, pointLightPositions[3].z); //SpotDir 4�۹�ķ������� camera.Front
	glUniform3f(glGetUniformLocation(program[0], "pointLights[3].ambient"), 0.05f, 0.05f, 0.05f);
	glUniform3f(glGetUniformLocation(program[0], "pointLights[3].diffuse"), 0.8f, 0.8f, 0.8f);
	glUniform3f(glGetUniformLocation(program[0], "pointLights[3].specular"), 0.5f, 0.5f, 0.5f);
	glUniform1f(glGetUniformLocation(program[0], "pointLights[3].constant"), 1.0f); //
	glUniform1f(glGetUniformLocation(program[0], "pointLights[3].linear"), 0.07f); //0.9
	glUniform1f(glGetUniformLocation(program[0], "pointLights[3].quadratic"), 0.017f);//0.032

	glUniform3f(glGetUniformLocation(program[0], "viewPos"), 0.0f, 0.0f, 20.0f);
	glUniform3f(glGetUniformLocation(program[0], "spotLight.position"), 0.0f, 0.0f, 20.0f); //camera.Position xyz���겻��Ӧ
	glUniform3f(glGetUniformLocation(program[0], "spotLight.direction"), 0.0f, 0.0f, -15.0f); //SpotDir 4�۹�ķ������� camera.Front
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
		glm::vec3(0.0f, 0.0f, 20.0f),//�������λ��
		glm::vec3(0.0f, 0.0f, 0.0f), //�ӵ�λ��
		glm::vec3(0.0f, 1.0f, 0.0f) //�Ϸ���; 
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
	// ... ���ù۲��ͶӰ����
	glm::mat4 skyboxview = glm::lookAt(
		glm::vec3(20.0f, 0.0f, 00.0f),//�������λ��
		glm::vec3(0.0f, 0.0f, 0.0f),  //�ӵ�λ��
		glm::vec3(0.0f, 1.0f, 0.0f) //�Ϸ���; 
	);  //camera.GetViewMatrix() 
	//������������ƶ��˶�Զ����պж�������������Ҳ�����Χ�����ǳ����ӡ��
	skyboxview = glm::mat4(glm::mat3(skyboxview)); //�⽫�Ƴ��κε�λ�ƣ���������ת�任���������Ȼ�ܹ����˳�����
	glm::mat4 skyboxprojection = projection;
	// draw skybox as last
	//��ǰ��Ȳ���(Early Depth Testing)���� *����* ����Ƭ���ܹ���ʡ���Ǻܶ౦��Ĵ���
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
	//���ܲ�֧�֣� ������ȷ �����ļ���ȫӦ�� 
	//	��Ƭ������������





	glutSwapBuffers();
}

void idle()
{
	t = clock() / 50 % 360;   //360��һѭ������ʡ�԰ɣ� ���س���ִ����һ��Ϊ����Ŀ�ͷ����������ʱ����ʹ�õ�ʱ�䡣
	glm::vec3 lightColor;
	//sin(t * 2.0f) > 0 ? lightColor.x = sin(t * 2.0f) : lightColor.x = 0.5;
	//sin(t * 0.7f) > 0 ? lightColor.y = sin(t * 0.7f) : lightColor.y = 0.5;
	//sin(t * 1.3f) > 0 ? lightColor.z = sin(t * 1.3f) : lightColor.z = 0.5;
	lightColor.x = sin(t * 2.0f);
	lightColor.y = sin(t * 0.7f);
	lightColor.z = sin(t * 1.3f);
	glm::vec3  diffuseColor = lightColor * glm::vec3(0.5f); // ����Ӱ��
	glm::vec3  ambientColor = diffuseColor * glm::vec3(0.2f); // �ܵ͵�Ӱ��
	glUseProgram(program[0]);

	//cout << lightColor.x << " " << lightColor.y << " " << lightColor.z << endl;
	
	//Ϊʲô���ڣ� ��Ȼ�и�ֵ��������Ϊ��� �ٴ���һ������
	glUniform3f(glGetUniformLocation(program[0], "spotLight.ambient"), ambientColor.x, ambientColor.y, ambientColor.z);
	glUniform3f(glGetUniformLocation(program[0], "spotLight.diffuse"), diffuseColor.x, diffuseColor.y, diffuseColor.z);
	//glUniform3f(glGetUniformLocation(program[0], "spotLight.ambient"), 0.05f, 0.05f, 0.05f);
	//glUniform3f(glGetUniformLocation(program[0], "spotLight.diffuse"), 0.05f, 0.05f, 0.05f);

	glutPostRedisplay(); //�ػ溯�� ��ʲô���ã�
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
	int mainwnd = glutCreateWindow("���ϻ���������");
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
{//������������ͼ
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