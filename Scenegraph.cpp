#include "Scenegraph.h"
#include <stack>
#include "TransformNode.h"
#include<iostream>
#define GLM_FORCE_RADIANS
using namespace std;
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>
#include <sstream>

Scenegraph::Scenegraph()
{
    root = NULL;
	nodes_found=false;
	cone_found=false;
	flag_found=false;
}

void Scenegraph::makeScenegraph(Node *r)
{
	nodes_found = false;
    if (root!=NULL)
    {
        delete root;
        root = NULL;
    }
    this->root = r;

	if (this->root!=NULL)
		this->root->setScenegraph(this);
}



Scenegraph::~Scenegraph()
{
    if (root!=NULL)
    {
        delete root;
        root = NULL;
    }

	for (map<string,graphics::Texture *>::iterator it=textures.begin();it!=textures.end();it++)
	{
		delete it->second;
	}
}

void Scenegraph::initShaderProgram(GLint shaderProgram)
{
  // objectColorLocation = glGetUniformLocation(shaderProgram, "vColor");
   modelviewLocation = glGetUniformLocation(shaderProgram,"modelview");
   normalMatrixLocation = glGetUniformLocation(shaderProgram, "normalmatrix");
  // texturematrixLocation = glGetUniformLocation(shaderProgram, "texturematrix"); broken...
   numLightsLocation = glGetUniformLocation(shaderProgram, "numLights");

   mat_ambientLocation = glGetUniformLocation(shaderProgram, "material.ambient");
   mat_diffuseLocation = glGetUniformLocation(shaderProgram, "material.diffuse");
   mat_specularLocation = glGetUniformLocation(shaderProgram, "material.specular");
   mat_shininessLocation = glGetUniformLocation(shaderProgram, "material.shininess");
   textureLocation = glGetUniformLocation(shaderProgram, "image");
   //sf::Image im;

   //im.loadFromFile("earthmap.png");

   ////origin of the image is top left, all our texture coordinates are with respect to bottom left, so flip image vertically
   //im.flipVertically();

   //glGenTextures(1, &textureID); //get a unique texture ID
   //glBindTexture(GL_TEXTURE_2D, textureID);// bind this texture as "current". All texture commands henceforth will apply to this ID

   //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
   //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
   //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
   //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
   //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, im.getSize().x, im.getSize().y, 0, GL_RGBA, GL_UNSIGNED_BYTE, im.getPixelsPtr());
}


void Scenegraph::draw(stack<glm::mat4>& modelView, GLint shaderProgram)
{

	if (root != NULL)
	{
		vector<graphics::Light> lightvector;
		root->getlights(lightvector, modelView);
		LightLocation lightLocation[5];

		for (int i = 0; i<lightvector.size(); i++)
		{
			stringstream name;

			name << "light[" << i << "].ambient";

			lightLocation[i].ambientLocation = glGetUniformLocation(shaderProgram, name.str().c_str());

			name.clear();//clear any bits set
			name.str(std::string());

			name << "light[" << i << "].diffuse";

			lightLocation[i].diffuseLocation = glGetUniformLocation(shaderProgram, name.str().c_str());

			name.clear();//clear any bits set
			name.str(std::string());

			name << "light[" << i << "].specular";

			lightLocation[i].specularLocation = glGetUniformLocation(shaderProgram, name.str().c_str());

			name.clear();//clear any bits set
			name.str(std::string());

			name << "light[" << i << "].position";

			lightLocation[i].positionLocation = glGetUniformLocation(shaderProgram, name.str().c_str());

			name.clear();//clear any bits set
			name.str(std::string());

		}
		glUniform1i(numLightsLocation, lightvector.size());

		//light properties
		for (int i = 0; i<lightvector.size(); i++)
		{
			glUniform3fv(lightLocation[i].ambientLocation, 1, glm::value_ptr(lightvector[i].getAmbient()));
			glUniform3fv(lightLocation[i].diffuseLocation, 1, glm::value_ptr(lightvector[i].getDiffuse()));
			glUniform3fv(lightLocation[i].specularLocation, 1, glm::value_ptr(lightvector[i].getSpecular()));
			glUniform4fv(lightLocation[i].positionLocation, 1, glm::value_ptr(lightvector[i].getPosition()));
		}
	}



	glEnable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_2D, textureID);
	//glUniform1i(textureLocation, 0); //bind GL_TEXTURE0 to sampler2D (whatever is bound to GL_TEXTURE0)

    if (root!=NULL)
    {
        root->draw(modelView);
    }

	/*if (root!=NULL)
	{
		root->updateBB();
		root->drawBB(modelView);
	}*/
}
void Scenegraph::animate_cone(int x, int y)
{
	if(cone_found==false)
	{
		cone = root->getNode("spot");
		cone_found=true;
	}
	TransformNode* t_ride= dynamic_cast<TransformNode*>(cone);
	if(t_ride!=NULL)
	{

		t_ride->setAnimationTransform(glm::translate(glm::mat4(1.0), glm::vec3(x, y, 0)));
	}
}

