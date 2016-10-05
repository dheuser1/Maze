#ifndef _LEAF_H_
#define _LEAF_H_
#include "node.h"
#include "utils/Object.h"
#include "utils/Material.h"
#include <glm/gtc/matrix_transform.hpp>
#include "utils/Texture.h"


class LeafNode : public Node
{
public:

	LeafNode(graphics::Object *instanceOf,Scenegraph *graph,string name="")
		:Node(graph,name)
	{
		this->texture = NULL;
		this->instanceOf = instanceOf;
		//default material
		material.setAmbient(1.0f,0.6f,0.6f);
		material.setDiffuse(1.0f,0.6f,0.6f);
		material.setSpecular(0.2f,0.1f,0.1f);
		material.setShininess(1);
	}

	~LeafNode(void)
	{
	}

	Node *clone()
	{
		LeafNode *newclone = new LeafNode(instanceOf,scenegraph,name);
		newclone->setMaterial(material);

		return newclone;
	}
	virtual void getNodeAni(glm::mat4& modelView)
	{
		if (parent != NULL)
			parent->getNodeAni(modelView);
		return;

	}
	virtual void getlights(vector<graphics::Light>& lightvector, stack<glm::mat4> &modelView)
	{
		for (int i = 0; i < lights.size(); i++)
		{
			graphics::Light l;
			l.setAmbient(lights[i].getAmbient());
			l.setDiffuse(lights[i].getDiffuse());
			l.setPosition(lights[i].getPosition().x, lights[i].getPosition().y, lights[i].getPosition().z);
			l.setSpecular(lights[i].getSpecular());
			l.setSpotAngle(lights[i].getSpotAngle());
			l.setSpotDirection(glm::vec3(lights[i].getSpotDirection().x, lights[i].getSpotDirection().y, lights[i].getSpotDirection().z));
			glm::vec4 newpos = modelView.top()*l.getPosition();
			l.setPosition(newpos.x,newpos.y,newpos.z);
			lightvector.push_back(l);
			
		}
		
	}

	virtual void draw(stack<glm::mat4> &modelView)
    {
		GLuint a;
        if (instanceOf!=NULL)
		{
			 //get the color
           // glm::vec4 color = material.getAmbient();

            //set the color for all vertices to be drawn for this object
           // glUniform3fv(scenegraph->objectColorLocation,1,glm::value_ptr(material.getAmbient()));
			glUniform3fv(scenegraph->mat_ambientLocation, 1, glm::value_ptr(material.getAmbient()));
			glUniform3fv(scenegraph->mat_diffuseLocation, 1, glm::value_ptr(material.getDiffuse()));
			glUniform3fv(scenegraph->mat_specularLocation, 1, glm::value_ptr( material.getSpecular()));
			glUniform1f(scenegraph->mat_shininessLocation, material.getShininess());
			glUniformMatrix4fv(scenegraph->modelviewLocation,1,GL_FALSE,glm::value_ptr(modelView.top()));
			glUniformMatrix4fv(scenegraph->normalMatrixLocation, 1, GL_FALSE, glm::value_ptr(glm::transpose(glm::inverse(modelView.top() * instanceOf->getTransform()))));
			if (texture == NULL)
			{
				glBindTexture(GL_TEXTURE_2D, 1);
			}
			else{

				glBindTexture(GL_TEXTURE_2D, texture->getTextureID());
			}
			
			//glUniformMatrix4fv(scenegraph->texturematrixLocation, 1, GL_FALSE, glm::value_ptr(instanceOf->getTextureTransform()));
			glUniform1i(scenegraph->textureLocation, 0); //bind GL_TEXTURE0 to sampler2D (whatever is bound to GL_TEXTURE0)


			instanceOf->draw(GL_TRIANGLES);        			
		}
    }

	virtual void drawBB(stack<glm::mat4>& modelView)
	{
		if (bbDraw)
		{
			glm::mat4 bbTransform;

			bbTransform = glm::translate(glm::mat4(1.0),0.5f*(minBounds+maxBounds)) * glm::scale(glm::mat4(1.0),maxBounds-minBounds);
			glm::vec4 color = glm::vec4(1,1,1,1);
			//set the color for all vertices to be drawn for this object
			glUniform3fv(scenegraph->objectColorLocation,1,glm::value_ptr(color));
			glUniformMatrix4fv(scenegraph->modelviewLocation,1,GL_FALSE,glm::value_ptr(modelView.top() * bbTransform));
			scenegraph->getInstance("box")->draw(GL_TRIANGLES);        		
		}
	}
	
	virtual void updateBB()
	{
		minBounds = instanceOf->getMinimumWorldBounds().xyz();
		maxBounds = instanceOf->getMaximumWorldBounds().xyz();
	}

	glm::vec4 getColor()
	{
		return material.getAmbient();
	}

	/*
		*Set the material of each vertex in this object
		*/
	virtual void setMaterial(graphics::Material& mat)
	{
		material = mat;
	}

	/*
		* gets the material
		*/
	graphics::Material getMaterial()
	{
		return material;
	}

	virtual void setTexture(graphics::Texture *tex)
	{
		texture = tex;
	}
protected:
	graphics::Object *instanceOf;
	graphics::Material material;
	graphics::Texture *texture;
};
#endif
