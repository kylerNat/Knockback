#include "RenderingStructs.h"

vertexObject bindVO(vertexObject vO){
	glBindVertexArray(vO.vertexArrayObject);
	glBindBuffer(GL_ARRAY_BUFFER, vO.vertexBufferObject);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(0, 3, GL_FLOAT, 0, 0, 0);
	glVertexAttribPointer(1, 3, GL_FLOAT, 0, 0, (void*)((vO.data.vertexBytes)*1/2));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vO.indexBufferObject);
	return vO;
}