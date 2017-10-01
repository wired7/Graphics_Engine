#include "GraphicsObject.h"

void DecoratedGraphicsObject::enableBuffers()
{
	glBindVertexArray(VAO);

	int min = 0;

	if(child)
	{
		min = child->layoutCount;
	}

	for (int i = min; i < layoutCount; i++)
	{
		glEnableVertexAttribArray(i);
	}
}

MeshObject::MeshObject()
{
	layoutCount = 3;
}

MeshObject::MeshObject(vector<Vertex> vertices, vector<GLuint> indices) : vertices(vertices), indices(indices)
{
	layoutCount = 3;
	bufferSignature = "VERTEX";
	bindBuffers();
}

void MeshObject::commitVBOToGPU()
{
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_DYNAMIC_DRAW);

	if (indices.size())
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_DYNAMIC_DRAW);
	}

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, position));
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, color));
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, normal));

	glBindVertexArray(0);
}

void MeshObject::bindBuffers(void)
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	commitVBOToGPU();
}

void MeshObject::updateBuffers(void)
{
	commitVBOToGPU();
}

void MeshObject::addVertex(vec3 pos, vec4 color, vec3 normal)
{
	vertices.push_back(Vertex(pos, normal, color));
}

void MeshObject::setLocalUniforms(void)
{

}

void MeshObject::draw(void)
{
	setLocalUniforms();
	enableBuffers();
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

template <class T> ExtendedMeshObject<T>::ExtendedMeshObject(DecoratedGraphicsObject* child, string bufferSignature, int divisor) :
	child(child), bufferSignature(bufferSignature), divisor(divisor)
{
	layoutCount = child->layoutCount + 1;
	VAO = child->VAO;
}

template <class T> ExtendedMeshObject<T>::ExtendedMeshObject(DecoratedGraphicsObject* child, vector<T> data, string bufferSignature, int divisor) :
	child(child), extendedData(data), bufferSignature(bufferSignature), divisor(divisor)
{
	layoutCount = child->layoutCount + 1;
	VAO = child->VAO;
	bindBuffers();
}

template <class T> void ExtendedMeshObject<T>::addVertex(vec3 pos, vec4 color, vec3 normal)
{
	child->addVertex(pos, color, normal);
}

template <class T> void ExtendedMeshObject<T>::commitVBOToGPU(void)
{
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, extendedData.size() * sizeof(T), &extendedData[0], GL_DYNAMIC_DRAW);

	glVertexAttribPointer(layoutCount - 1, sizeof(T) / sizeof(float), GL_FLOAT, GL_FALSE, sizeof(T), (GLvoid*)0);
	glVertexAttribDivisor(layoutCount - 1, divisor);

	glBindVertexArray(0);
}

template <class T> void ExtendedMeshObject<T>::bindBuffers(void)
{
	glGenBuffers(1, &VBO);

	commitVBOToGPU();
}

template <class T> void ExtendedMeshObject<T>::updateBuffers(void)
{
	commitVBOToGPU();
}

template <class T> void ExtendedMeshObject<T>::setLocalUniforms(void)
{

}

template <class T> void ExtendedMeshObject<T>::draw(void)
{
	setLocalUniforms();
	enableBuffers();
	child->draw();
}

InstancedMeshObject::InstancedMeshObject(MeshObject* mesh, vector<mat4> matrices, vector<vec4> colors) :
	instancedObject(mesh), matrices(matrices), colors(colors)
{
	if (this->matrices.size())
	{
		bindBuffers();
	}
}