void Scenegraph::animate_flag(int x, int y)
{
	if(flag_found==false)
	{
		flag = root->getNode("flag");
		flag_found=true;
	}
	TransformNode* t_ride= dynamic_cast<TransformNode*>(flag);
	if(t_ride!=NULL)
	{

		t_ride->setAnimationTransform(glm::translate(glm::mat4(1.0), glm::vec3(x, y, 0)));

	}
}

void Scenegraph::animate(float time)
{
	
	if (nodes_found == false)
	{
		//only want to find them once to make the framerate better
		nodes_found = true;
		ride = root->getNode("ride_11");
		seat_0 = root->getNode("move_seat_0");
		seat_1 = root->getNode("move_seat_1");
		seat_2 = root->getNode("move_seat_2");
		seat_3 = root->getNode("move_seat_3");
		wheel = root->getNode("wheeltransform");
		j_seat_0 = root->getNode("seattransformone");
		j_seat_1 = root->getNode("seattransformtwo");
		j_seat_2 = root->getNode("seattransformthree");
		j_seat_3 = root->getNode("seattransformfour");
	}

	TransformNode* t_ride = dynamic_cast<TransformNode*>(ride);
	if (t_ride != NULL)
	{

		t_ride->setAnimationTransform(glm::rotate(glm::mat4(1.0), glm::radians(300.0f*time), glm::vec3(0, 1, 0)));
	}

	t_ride = dynamic_cast<TransformNode*>(seat_0);
	if (t_ride != NULL)
	{

		t_ride->setAnimationTransform(glm::rotate(glm::mat4(1.0), glm::radians(800.0f*time), glm::vec3(0, 1, 0)));
	}

	t_ride = dynamic_cast<TransformNode*>(seat_1);
	if (t_ride != NULL)
	{

		t_ride->setAnimationTransform(glm::rotate(glm::mat4(1.0), glm::radians(1000.0f*time), glm::vec3(0, 1, 0)));
	}

	t_ride = dynamic_cast<TransformNode*>(seat_2);
	if (t_ride != NULL)
	{

		t_ride->setAnimationTransform(glm::rotate(glm::mat4(1.0), glm::radians(1200.0f*time), glm::vec3(0, 1, 0)));
	}

	t_ride = dynamic_cast<TransformNode*>(seat_3);
	if (t_ride != NULL)
	{

		t_ride->setAnimationTransform(glm::rotate(glm::mat4(1.0), glm::radians(1800.0f*time), glm::vec3(0, 1, 0)));
	}



	TransformNode * twheel = dynamic_cast<TransformNode*> (wheel);

	if (twheel != NULL)
	{
		twheel->setAnimationTransform(glm::translate(glm::mat4(1.0), glm::vec3(0, 100, 0))*glm::rotate(glm::mat4(1.0), glm::radians(150.0f*time), glm::vec3(1, 0, 0))*glm::translate(glm::mat4(1.0), glm::vec3(0, -100, 0)));
	}

	TransformNode * tseat = dynamic_cast<TransformNode*> (j_seat_0);
	if (tseat != NULL)
	{
		tseat->setAnimationTransform(glm::rotate(glm::mat4(1.0), glm::radians(220.0f*time), glm::vec3(0, 1, 0)));
	}

	tseat = dynamic_cast<TransformNode*> (j_seat_1);
	if (tseat != NULL)
	{
		tseat->setAnimationTransform(glm::rotate(glm::mat4(1.0), glm::radians(220.0f*time), glm::vec3(0, 1, 0)));
	}

	tseat = dynamic_cast<TransformNode*> (j_seat_2);
	if (tseat != NULL)
	{
		tseat->setAnimationTransform(glm::rotate(glm::mat4(1.0), glm::radians(220.0f*time), glm::vec3(0, 1, 0)));
	}

	tseat = dynamic_cast<TransformNode*> (j_seat_3);
	if (tseat != NULL)
	{
		tseat->setAnimationTransform(glm::rotate(glm::mat4(1.0), glm::radians(220.0f*time), glm::vec3(0, 1, 0)));
	}
}

