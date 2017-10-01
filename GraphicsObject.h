#pragma once
#include "Shader.h"
#include <vector>
#include <glew.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtx/rotate_vector.hpp>

using namespace glm;
using namespace std;

// Make a factory to avoid creating erroneous patterns!
// TODO: Add a uniform references array that somehow links to the shader

struct Vertex {
	glm::vec3 position;
	glm::vec4 color;
	glm::vec3 normal;

	Vertex() {};
	Vertex(vec3 pos, vec3 norm, vec4 col) : position(pos), normal(norm), color(col) {};
};

class DecoratedGraphicsObject
{
public:
	DecoratedGraphicsObject* child;
	GLuint VAO;
	GLuint VBO;
	bool dirty = false;
	int layoutCount;
	string bufferSignature;
	DecoratedGraphicsObject() {};
	~DecoratedGraphicsObject() {};
	// TODO: Recursive Lookup of signature through children
	virtual bool signatureLookup(string signature);
	virtual bool signatureLookup(vector<string> signatures);
	// TODO: Look for buffer with signature and call the function at occurrence
	virtual bool signatureCallback(string signature/*, function callback*/);
	virtual bool signatureCalback(vector<string> signatures/*, function callback*/);
	// TODO: Look for the signature recursively, add it on top of signature owner, reorganize layout indices and bind all shifted buffers, including the new one
	virtual DecoratedGraphicsObject* addOnTopOf(string bufferSignature);
	// TODO: Look for the signature recursively, unbind it from the VAO, delete VBO, return its child, and reorganize layout indices from any child and parent
	// so that they're sequential, and rebind every buffer that was shifted to the proper layout number
	virtual DecoratedGraphicsObject* remove(string bufferSignature);
	virtual DecoratedGraphicsObject* remove(vector<string> bufferSignatures);
	virtual void commitVBOToGPU(void) = 0;
	virtual void bindBuffers(void) = 0;
	virtual void updateBuffers(void) = 0;
	// TODO: Assign signatures to buffers so as to allow updates to be applied cross-signature and discontinuously, and not necessarily hollistically
	virtual void updateBuffers(vector<string> bufferSignatures);
	virtual void updateBuffersPartially(int minBufferIndex, int maxBufferIndex);
	virtual void updateBuffersPartially(int minBufferIndex, int maxBufferIndex, vector<string> bufferSignatures);
	virtual void enableBuffers(void);
	virtual void setLocalUniforms(void) = 0;
	virtual void draw() = 0;
};

class MeshObject : public DecoratedGraphicsObject
{
public:
	mat4 model;
	vector<Vertex> vertices;
	vector<GLuint> indices;
	GLuint EBO;

	MeshObject();
	MeshObject(vector<Vertex> vertices, vector<GLuint> indices);
	~MeshObject() {};

	virtual void addVertex(vec3 pos, vec4 color, vec3 normal);
	virtual void addTriangle(int a, int b, int c);
	// TODO: apply vertex deletion, but careful! when a vertex is deleted from the buffer, all of the indices change, and it's necessary to also
	// remove any triangles that may refer to that vertex index first
	virtual void deleteVertex(int index);
	// TODO: apply triangle deletion by looking for matching triples in any order within the indices array
	virtual void deleteTriangle(int a, int b, int c);
	virtual void commitVBOToGPU(void) = 0;
	virtual void bindBuffers(void);
	virtual void updateBuffers(void);
	virtual void setLocalUniforms(void);
	virtual void draw();
};

template <class T> class ExtendedMeshObject<T> : DecoratedGraphicsObject
{
	vector<T> extendedData;
	ExtendedMeshObject(DecoratedGraphicsObject* child, string bufferSignature, int divisor = 1);
	ExtendedMeshObject(DecoratedGraphicsObject* child, vector<T> data, string bufferSignature, int divisor = 1);
	~ExtendedMeshObject() {};
	virtual void commitVBOToGPU(void);
	virtual void bindBuffers(void);
	virtual void updateBuffers(void);
	virtual void setLocalUniforms(void);
	virtual void draw();
};

class TexturedMeshObject : public ExtendedMeshObject<vec2>
{
	TexturedMeshObject(DecoratedGraphicsObject* child, string filename, int divisor = 1);
	TexturedMeshObject(DecoratedGraphicsObject* child, string filename, vector<vec2> data, int divisor = 1);
	~TexturedMeshObject() {};
	virtual void commitVBOToGPU(void);
};

template <class T> class InstancedMeshObject<T> : public ExtendedMeshObject<T>
{
public:
	InstancedMeshObject(DecoratedGraphicsObject* mesh, int divisor = 1);
	InstancedMeshObject(DecoratedGraphicsObject* mesh, vector<T> data, int divisor = 1);
	~InstancedMeshObject() {};
	virtual void commitVBOToGPU(void);
	virtual void bindBuffers(void);
	virtual void updateBuffers(void);
	virtual void draw();
};

class Polyhedron : public MeshObject
{
public:
	int resolution;
	Polyhedron() {};
	Polyhedron(int, vec3, vec3, vec4);
	void draw();
};