void InstancedMeshObject::bindBuffers(void)
{
	// Set matrices buffer
	glBindVertexArray(instancedObject->VAO);
	glGenBuffers(1, &transformsBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, transformsBuffer);
	glBufferData(GL_ARRAY_BUFFER, matrices.size() * sizeof(glm::mat4), &(matrices[0]), GL_DYNAMIC_DRAW);

	// Matrix Columns
	GLsizei vec4Size = sizeof(glm::vec4);
	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (GLvoid*)0);
	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (GLvoid*)(vec4Size));
	glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (GLvoid*)(2 * vec4Size));
	glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (GLvoid*)(3 * vec4Size));

	// Divisors
	glVertexAttribDivisor(3, 1);
	glVertexAttribDivisor(4, 1);
	glVertexAttribDivisor(5, 1);
	glVertexAttribDivisor(6, 1);

	// Set colors buffer
	glGenBuffers(1, &colorsBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, colorsBuffer);
	glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(glm::vec4), &(colors[0]), GL_DYNAMIC_DRAW);

	// Color
	glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, vec4Size, (GLvoid*)0);

	// Divisor
	glVertexAttribDivisor(7, matrices.size() / colors.size());

	glBindVertexArray(0);

}

void InstancedMeshObject::enableBuffers(void)
{
	instancedObject->enableBuffers();
	glEnableVertexAttribArray(3);
	glEnableVertexAttribArray(4);
	glEnableVertexAttribArray(5);
	glEnableVertexAttribArray(6);
	glEnableVertexAttribArray(7);
}



void InstancedMeshObject::updateBuffers()
{
	glBindVertexArray(instancedObject->VAO);
	glBindBuffer(GL_ARRAY_BUFFER, transformsBuffer);
	glBufferData(GL_ARRAY_BUFFER, matrices.size() * sizeof(glm::mat4), &(matrices[0]), GL_DYNAMIC_DRAW);

	glBindVertexArray(0);
}

void InstancedMeshObject::draw()
{
	enableBuffers();
	setLocalUniforms();
	glDrawElementsInstanced(GL_TRIANGLES, instancedObject->indices.size(), GL_UNSIGNED_INT, 0, matrices.size());
	glBindVertexArray(0);
}

Polyhedron::Polyhedron(int resolution, vec3 pos, vec3 radii, vec4 color)
{
	model = scale(translate(mat4(1.0f), pos), radii);
	this->resolution = resolution;

	double theta = 2 * 3.1415 / resolution;
	double phi = 3.1415 / resolution;
	vector<vector<vec3>> circles;
	vec3 start(0, -0.5f, 0);
	for (int j = 1; j < resolution; j++)
	{
		vec3 temp = rotate(start, (GLfloat)(phi * j), vec3(0, 0, 1));
		circles.push_back(vector<vec3>());
		for (int i = 0; i < resolution; i++)
			circles[circles.size() - 1].push_back(rotate(temp, (GLfloat)(theta * i), vec3(0, 1, 0)));
	}

	for (int j = 0; j < circles.size(); j++)
	{
		for (int i = 0; i < circles[j].size(); i++)
		{
			addVertex(circles[j][i], color, normalize(circles[j][i]));

			if (j > 0 && i > 0)
			{
				indices.push_back(vertices.size() - 2 - circles[j].size());
				indices.push_back(vertices.size() - 2);
				indices.push_back(vertices.size() - 1 - circles[j].size());

				indices.push_back(vertices.size() - 2);
				indices.push_back(vertices.size() - 1);
				indices.push_back(vertices.size() - 1 - circles[j].size());
			}
		}

		if (j > 0)
		{
			indices.push_back(vertices.size() - 1 - circles[j].size());
			indices.push_back(vertices.size() - 1);
			indices.push_back(vertices.size() - 2 * circles[j].size());

			indices.push_back(vertices.size() - 1);
			indices.push_back(vertices.size() - 2 * circles[j].size());
			indices.push_back(vertices.size() - circles[j].size());
		}
	}

	addVertex(start, color, start);
	addVertex(-start, color, -start);

	for (int i = 0; i < resolution; i++)
	{
		indices.push_back(i);
		indices.push_back((i + 1) % resolution);
		indices.push_back(vertices.size() - 2);

		indices.push_back(vertices.size() - 2 - resolution + i);
		indices.push_back(vertices.size() - resolution + (i + 1) % resolution - 2);
		indices.push_back(vertices.size() - 1);
	}

	bindBuffers();
}

void Polyhedron::draw(void)
{
	enableBuffers();
	glUniformMatrix4fv(Shader::activeShader->modelID, 1, GL_FALSE, &model[0][0]);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
}